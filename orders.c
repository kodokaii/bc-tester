#include "bc-tester.h"

bool BCOrdersTester(BCT_t *bct, char *key, int threadIndex, BCT_Word_t **words, int wordCount)
{
	if (bct->data->opt.combos)
		return (BCCombosTester(bct, key, threadIndex, words, wordCount));
	else if (bct->data->opt.permutes)
		return (BCPermutesTester(bct, key, threadIndex, words, wordCount));
	return (BCSeparatorsTester(bct, key, threadIndex, words, wordCount));
}

size_t BCOrdersCountTest(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	if (bct->data->opt.combos)
		return (BCCombosCountTest(bct, words, wordCount));
	else if (bct->data->opt.permutes)
		return (BCPermutesCountTest(bct, words, wordCount));
	return (1);
}