
case = 1
n, a, b = map(int, input().split());

while n:
	numbers = [(a * i + b) % n for i in range(n)];
	print(numbers);
	
#	count = 0;
#	changed = True;
#	while changed:
#		changed = False;
#		for i in range(n-1):
#			if numbers[i] > numbers[i + 1]:
#				numbers[i + 1], numbers[i] = numbers[i], numbers[i + 1];
#				changed = True;
#				count += 1;
#	
#	print(f"case {case}: {count}");
	
	count = 0;
	for i in range(n):
		for j in range(i):
			if numbers[j] > numbers[i]:
				count += 1;
	
	print(f"case {case}: {count}");
	
	count = 0;
	s = list();
	for i in range(n):
		if numbers[i] > i:
			s.append(numbers[i]);
		elif i in s:
			s.remove(i);
		count += len(s);
	
	print(f"case {case}: {count}");
	
#	count = 0;
#	for i in range(n):
#		for j in range(n):
#			if numbers[j] > numbers[i]:
#				count += 1;
#	
#	print(f"case {case}: {count}");
	
	n, a, b = map(int, input().split());
	case += 1;












