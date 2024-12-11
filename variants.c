#include "variants.h"

static void _strlwr(char *str)
{
	for (size_t i = 0; str[i]; ++i)
		str[i] = tolower(str[i]);
}

static void _strupr(char *str)
{
	for (size_t i = 0; str[i]; ++i)
		str[i] = toupper(str[i]);
}

static void _strcap(char *str)
{
	str[0] = toupper(str[0]);
	for (size_t i = 1; str[i]; ++i)
		str[i] = tolower(str[i]);
}

static BCT_Word_t **_generateVariants(BCT_t *bct, BCT_Word_t *word, int variantCount)
{
	BCT_Word_t **variants = malloc(sizeof(char *) * variantCount);
	int v = 0;

	for (int i = 0; i < VARIANT_MAX_COUNT; ++i)
	{
		if (bct->variants[i])
		{
			variants[v] = malloc(sizeof(BCT_Word_t));
			variants[v]->len = word->len;
			variants[v]->str = strdup(word->str);
			switch (i)
			{
			case VARIANT_LOWER:
				_strlwr(variants[v]->str);
				break;
			case VARIANT_CAPITAL:
				_strcap(variants[v]->str);
				break;
			case VARIANT_UPPER:
				_strupr(variants[v]->str);
				break;
			}
			++v;
		}
	}
	return (variants);
}

BCT_Word_t ***_generateWordVariants(BCT_t *bct, BCT_Word_t **words, int wordCount, int variantCount)
{
	BCT_Word_t ***wordVariants = malloc(sizeof(BCT_Word_t **) * wordCount);

	for (int i = 0; i < wordCount; ++i)
		wordVariants[i] = _generateVariants(bct, words[i], variantCount);
	return (wordVariants);
}

void _freeWordVariants(BCT_Word_t ***wordVariants, int wordCount, int variantCount)
{
	for (int i = 0; i < wordCount; ++i)
	{
		for (int j = 0; j < variantCount; ++j)
		{
			free(wordVariants[i][j]->str);
			free(wordVariants[i][j]);
		}
		free(wordVariants[i]);
	}
	free(wordVariants);
}

static int _getVariantCount(BCT_t *bct)
{
	int variantCount = 0;

	for (int i = 0; i < VARIANT_MAX_COUNT; ++i)
		variantCount += bct->variants[i];
	return (variantCount);
}

static bool _variantsIncrement(int *wordsVariantOrder, int wordCount, int variantCount)
{
	int i = 0;
	while (i < wordCount && wordsVariantOrder[i] + 1 == variantCount)
		wordsVariantOrder[i++] = 0;
	if (i < wordCount)
		wordsVariantOrder[i]++;
	else
		return (false);
	return (true);
}

static bool _exitTester(BCT_Word_t ***wordsVariants, int *wordsVariantOrder, BCT_Word_t **newWords, int wordCount, int variantCount, bool result)
{
	_freeWordVariants(wordsVariants, wordCount, variantCount);
	free(wordsVariantOrder);
	free(newWords);
	return (result);
}

bool BCVariantsTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	int variantCount = _getVariantCount(bct);
	BCT_Word_t ***wordsVariants = _generateWordVariants(bct, words, wordCount, variantCount);
	int *wordsVariantOrder = calloc(wordCount, sizeof(int));
	BCT_Word_t **newWords = malloc(wordCount * sizeof(BCT_Word_t *));

	do
	{
		for (int i = 0; i < wordCount; ++i)
			newWords[i] = wordsVariants[i][wordsVariantOrder[i]];
		if (BCOrderTester(bct, newWords, wordCount))
			return (_exitTester(wordsVariants, wordsVariantOrder, newWords, wordCount, variantCount, true));
	} while (_variantsIncrement(wordsVariantOrder, wordCount, variantCount));
	return (_exitTester(wordsVariants, wordsVariantOrder, newWords, wordCount, variantCount, false));
}