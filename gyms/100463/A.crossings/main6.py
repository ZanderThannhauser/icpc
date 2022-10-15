
case = 1
n, a, b = map(int, input().split());

while n:
	count = 0;
	s = set();
	for i in range(n):
		x = (a * i + b) % n
		count += sum([e > x for e in s]);
		s.add(x);
	print(f"Case {case}: {count}");
	case += 1;
	n, a, b = map(int, input().split());
