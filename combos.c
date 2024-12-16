#include "bc-tester.h"

static int _getNewWordCount(BCT_Word_t **words, int wordCount, size_t keysize)
{
	size_t minLenght = SIZE_MAX;

	for (int i = 0; i < wordCount; i++)
	{
		if (words[i]->len < minLenght)
			minLenght = words[i]->len;
	}
	return (keysize / minLenght + (keysize % minLenght != 0));
}

static void _combosSet(BCT_Segment_t *segment, int *wordOrders, int wordCount, int newWordCount)
{
	int index = segment->startOrders;

	for (int i = 0; i < newWordCount; ++i)
	{
		wordOrders[i] = index % wordCount;
		index /= wordCount;
	}
}

static void _combosIncrement(int *wordOrders, int wordCount, int newWordCount)
{
	int i = 0;
	while (i < newWordCount && wordOrders[i] + 1 == wordCount)
		wordOrders[i++] = 0;
	if (i < newWordCount)
		wordOrders[i]++;
}

static bool _exitTester(BCT_Word_t **newWords, int *wordOrders, bool result)
{
	free(newWords);
	free(wordOrders);
	return (result);
}

bool BCCombosTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int newWordCount = _getNewWordCount(words, wordCount, bct->data->keysize);
	BCT_Word_t **newWords = malloc(newWordCount * sizeof(BCT_Word_t *));
	int *wordOrders = calloc(newWordCount, sizeof(int));
	size_t tested = 0;

	_combosSet(&bct->ctx.instance, wordOrders, wordCount, newWordCount);
	while (tested < bct->ctx.instance.totalOrders)
	{
		for (int i = 0; i < newWordCount; ++i)
			newWords[i] = words[wordOrders[i]];
		if (BCSeparatorsTester(bct, newWords, newWordCount))
			return (_exitTester(newWords, wordOrders, true));
		_combosIncrement(wordOrders, wordCount, newWordCount);
		++tested;
	}
	return (_exitTester(newWords, wordOrders, false));
}

size_t BCCombosCountTest(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	if (!bct->data->opt.combos)
		return (1);
	int newWordCount = _getNewWordCount(words, wordCount, bct->data->keysize);
	size_t count = 1;
	for (int i = 0; i < newWordCount; ++i)
		count *= wordCount;
	return (count);
}