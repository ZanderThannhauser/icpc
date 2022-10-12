
case = 1
n, a, b = map(int, input().split());

while n:
	numbers = [(a * i + b) % n for i in range(n)];
	print(numbers);
	
	count = 0;
	for i, n in enumerate(numbers):
		count += abs(n - i);
	
	print(f"case {case}: {count}");
	n, a, b = map(int, input().split());
	case += 1;

