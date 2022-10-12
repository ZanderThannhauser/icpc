
case = 1
n, a, b = map(int, input().split());

while n:
	numbers = [(a * i + b) % n for i in range(n)];
	print(numbers);
	
	count = 0;
	s = set(numbers);
	for n in numbers:
		if min(s) < n:
			count += sum(e < n for e in s);
		s.discard(n);
	
	print(f"case {case}: {count}");
	n, a, b = map(int, input().split());
	case += 1;

