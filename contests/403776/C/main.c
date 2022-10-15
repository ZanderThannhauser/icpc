
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

/* We need either depths, counts or both (the latter being the default) */
#if !defined(AVL_DEPTH) && !defined(AVL_COUNT)
#define AVL_DEPTH
#define AVL_COUNT
#endif

/* User supplied function to compare two items like strcmp() does.
 * For example: cmp(a,b) will return:
 *   -1  if a < b
 *    0  if a = b
 *    1  if a > b
 */
typedef int (*avl_compare_t)(const void *, const void *);

/* User supplied function to delete an item when a node is free()d.
 * If NULL, the item is not free()d.
 */
typedef void (*avl_freeitem_t)(void *);

typedef struct avl_node_t {
	struct avl_node_t *next;
	struct avl_node_t *prev;
	struct avl_node_t *parent;
	struct avl_node_t *left;
	struct avl_node_t *right;
	void *item;
#ifdef AVL_COUNT
	unsigned int count;
#endif
#ifdef AVL_DEPTH
	unsigned char depth;
#endif
} avl_node_t;

typedef struct avl_tree_t {
	avl_node_t *head;
	avl_node_t *tail;
	avl_node_t *top;
	avl_compare_t cmp;
	avl_freeitem_t freeitem;
} avl_tree_t;


static void avl_rebalance(avl_tree_t *, avl_node_t *);

static avl_node_t *avl_insert_before(avl_tree_t *, avl_node_t *old, avl_node_t *new);

static avl_node_t *avl_insert_after(avl_tree_t *, avl_node_t *old, avl_node_t *new);

#ifdef AVL_COUNT
#define NODE_COUNT(n)  ((n) ? (n)->count : 0)
#define L_COUNT(n)     (NODE_COUNT((n)->left))
#define R_COUNT(n)     (NODE_COUNT((n)->right))
#define CALC_COUNT(n)  (L_COUNT(n) + R_COUNT(n) + 1)
#endif

#ifdef AVL_DEPTH
#define NODE_DEPTH(n)  ((n) ? (n)->depth : 0)
#define L_DEPTH(n)     (NODE_DEPTH((n)->left))
#define R_DEPTH(n)     (NODE_DEPTH((n)->right))
#define CALC_DEPTH(n)  ((L_DEPTH(n)>R_DEPTH(n)?L_DEPTH(n):R_DEPTH(n)) + 1)
#endif

#ifndef AVL_DEPTH
/* Also known as ffs() (from BSD) */
static int lg(unsigned int u) {
	int r = 1;
	if(!u) return 0;
	if(u & 0xffff0000) { u >>= 16; r += 16; }
	if(u & 0x0000ff00) { u >>= 8; r += 8; }
	if(u & 0x000000f0) { u >>= 4; r += 4; }
	if(u & 0x0000000c) { u >>= 2; r += 2; }
	if(u & 0x00000002) r++;
	return r;
}
#endif

static int avl_check_balance(avl_node_t *avlnode) {
#ifdef AVL_DEPTH
	int d;
	d = R_DEPTH(avlnode) - L_DEPTH(avlnode);
	return d<-1?-1:d>1?1:0;
#else
/*	int d;
 *	d = lg(R_COUNT(avlnode)) - lg(L_COUNT(avlnode));
 *	d = d<-1?-1:d>1?1:0;
 */
#ifdef AVL_COUNT
	int pl, r;

	pl = lg(L_COUNT(avlnode));
	r = R_COUNT(avlnode);

	if(r>>pl+1)
		return 1;
	if(pl<2 || r>>pl-2)
		return 0;
	return -1;
#else
#error No balancing possible.
#endif
#endif
}

static int avl_search_closest(const avl_tree_t *avltree, const void *item, avl_compare_t cmp, avl_node_t **avlnode) {
	avl_node_t *node;
	int c;

	if (!avlnode)
		avlnode = &node;

	node = avltree->top;

	if (!node)
		return *avlnode = NULL, 0;

	for (;;) {
		c = cmp(item, node->item);

		if(c < 0) {
			if(node->left)
				node = node->left;
			else
				return *avlnode = node, -1;
		} else if(c > 0) {
			if(node->right)
				node = node->right;
			else
				return *avlnode = node, 1;
		} else {
			return *avlnode = node, 0;
		}
	}
}

