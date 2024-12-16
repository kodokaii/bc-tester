#include "bc-tester.h"

size_t factorial(size_t n)
{
	size_t fact = 1;

	for (size_t i = 1; i <= n; ++i)
		fact *= i;
	return (fact);
}

void factorize(size_t n, void (*callback)(void *, size_t), void *data)
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