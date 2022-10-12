
case = 1
n, a, b = map(int, input().split());

while (n):
	places = 0;
	t = n;
	while t:
		places += 1;
		t >>= 1;
	
	size = (1 << (places + 1)) - 1;
	
	counts = [0] * size;
	
	count = 0;
	
	for i in range(n):
		num = (a * i + b) % n;
		
		r = 2;
		for i in range(1, places+1):
			q = num >> (places - i);
			if (not (1 & q)):
				count += counts[r + q + 1];
			r <<= 1;
		
		r = 1;
		for i in range(places+1):
			counts[r + (num >> (places - i))] += 1;
			r <<= 1;
	
	print(f"Case {case}: {count}");
	
	case += 1;
	n, a, b = map(int, input().split());


















