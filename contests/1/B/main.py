
def letters(x):
	digits = [];
	while x:
		digits.append(x % 26);
		x //= 26;
	carry = 0;
	for i in range(len(digits)):
		if carry:
			digits[i] -= carry;
			carry = 0;
		if digits[i] == 0 and i + 1 == len(digits):
			digits.pop();
		else:
			while digits[i] <= 0:
				digits[i] += 26;
				carry += 1;
	s = "";
	for d in digits:
		assert(d);
		s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[d - 1] + s;
	return s;

def numbers(s):
	digits = [];
	for c in s:
		digits = [ord(c) - ord('A') + 1] + digits;
	carry = 0;
	for i in range(len(digits)):
		if carry:
			digits[i] += carry;
			carry = 0;
		while digits[i] >= 26:
			digits[i] -= 26;
			carry += 1;
	if carry:
		digits.append(carry);
	x = 0;
	for d in digits[::-1]:
		x = x * 26 + d;
	return x;

for _ in range(int(input())):
	line = input();
	l = len(line);
	if line[0] == 'R' and line[1] in "0123456789" and "C" in line:
		i = 1;
		
		x = 0;
		while line[i] in "0123456789":
			x = (ord(line[i]) - ord('0')) + x * 10;
			i += 1;
		
		i += 1;
		y = 0;
		while i < l and line[i] in "0123456789":
			y = (ord(line[i]) - ord('0')) + y * 10;
			i += 1;
		
		print(f"{letters(y)}{x}");
	else:
		i = 0;
		while line[i] in "ABCDEFGHIJKLMNOPQRSTUVWXYZ":
			i += 1;
		x = int(line[i:]);
		y = numbers(line[:i]);
		print(f"R{x}C{y}");






