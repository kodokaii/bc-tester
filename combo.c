#include "combo.h"

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

static bool _comboIncrement(int *wordOrder, int wordCount, int newWordCount)
{
	int i = 0;
	while (i < newWordCount && wordOrder[i] + 1 == wordCount)
		wordOrder[i++] = 0;
	if (i < newWordCount)
		wordOrder[i]++;
	else
		return (false);
	return (true);
}

static bool _exitTester(int *wordOrder, BCT_Word_t **newWords, bool result)
{
	free(newWords);
	free(wordOrder);
	return (result);
}

bool BCComboTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int newWordCount = _getNewWordCount(words, wordCount, bct->keysize);
	BCT_Word_t **newWords = malloc(newWordCount * sizeof(BCT_Word_t *));
	int *wordOrder = calloc(newWordCount, sizeof(int));

	do
	{
		for (int i = 0; i < newWordCount; ++i)
			newWords[i] = words[wordOrder[i]];
		if (BCSeparatorTester(bct, newWords, newWordCount))
			return (_exitTester(wordOrder, newWords, true));
	} while (_comboIncrement(wordOrder, wordCount, newWordCount));
	return (_exitTester(wordOrder, newWords, false));
}