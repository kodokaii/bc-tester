#include "bc-tester.h"

static void _generateKey(BCT_t *bct, BCT_Word_t **words, int wordCount, int separatorsIndex)
{
	size_t k = 0;

	for (int i = 0; i < wordCount && k < bct->data->keysize; ++i)
	{
		size_t len = words[i]->len;
		if (bct->data->keysize < k + len)
			len = bct->data->keysize - k;
		memcpy(bct->data->key + k, words[i]->str, len);
		k += len;
		if (i + 1 < wordCount && k < bct->data->keysize && bct->data->separators[separatorsIndex] != '\0')
			bct->data->key[k++] = bct->data->separators[separatorsIndex];
	}
	bct->data->key[k] = '\0';
}

bool BCSeparatorsTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int separatorsIndex = bct->ctx.instance.startSeparators;
	size_t tested = 0;

	while (tested < bct->ctx.instance.totalSeparators)
	{
		_generateKey(bct, words, wordCount, separatorsIndex);
		progress(bct);
		if (BCKeyTester(bct->data->fileKey, bct->data->key))
			return (true);
		++separatorsIndex;
		++tested;
	}
	return (false);
}

size_t BCSeparatorsCountTest(BCT_t *bct)
{
	return (bct->data->separatorCount);
}