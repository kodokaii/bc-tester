#include "bckey-tester.h"

static uint8_t *_hashKey(char *key, size_t keysize)
{
	static uint8_t keyHash[MAXKEYBYTES];
	BLOWFISH_CTX ctx;
	uint32_t L, R;

	Blowfish_Init(&ctx, (uint8_t *)key, keysize);

	memcpy(&L, key, sizeof(uint32_t));
	memcpy(&R, key + sizeof(uint32_t), sizeof(uint32_t));

	for (int i = 0; i < MAXKEYBYTES; i += (sizeof(uint32_t) * 2))
	{
		Blowfish_Encrypt(&ctx, &L, &R);
		memcpy(keyHash + i, &L, sizeof(uint32_t));
		memcpy(keyHash + i + sizeof(uint32_t), &R, sizeof(uint32_t));
	}
	return (keyHash);
}

#include <stdio.h>
bool BCKeyTester(uint8_t const *keyFile, char *key)
{
	// printf("key: %s\n", key);
	size_t keysize = strlen(key);
	if (keysize < BLOCKSIZE)
		return (false);
	uint8_t *keyHash = _hashKey(key, keysize);
	uint8_t keyFileBlock[BLOCKSIZE];
	int h = MAXKEYBYTES - 1;
	int f = MAXKEYBYTES + BLOCKSIZE - 1;
	bool stripped = false;
	BLOWFISH_CTX ctx;

	Blowfish_Init(&ctx, keyHash, MAXKEYBYTES);
	while (0 <= h && 0 <= f)
	{
		memcpy(keyFileBlock, keyFile + f + 1 - BLOCKSIZE, BLOCKSIZE);
		Blowfish_Decrypt(&ctx, (uint32_t *)(keyFileBlock), (uint32_t *)(keyFileBlock + HALF_BLOCKSIZE));
		for (int i = BLOCKSIZE - 1; 0 <= i && 0 <= h && 0 <= f; --i)
		{
			if (!stripped)
			{
				if (keyFileBlock[i] != 0)
					stripped = true;
			}
			if (stripped)
			{
				if (keyFileBlock[i] != keyHash[h])
					return (false);
				--h;
			}
		}
		f -= BLOCKSIZE;
	}
	return (true);
}