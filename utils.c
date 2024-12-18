#include "bc-tester.h"

void factorize(int n, void (*callback)(void *, int), void *data)
{
	while (n % 2 == 0)
	{
		callback(data, 2);
		n /= 2;
	}
	while (n % 3 == 0)
	{
		callback(data, 3);
		n /= 3;
	}
	int i = 5;
	while (i * i <= n)
	{
		if (n % i == 0)
		{
			callback(data, i);
			n /= i;
			continue;
		}
		i += 2;
		if (n % i == 0)
		{
			callback(data, i);
			n /= i;
			continue;
		}
		i += 4;
	}
	if (1 < n)
		callback(data, n);
}