#include "bckey-tester.h"

static uint8_t *_hashKey(char *key, size_t keysize)
{
	uint8_t *keyHash = malloc(MAXKEYBYTES);
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

static bool _exitTester(uint8_t *keyHash, bool result)
{
	free(keyHash);
	return (result);
}

bool BCKeyTester(uint8_t const *keyFile, char *key)
{
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
					return (_exitTester(keyHash, false));
				--h;
			}
		}
		f -= BLOCKSIZE;
	}
	return (_exitTester(keyHash, true));
}