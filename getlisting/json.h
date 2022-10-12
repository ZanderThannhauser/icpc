
#include <stdio.h>

struct zebu_token
{
	unsigned char* data;
	unsigned len, refcount;
};

struct zebu_$start
{
	struct zebu_value* value;
	unsigned refcount;
};

struct zebu_keyvalue
{
	char* key;
	struct zebu_value* value;
	unsigned refcount;
};

struct zebu_value
{
	struct zebu_token* decimal;
	struct zebu_token* dict;
	struct {
		struct zebu_value** data;
		unsigned n, cap;
	} elements;
	struct {
		struct zebu_keyvalue** data;
		unsigned n, cap;
	} entries;
	char* integer;
	struct zebu_token* list;
	char* string;
	unsigned refcount;
};



extern struct zebu_token* inc_zebu_token(struct zebu_token* token);
extern struct zebu_$start* inc_zebu_$start(struct zebu_$start* ptree);
extern struct zebu_keyvalue* inc_zebu_keyvalue(struct zebu_keyvalue* ptree);
extern struct zebu_value* inc_zebu_value(struct zebu_value* ptree);


extern void free_zebu_token(struct zebu_token* token);
extern void free_zebu_$start(struct zebu_$start* ptree);

extern void free_zebu_keyvalue(struct zebu_keyvalue* ptree);

extern void free_zebu_value(struct zebu_value* ptree);



struct zebu_$start* zebu_parse(FILE* stream);
