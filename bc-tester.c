#include "bc-tester.h"

bool BCTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	if (bct->opt.variant)
		return (BCVariantsTester(bct, words, wordCount));
	return (BCOrderTester(bct, words, wordCount));
}