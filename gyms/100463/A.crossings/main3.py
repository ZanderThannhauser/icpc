
case = 1
n, a, b = map(int, input().split());

while n:
	numbers = [(a * i + b) % n for i in range(n)];
	print(numbers);
	
	count = 0;
	i = 0;
	while i < n:
		while i != numbers[i]:
			numbers[numbers[i]], numbers[i] = numbers[i], numbers[numbers[i]];
			print(numbers);
			count += 1;
		i += 1;
	
	print(f"case {case}: {count}");
	n, a, b = map(int, input().split());
	case += 1;

