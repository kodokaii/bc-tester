#include "combo.h"

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
		totalLenght += lenghts[newWordCount % wordCount];
		newWordCount++;
	}
	free(lenghts);
	return (newWordCount);
}

static bool _permutationsTester(BCT_t *bct, BCT_Word_t **words, BCT_Word_t **newWords, bool *used, int wordCount, int newWordCount, int depth)
{
	if (depth == newWordCount)
		return (BCSeparatorTester(bct, newWords, newWordCount));
	for (int i = 0; i < wordCount; i++)
	{
		if (!used[i])
		{
			used[i] = true;
			newWords[depth] = words[i];
			if (_permutationsTester(bct, words, newWords, used, wordCount, newWordCount, depth + 1))
				return (true);
			used[i] = false;
		}
	}
	return (false);
}

bool BCPermuteTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int newWordCount = _getNewWordCount(words, wordCount, bct->keysize);
	BCT_Word_t **newWords = malloc(newWordCount * sizeof(BCT_Word_t *));
	bool *used = calloc(wordCount, sizeof(bool));

	for (int i = 0; i < newWordCount; i++)
		newWords[i] = words[i];
	bool result = _permutationsTester(bct, words, newWords, used, wordCount, newWordCount, 0);
	free(newWords);
	free(used);
	return (result);
}