/*
 * avl_search:
 * Return a pointer to a node with the given item in the tree.
 * If no such item is in the tree, then NULL is returned.
 */
static avl_node_t *avl_search(const avl_tree_t *avltree, const void *item) {
	avl_node_t *node;
	return avl_search_closest(avltree, item, avltree->cmp, &node) ? NULL : node;
}

static avl_tree_t *avl_init_tree(avl_tree_t *rc, avl_compare_t cmp, avl_freeitem_t freeitem) {
	if (rc) {
		rc->head = NULL;
		rc->tail = NULL;
		rc->top = NULL;
		rc->cmp = cmp;
		rc->freeitem = freeitem;
	}
	return rc;
}

static avl_tree_t *avl_alloc_tree(avl_compare_t cmp, avl_freeitem_t freeitem) {
	return avl_init_tree(malloc(sizeof(avl_tree_t)), cmp, freeitem);
}

static void avl_clear_tree(avl_tree_t *avltree) {
	avltree->top = avltree->head = avltree->tail = NULL;
}

static void avl_free_nodes(avl_tree_t *avltree) {
	avl_node_t *node, *next;
	avl_freeitem_t freeitem;

	freeitem = avltree->freeitem;

	for(node = avltree->head; node; node = next) {
		next = node->next;
		if(freeitem)
			freeitem(node->item);
		free(node);
	}
	
	avl_clear_tree(avltree);
}

/*
 * avl_free_tree:
 * Free all memory used by this tree.  If freeitem is not NULL, then
 * it is assumed to be a destructor for the items referenced in the avl_
 * tree, and they are deleted as well.
 */
static void avl_free_tree(avl_tree_t *avltree) {
	if (avltree) {
		avl_free_nodes(avltree);
		free(avltree);
	}
}

static void avl_clear_node(avl_node_t *newnode) {
	newnode->left = newnode->right = NULL;
	#ifdef AVL_COUNT
	newnode->count = 1;
	#endif
	#ifdef AVL_DEPTH
	newnode->depth = 1;
	#endif
}

static avl_node_t *avl_init_node(avl_node_t *newnode, void *item) {
	if(newnode) {
/*		avl_clear_node(newnode); */
		newnode->item = item;
	}
	return newnode;
}

static avl_node_t *avl_insert_top(avl_tree_t *avltree, avl_node_t *newnode) {
	avl_clear_node(newnode);
	newnode->prev = newnode->next = newnode->parent = NULL;
	avltree->head = avltree->tail = avltree->top = newnode;
	return newnode;
}

static avl_node_t *avl_insert_before(avl_tree_t *avltree, avl_node_t *node, avl_node_t *newnode) {
	if(!node)
		return avltree->tail
			? avl_insert_after(avltree, avltree->tail, newnode)
			: avl_insert_top(avltree, newnode);

	if(node->left)
		return avl_insert_after(avltree, node->prev, newnode);

	avl_clear_node(newnode);

	newnode->next = node;
	newnode->parent = node;

	newnode->prev = node->prev;
	if(node->prev)
		node->prev->next = newnode;
	else
		avltree->head = newnode;
	node->prev = newnode;

	node->left = newnode;
	avl_rebalance(avltree, node);
	return newnode;
}

static avl_node_t *avl_insert_after(avl_tree_t *avltree, avl_node_t *node, avl_node_t *newnode) {
	if(!node)
		return avltree->head
			? avl_insert_before(avltree, avltree->head, newnode)
			: avl_insert_top(avltree, newnode);

	if(node->right)
		return avl_insert_before(avltree, node->next, newnode);

	avl_clear_node(newnode);

	newnode->prev = node;
	newnode->parent = node;

	newnode->next = node->next;
	if(node->next)
		node->next->prev = newnode;
	else
		avltree->tail = newnode;
	node->next = newnode;

	node->right = newnode;
	avl_rebalance(avltree, node);
	return newnode;
}

