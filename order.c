#include "order.h"

bool BCOrderTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	if (bct->opt.combo)
		return (BCComboTester(bct, words, wordCount));
	else if (bct->opt.permute)
		return (BCPermuteTester(bct, words, wordCount));
	return (BCSeparatorTester(bct, words, wordCount));
}