#include "bc-tester.h"

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

static BCT_Word_t **_generateVariants(BCT_t *bct, BCT_Word_t *word)
{
	BCT_Word_t **variants = malloc(sizeof(BCT_Word_t *) * bct->data->variantCount);
	int v = 0;

	for (int i = 0; i < VARIANT_MAX_COUNT; ++i)
	{
		if (bct->data->variants[i])
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

BCT_Word_t ***_generateWordVariants(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	BCT_Word_t ***wordVariants = malloc(sizeof(BCT_Word_t **) * wordCount);

	for (int i = 0; i < wordCount; ++i)
		wordVariants[i] = _generateVariants(bct, words[i]);
	return (wordVariants);
}

void _freeWordVariants(BCT_t *bct, BCT_Word_t ***wordVariants, int wordCount)
{
	for (int i = 0; i < wordCount; ++i)
	{
		for (int j = 0; j < bct->data->variantCount; ++j)
		{
			free(wordVariants[i][j]->str);
			free(wordVariants[i][j]);
		}
		free(wordVariants[i]);
	}
	free(wordVariants);
}

static void _variantsSet(BCT_Segment_t *segment, int *wordsVariantOrders, int wordCount, int variantCount)
{
	int index = segment->startVariants;

	for (int i = 0; i < wordCount; ++i)
	{
		wordsVariantOrders[i] = index % variantCount;
		index /= variantCount;
	}
}

static void _variantsIncrement(int *wordsVariantOrders, int wordCount, int variantCount)
{
	int i = 0;
	while (i < wordCount && wordsVariantOrders[i] + 1 == variantCount)
		wordsVariantOrders[i++] = 0;
	if (i < wordCount)
		wordsVariantOrders[i]++;
}

static bool _exitTester(BCT_t *bct, BCT_Word_t ***wordsVariants, int *wordsVariantOrders, BCT_Word_t **newWords, int wordCount, bool result)
{
	_freeWordVariants(bct, wordsVariants, wordCount);
	free(wordsVariantOrders);
	free(newWords);
	return (result);
}

bool BCVariantsTester(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	BCT_Word_t ***wordsVariants = _generateWordVariants(bct, words, wordCount);
	int *wordsVariantOrders = calloc(wordCount, sizeof(int));
	BCT_Word_t **newWords = malloc(wordCount * sizeof(BCT_Word_t *));
	size_t tested = 0;

	_variantsSet(&bct->ctx.instance, wordsVariantOrders, wordCount, bct->data->variantCount);
	while (tested < bct->ctx.instance.totalVariants)
	{
		for (int i = 0; i < wordCount; ++i)
			newWords[i] = wordsVariants[i][wordsVariantOrders[i]];
		if (BCOrdersTester(bct, newWords, wordCount))
			return (_exitTester(bct, wordsVariants, wordsVariantOrders, newWords, wordCount, true));
		_variantsIncrement(wordsVariantOrders, wordCount, bct->data->variantCount);
		++tested;
	}
	return (_exitTester(bct, wordsVariants, wordsVariantOrders, newWords, wordCount, false));
}

size_t BCVariantsCountTest(BCT_t *bct, int wordCount)
{
	if (!bct->data->opt.variants)
		return (1);
	size_t count = 1;
	for (int i = 0; i < wordCount; ++i)
		count *= bct->data->variantCount;
	return (count);
}