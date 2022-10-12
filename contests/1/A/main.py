
n, m, a = map(int, input().split());

x = n // a;
y = m // a;

if n % a: x += 1;
if m % a: y += 1;

print(x * y);

