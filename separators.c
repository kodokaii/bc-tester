#include "bc-tester.h"

static void _generateKey(BCT_t *bct, char *key, BCT_Word_t **words, int wordCount, int separatorsIndex)
{
	size_t k = 0;

	for (int i = 0; i < wordCount && k < bct->data->keysize; ++i)
	{
		size_t len = words[i]->len;
		if (bct->data->keysize < k + len)
			len = bct->data->keysize - k;
		memcpy(key + k, words[i]->str, len);
		k += len;
		if (i + 1 < wordCount && k < bct->data->keysize && bct->data->separators[separatorsIndex] != '\0')
			key[k++] = bct->data->separators[separatorsIndex];
	}
	key[k] = '\0';
}

static bool _exitTester(BCT_t *bct, int threadIndex, bool result)
{
	if (result)
		bct->ctx.threadFound = threadIndex;
	return (result);
}

bool BCSeparatorsTester(BCT_t *bct, char *key, int threadIndex, BCT_Word_t **words, int wordCount)
{
	int separatorsIndex = bct->ctx.instance.startSeparators + bct->ctx.threads[threadIndex].startSeparators;
	size_t tested = 0;

	while (tested < bct->ctx.threads[threadIndex].totalSeparators)
	{
		_generateKey(bct, key, words, wordCount, separatorsIndex);
		progress(bct, key, threadIndex);
		if (BCKeyTester(bct->data->fileKey, key))
			return (_exitTester(bct, threadIndex, true));
		++separatorsIndex;
		++tested;
	}
	return (_exitTester(bct, threadIndex, false));
}

size_t BCSeparatorsCountTest(BCT_t *bct)
{
	return (bct->data->separatorCount);
}