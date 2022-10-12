
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

// |                0:n                    |
// |      0:n/2      |         n/2:n       |
// | 0:n/4 | n/4:n/2 | n/2:3*n/4 | 3*n/4:n |
// |                ...                    |
// | 0 | 1 | 2      ...    n-3 | n-2 | n-1 |

static uint64_t counts[2097151];

int main()
{
	uint64_t round = 1, n, a, b;
	scanf("%"PRIu64" %"PRIu64" %"PRIu64"", &n, &a, &b);
	
	while (n)
	{
		uint64_t places = 0;
		uint64_t t = n;
		while (t) places++, t /= 2;
		
		uint64_t size = (1 << (places + 1)) - 1;
		
		memset(counts, 0, sizeof(*counts) * size);
		
		uint64_t count = 0;
		
		for (uint64_t i = 0; i < n; i++)
		{
			uint64_t num = (a * i + b) % n;
			
			for (uint64_t i = 1, r = 2; i <= places; i++, r <<= 1)
			{
				uint64_t q = num >> (places - i);
				
				if (!(1 & q)) count += counts[r + q + 1];
			}
			
			for (uint64_t i = 0, r = 1; i <= places; i++, r <<= 1)
				counts[r + (num >> (places - i))] += 1;
		}
		
		printf("Case %"PRIu64": %"PRIu64"\n", round++, count);
		
		scanf("%"PRIu64" %"PRIu64" %"PRIu64"", &n, &a, &b);
	}
	
	return 0;
}


















