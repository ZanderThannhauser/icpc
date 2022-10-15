
def check(n, a):
	# print(a);
	
	pos = list();
	
	for i in range(n):
		if a[i] > 0:
			pos.append(i);
	
	sums = dict(); # (i, j): sum
	maxs = dict(); # (i, j): max
	
	for i in range(n):
		sums[(i, i)] = a[i];
		maxs[(i, i)] = a[i];
	
	def calc_max(i, j):
		if (i, j) not in maxs:
			m = (i + j) // 2;
			maxs[(i, j)] = max(calc_max(i, m), calc_max(m + 1, j));
		return maxs[(i, j)];
	
	def calc_sum(i, j):
		if (i, j) not in sums:
			s = (i + j) // 2;
			sums[(i, j)] = calc_sum(i, s) + calc_sum(s + 1, j);
		return sums[(i, j)];
	
	for i in range(len(pos)):
		for j in range(i + 1, len(pos)):
			i = pos[i]
			j = pos[j]
			if not (calc_max(i, j) >= calc_sum(i, j)):
				return False;
	return True;

for _ in range(int(input())):
	n = int(input());
	a = [int(e) for e in input().split()];
	if check(n, a):
		print("YES");
	else:
		print("NO");












