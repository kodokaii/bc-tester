#include "bc-tester.h"

inline static int _compareSizeT(const void *a, const void *b)
{
	return (*(size_t *)a - *(size_t *)b);
}

static int _getNewWordCount(BCT_Word_t **words, int wordCount, size_t keysize)
{
	size_t *lenghts = malloc(wordCount * sizeof(size_t));

	for (int i = 0; i < wordCount; i++)
		lenghts[i] = words[i]->len;
	qsort(lenghts, wordCount, sizeof(size_t), _compareSizeT);
	int newWordCount = 0;
	size_t totalLenght = 0;
	while (totalLenght < keysize && newWordCount < wordCount)
	{
		totalLenght += lenghts[newWordCount];
		newWordCount++;
	}
	free(lenghts);
	return (newWordCount);
}

static void _permutesSet(size_t index, int *wordOrders, int *wordOrderShifts, int wordCount, int newWordCount)
{
	int base = wordCount - newWordCount + 1;

	for (int i = 0; i < newWordCount; ++i)
	{
		wordOrders[i] = index % base;
		index /= base;
		++base;
	}
	for (int i = 0; i < newWordCount; ++i)
	{
		wordOrderShifts[i] = 0;
		for (int j = i + 1; j < newWordCount; ++j)
		{
			if (wordOrders[j] <= wordOrders[i] + wordOrderShifts[i])
				++wordOrderShifts[i];
		}
	}
}

static void _permutesIncrement(int *wordOrders, int *wordOrderShifts, int wordCount, int newWordCount)
{
	int i = 0;
	while (i < newWordCount && wordOrders[i] == wordCount - newWordCount + i)
		wordOrders[i++] = 0;
	if (i < newWordCount)
	{
		wordOrders[i]++;
		while (0 <= i)
		{
			wordOrderShifts[i] = 0;
			for (int j = i + 1; j < newWordCount; ++j)
			{
				if (wordOrders[j] <= wordOrders[i] + wordOrderShifts[i])
					++wordOrderShifts[i];
			}
			--i;
		}
	}
}

static bool _exitTester(BCT_Word_t **newWords, int *wordOrders, int *wordOrderShifts, bool result)
{
	free(newWords);
	free(wordOrders);
	free(wordOrderShifts);
	return (result);
}

bool BCPermutesTester(BCT_t *bct, char *key, int threadIndex, BCT_Word_t **words, int wordCount)
{
	int newWordCount = _getNewWordCount(words, wordCount, bct->data->keysize);
	BCT_Word_t **newWords = malloc(newWordCount * sizeof(BCT_Word_t *));
	int *wordOrders = calloc(newWordCount, sizeof(int));
	int *wordOrderShifts = malloc(newWordCount * sizeof(int));
	size_t tested = 0;

	_permutesSet(bct->ctx.instance.startOrders + bct->ctx.threads[threadIndex].startOrders, wordOrders, wordOrderShifts, wordCount, newWordCount);
	while (tested < bct->ctx.threads[threadIndex].totalOrders)
	{
		for (int i = 0; i < newWordCount; ++i)
			newWords[i] = words[wordOrders[i] + wordOrderShifts[i]];
		if (BCSeparatorsTester(bct, key, threadIndex, newWords, newWordCount))
			return (_exitTester(newWords, wordOrders, wordOrderShifts, true));
		_permutesIncrement(wordOrders, wordOrderShifts, wordCount, newWordCount);
		++tested;
	}
	return (_exitTester(newWords, wordOrders, wordOrderShifts, false));
}

size_t BCPermutesCountTest(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	if (!bct->data->opt.permutes)
		return (1);
	int newWordCount = _getNewWordCount(words, wordCount, bct->data->keysize);
	size_t count = 1;
	for (int i = 0; i < newWordCount; ++i)
	{
		if (SIZE_MAX / (wordCount - i) < count)
			return (errno = ERANGE, EXIT_FAILURE);
		count *= wordCount - i;
	}
	return (count);
}