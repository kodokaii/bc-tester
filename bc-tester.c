#include "bc-tester.h"

static void _factorDistribution(BCT_Distribution_t *distrib, size_t factor)
{
	if (distrib->totalVariants % (distrib->divisorVariants * factor) == 0)
		distrib->divisorVariants *= factor;
	else if (distrib->totalOrders % (distrib->divisorOrders * factor) == 0)
		distrib->divisorOrders *= factor;
	else if (distrib->totalSeparators % (distrib->divisorSeparators * factor) == 0)
		distrib->divisorSeparators *= factor;
	else
		distrib->rest *= factor;
}

static void _divisorDistribution(BCT_Distribution_t *distrib, size_t divisors)
{
	factorize(divisors, (void (*)(void *, size_t))_factorDistribution, distrib);
	if (distrib->rest != 1)
	{
		if (distrib->totalOrders <= distrib->totalVariants && distrib->totalSeparators <= distrib->totalVariants)
			distrib->divisorVariants *= distrib->rest;
		else if (distrib->totalSeparators <= distrib->totalOrders)
			distrib->divisorOrders *= distrib->rest;
		else
			distrib->divisorSeparators *= distrib->rest;
	}
}

static void _initSegment(size_t totalVariants, size_t totalOrders, size_t totalSeparators, int index, int total, BCT_Segment_t *segment)
{
	BCT_Distribution_t distrib;

	distrib.totalVariants = totalVariants;
	distrib.totalOrders = totalOrders;
	distrib.totalSeparators = totalSeparators;
	distrib.divisorVariants = 1;
	distrib.divisorOrders = 1;
	distrib.divisorSeparators = 1;
	distrib.rest = 1;
	_divisorDistribution(&distrib, total);

	int indexVariants = (index % total) * distrib.divisorVariants / total;
	segment->startVariants = indexVariants * totalVariants / distrib.divisorVariants;
	segment->totalVariants = (indexVariants + 1) * totalVariants / distrib.divisorVariants - segment->startVariants;

	total /= distrib.divisorVariants;

	int indexOrders = (index % total) * distrib.divisorOrders / total;
	segment->startOrders = indexOrders * totalOrders / distrib.divisorOrders;
	segment->totalOrders = (indexOrders + 1) * totalOrders / distrib.divisorOrders - segment->startOrders;

	total /= distrib.divisorOrders;

	int indexSeparators = (index % total) * distrib.divisorSeparators / total;
	segment->startSeparators = indexSeparators * totalSeparators / distrib.divisorSeparators;
	segment->totalSeparators = (indexSeparators + 1) * totalSeparators / distrib.divisorSeparators - segment->startSeparators;

	segment->total = segment->totalVariants * segment->totalOrders * segment->totalSeparators;
	segment->tested = 0;
}

static void _initInstance(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	size_t totalVariants = BCVariantsCountTest(bct, wordCount);
	size_t totalOrders = BCOrdersCountTest(bct, words, wordCount);
	size_t totalSeparators = BCSeparatorsCountTest(bct);
	_initSegment(totalVariants, totalOrders, totalSeparators, bct->data->instanceIndex, bct->data->instanceTotal, &bct->ctx.instance);
}

static void _initBCT(BCT_t *bct, BCT_Data_t *data, BCT_Word_t **words, int wordCount)
{
	bct->data = data;
	_initInstance(bct, words, wordCount);
	bct->ctx.progressStep = bct->ctx.instance.total / PROGRESS_STEP;
	bct->ctx.oldStep = 0;
}

bool BCTester(BCT_Data_t *data, BCT_Word_t **words, int wordCount)
{
	BCT_t bct;
	bool result;

	_initBCT(&bct, data, words, wordCount);
	if (bct.data->opt.variants)
		result = BCVariantsTester(&bct, words, wordCount);
	else
		result = BCOrdersTester(&bct, words, wordCount);
	if (!bct.data->opt.print)
		write(STDIN_FILENO, "\n", 1);
	return (result);
}