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

int *_getWordOrderShifts(int newWordCount)
{
	int *wordOrderShifts = malloc(newWordCount * sizeof(int));

	for (int i = 0; i < newWordCount; ++i)
		wordOrderShifts[i] = newWordCount - i - 1;
	return (wordOrderShifts);
}

static void _permutesSet(BCT_Segment_t *segment, int *wordOrders, int *wordOrderShifts, int wordCount, int newWordCount)
{
	int index = segment->startOrders;
	int base = wordCount - newWordCount + 1;

	for (int i = 0; i < newWordCount; ++i)
	{
		wordOrders[i] = index % base;
		index /= base;
		++base;
	}
	for (int i = 0; i < newWordCount; ++i)
	{
		for (int j = i + 1; j < newWordCount; ++j)
		{
			if (wordOrders[i] < wordOrders[j])
				--wordOrderShifts[i];
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
			wordOrderShifts[i] = newWordCount - i - 1;
			for (int j = i + 1; j < newWordCount; ++j)
			{
				if (wordOrders[i] < wordOrders[j])
					--wordOrderShifts[i];
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

bool BCPermutesTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int newWordCount = _getNewWordCount(words, wordCount, bct->data->keysize);
	BCT_Word_t **newWords = malloc(newWordCount * sizeof(BCT_Word_t *));
	int *wordOrders = calloc(newWordCount, sizeof(int));
	int *wordOrderShifts = _getWordOrderShifts(newWordCount);
	size_t tested = 0;

	_permutesSet(&bct->ctx.instance, wordOrders, wordOrderShifts, wordCount, newWordCount);
	while (tested < bct->ctx.instance.totalOrders)
	{
		for (int i = 0; i < newWordCount; ++i)
			newWords[i] = words[wordOrders[i] + wordOrderShifts[i]];
		if (BCSeparatorsTester(bct, newWords, newWordCount))
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
	int count = 1;
	for (int i = 0; i < newWordCount; ++i)
		count *= wordCount - i;
	return (count);
}