static avl_node_t *avl_insert_node(avl_tree_t *avltree, avl_node_t *newnode) {
	avl_node_t *node;

	if(!avltree->top)
		return avl_insert_top(avltree, newnode);

	switch(avl_search_closest(avltree, newnode->item, avltree->cmp, &node)) {
		case -1:
			return avl_insert_before(avltree, node, newnode);
		case 1:
			return avl_insert_after(avltree, node, newnode);
	}

	return NULL;
}

/*
 * avl_insert:
 * Create a new node and insert an item there.
 * Returns the new node on success or NULL if no memory could be allocated.
 */
static avl_node_t *avl_insert(avl_tree_t *avltree, void *item) {
	avl_node_t *newnode;

	newnode = avl_init_node(malloc(sizeof(avl_node_t)), item);
	if(newnode) {
		if(avl_insert_node(avltree, newnode))
			return newnode;
		free(newnode);
		errno = EEXIST;
	}
	return NULL;
}

/*
 * avl_rebalance:
 * Rebalances the tree if one side becomes too heavy.  This function
 * assumes that both subtrees are AVL-trees with consistant data.  The
 * function has the additional side effect of recalculating the count of
 * the tree at this node.  It should be noted that at the return of this
 * function, if a rebalance takes place, the top of this subtree is no
 * longer going to be the same node.
 */
