#include "separator.h"

static void _generateKey(BCT_t *bct, BCT_Word_t **words, int wordCount, int separatorIndex)
{
	size_t k = 0;

	for (int i = 0; i < wordCount && k < bct->keysize; ++i)
	{
		size_t len = words[i]->len;
		if (bct->keysize < k + len)
			len = bct->keysize - k;
		memcpy(bct->key + k, words[i]->str, len);
		k += len;
		if (i + 1 < wordCount && k < bct->keysize && 0 <= separatorIndex)
			bct->key[k++] = bct->separator[separatorIndex];
	}
	bct->key[k] = '\0';
}

bool BCSeparatorTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int separatorIndex = -1;

	do
	{
		_generateKey(bct, words, wordCount, separatorIndex);
		if (bct->opt.print)
			printf("%s\n", bct->key);
		if (BCKeyTester(bct->fileKey, bct->key))
			return (true);
	} while (bct->separator[++separatorIndex]);
	return (false);
}