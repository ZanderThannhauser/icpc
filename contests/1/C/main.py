
from math import sqrt;

def det2(a, b, c, d):
	return a * d - b * c;

def det3(a, b, c, d, e, f, g, h, i):
	return 0 \
		+ a * det2(e, f, h, i) \
		- b * det2(d, f, g, i) \
		+ c * det2(d, e, g, h);

x1, y1 = map(float, input().split());
x2, y2 = map(float, input().split());
x3, y3 = map(float, input().split());

h1 = x1 ** 2 + y1 ** 2;
h2 = x2 ** 2 + y2 ** 2;
h3 = x3 ** 2 + y3 ** 2;

m11 = det3(x1, y1,  1, x2, y2,  1, x3, y3,  1);
m12 = det3(h1, y1,  1, h2, y2,  1, h3, y3,  1);
m13 = det3(h1, x1,  1, h2, x2,  1, h3, x3,  1);
m14 = det3(h1, x1, y1, h2, x2, y2, h3, x3, y3);

x0 = m12 / m11 / +2;
y0 = m13 / m11 / -2;

r = sqrt(x0**2 + y0**2 + m14 / m11);

print(x0, y0, r);


