static void avl_rebalance(avl_tree_t *avltree, avl_node_t *avlnode) {
	avl_node_t *child;
	avl_node_t *gchild;
	avl_node_t *parent;
	avl_node_t **superparent;

	parent = avlnode;

	while(avlnode) {
		parent = avlnode->parent;

		superparent = parent
			? avlnode == parent->left ? &parent->left : &parent->right
			: &avltree->top;

		switch(avl_check_balance(avlnode)) {
		case -1:
			child = avlnode->left;
			#ifdef AVL_DEPTH
			if(L_DEPTH(child) >= R_DEPTH(child)) {
			#else
			#ifdef AVL_COUNT
			if(L_COUNT(child) >= R_COUNT(child)) {
			#else
			#error No balancing possible.
			#endif
			#endif
				avlnode->left = child->right;
				if(avlnode->left)
					avlnode->left->parent = avlnode;
				child->right = avlnode;
				avlnode->parent = child;
				*superparent = child;
				child->parent = parent;
				#ifdef AVL_COUNT
				avlnode->count = CALC_COUNT(avlnode);
				child->count = CALC_COUNT(child);
				#endif
				#ifdef AVL_DEPTH
				avlnode->depth = CALC_DEPTH(avlnode);
				child->depth = CALC_DEPTH(child);
				#endif
			} else {
				gchild = child->right;
				avlnode->left = gchild->right;
				if(avlnode->left)
					avlnode->left->parent = avlnode;
				child->right = gchild->left;
				if(child->right)
					child->right->parent = child;
				gchild->right = avlnode;
				if(gchild->right)
					gchild->right->parent = gchild;
				gchild->left = child;
				if(gchild->left)
					gchild->left->parent = gchild;
				*superparent = gchild;
				gchild->parent = parent;
				#ifdef AVL_COUNT
				avlnode->count = CALC_COUNT(avlnode);
				child->count = CALC_COUNT(child);
				gchild->count = CALC_COUNT(gchild);
				#endif
				#ifdef AVL_DEPTH
				avlnode->depth = CALC_DEPTH(avlnode);
				child->depth = CALC_DEPTH(child);
				gchild->depth = CALC_DEPTH(gchild);
				#endif
			}
		break;
		case 1:
			child = avlnode->right;
			#ifdef AVL_DEPTH
			if(R_DEPTH(child) >= L_DEPTH(child)) {
			#else
			#ifdef AVL_COUNT
			if(R_COUNT(child) >= L_COUNT(child)) {
			#else
			#error No balancing possible.
			#endif
			#endif
				avlnode->right = child->left;
				if(avlnode->right)
					avlnode->right->parent = avlnode;
				child->left = avlnode;
				avlnode->parent = child;
				*superparent = child;
				child->parent = parent;
				#ifdef AVL_COUNT
				avlnode->count = CALC_COUNT(avlnode);
				child->count = CALC_COUNT(child);
				#endif
				#ifdef AVL_DEPTH
				avlnode->depth = CALC_DEPTH(avlnode);
				child->depth = CALC_DEPTH(child);
				#endif
			} else {
				gchild = child->left;
				avlnode->right = gchild->left;
				if(avlnode->right)
					avlnode->right->parent = avlnode;
				child->left = gchild->right;
				if(child->left)
					child->left->parent = child;
				gchild->left = avlnode;
				if(gchild->left)
					gchild->left->parent = gchild;
				gchild->right = child;
				if(gchild->right)
					gchild->right->parent = gchild;
				*superparent = gchild;
				gchild->parent = parent;
				#ifdef AVL_COUNT
				avlnode->count = CALC_COUNT(avlnode);
				child->count = CALC_COUNT(child);
				gchild->count = CALC_COUNT(gchild);
				#endif
				#ifdef AVL_DEPTH
				avlnode->depth = CALC_DEPTH(avlnode);
				child->depth = CALC_DEPTH(child);
				gchild->depth = CALC_DEPTH(gchild);
				#endif
			}
		break;
		default:
			#ifdef AVL_COUNT
			avlnode->count = CALC_COUNT(avlnode);
			#endif
			#ifdef AVL_DEPTH
			avlnode->depth = CALC_DEPTH(avlnode);
			#endif
		}
		avlnode = parent;
	}
}

int main()
{
	signed* numbers = NULL;
	unsigned cap = 0;
	
	unsigned t;
	scanf("%u", &t);
	while (t--)
	{
		unsigned m;
		scanf("%u", &m);
		
		if (m > cap)
			cap = m, numbers = realloc(numbers, sizeof(*numbers) * cap);
		
		bool consecutive_pos = false;
		
		signed sum = 0;
		unsigned n = 0;
		for (unsigned i = 0; i < m; i++)
		{
			signed e;
			scanf(" %i", &e);
			
			if (e > 0)
			{
				if (sum)
					numbers[n++] = sum, sum = 0;
				else
					consecutive_pos = true;
					
				numbers[n++] = e;
			}
			else
				sum += e;
		}
		
		if (sum)
			numbers[n++] = sum;
		
		if (consecutive_pos)
		{
			puts("NO");
		}
		else
		{
			bool result = true;
			
			struct node
			{
				unsigned i, j;
				signed sum, max;
			};
			
			struct node* new(unsigned i, unsigned j, signed sum, signed max)
			{
				struct node* this = malloc(sizeof(*this));
				this->i = i, this->j = j;
				this->sum = sum;
				this->max = max;
				return this;
			}
			
			int compare(struct node* a, struct node* b)
			{
				if (a->i > b->i)
					return +1;
				if (a->i < b->i)
					return -1;
				if (a->j > b->j)
					return +1;
				if (a->j < b->j)
					return -1;
				return 0;
			}
			
			struct avl_tree_t* tree = avl_alloc_tree((void*) compare, free);
			
			struct node* calc(unsigned i, unsigned j)
			{
				// printf("calc(%u, %u);\n", i, j);
				
				struct avl_node_t* node = avl_search(tree, &(struct node) {i, j});
				
				if (node)
				{
					return node->item;
				}
				else
				{
					unsigned m = (i + j) / 2;
					struct node* left = calc(i, m);
					struct node* right = calc(m + 1, j);
					struct node* node = new(i, j,
						left->sum + right->sum,
						left->max > right->max ? left->max : right->max);
					avl_insert(tree, node);
					return node;
				}
			}
			
			for (unsigned i = 0; i < n; i++)
				avl_insert(tree, new(i, i, numbers[i], numbers[i]));
			
			for (unsigned i = numbers[0] < 0; i < n; i += 2)
			{
				for (unsigned j = i + 2; j < n; j += 2)
				{
					struct node* node = calc(i, j);
					
					// printf("%u-%u: sum = %i, max = %i\n", i, j, node->sum, node->max);
					
					if (node->max < node->sum)
					{
						result = false;
						goto done;
					}
				}
			}
			
			done:
			puts(result ? "YES" : "NO");
			
			avl_free_tree(tree);
		}
		
	}
	
	free(numbers);
	return 0;
}















