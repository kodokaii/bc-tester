#include "bc-tester.h"

typedef struct
{
	BCT_t *bct;
	char *key;
	int threadIndex;
	BCT_Word_t **words;
	int wordCount;
	sem_t init;
} BCT_ThreadData_t;

static void _factorDistribution(BCT_Distribution_t *distrib, int factor)
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

static void _divisorDistribution(BCT_Distribution_t *distrib, size_t totalVariants, size_t totalOrders, size_t totalSeparators, int divisor)
{
	distrib->totalVariants = totalVariants;
	distrib->totalOrders = totalOrders;
	distrib->totalSeparators = totalSeparators;
	distrib->divisorVariants = 1;
	distrib->divisorOrders = 1;
	distrib->divisorSeparators = 1;
	distrib->rest = 1;
	factorize(divisor, (void (*)(void *, int))_factorDistribution, distrib);
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

static int _initSegmentPart(size_t *segmentStart, size_t *segmentTotal, size_t total, int divisor, int divisorTotal, int index)
{
	if (SIZE_MAX / divisorTotal < total)
		return (errno = ERANGE, EXIT_FAILURE);
	index = (index % divisorTotal) * divisor / divisorTotal;
	*segmentStart = index * total / divisor;
	*segmentTotal = (index + 1) * total / divisor - *segmentStart;
	return (EXIT_SUCCESS);
}

static int _initSegment(BCT_Segment_t *segment, BCT_Distribution_t *distrib, int index)
{
	int divisorTotal = distrib->divisorVariants * distrib->divisorOrders * distrib->divisorSeparators;

	if (_initSegmentPart(&segment->startVariants, &segment->totalVariants, distrib->totalVariants, distrib->divisorVariants, divisorTotal, index))
		return (EXIT_FAILURE);
	divisorTotal /= distrib->divisorVariants;
	if (_initSegmentPart(&segment->startOrders, &segment->totalOrders, distrib->totalOrders, distrib->divisorOrders, divisorTotal, index))
		return (EXIT_FAILURE);
	divisorTotal /= distrib->divisorOrders;
	if (_initSegmentPart(&segment->startSeparators, &segment->totalSeparators, distrib->totalSeparators, distrib->divisorSeparators, divisorTotal, index))
		return (EXIT_FAILURE);

	if (SIZE_MAX / segment->totalVariants < segment->totalOrders || SIZE_MAX / segment->totalVariants < segment->totalSeparators || SIZE_MAX / segment->totalOrders < segment->totalSeparators)
		return (errno = ERANGE, EXIT_FAILURE);
	if (SIZE_MAX / segment->totalVariants < segment->totalOrders * segment->totalSeparators)
		return (errno = ERANGE, EXIT_FAILURE);
	segment->total = segment->totalVariants * segment->totalOrders * segment->totalSeparators;
	return (EXIT_SUCCESS);
}

static int _initInstance(BCT_t *bct, BCT_Word_t **words, int wordCount)
{
	BCT_Distribution_t distrib;
	size_t totalVariants = BCVariantsCountTest(bct, wordCount);
	size_t totalOrders = BCOrdersCountTest(bct, words, wordCount);
	size_t totalSeparators = BCSeparatorsCountTest(bct);

	_divisorDistribution(&distrib, totalVariants, totalOrders, totalSeparators, bct->data->instanceTotal);
	_initSegment(&bct->ctx.instance, &distrib, bct->data->instanceIndex);
	if (errno == ERANGE)
		return (warnx("Overflow"), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static int _initThreads(BCT_t *bct)
{
	BCT_Distribution_t distrib;

	_divisorDistribution(&distrib, bct->ctx.instance.totalVariants, bct->ctx.instance.totalOrders, bct->ctx.instance.totalSeparators, bct->data->threadCount);
	bct->ctx.threads = malloc(bct->data->threadCount * sizeof(BCT_Segment_t));
	for (int i = 0; i < bct->data->threadCount; i++)
		_initSegment(&bct->ctx.threads[i], &distrib, i);
	if (errno == ERANGE)
		return (warnx("Overflow"), free(bct->ctx.threads), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static int _initBCT(BCT_t *bct, BCT_Data_t *data, BCT_Word_t **words, int wordCount)
{
	bct->data = data;
	if (_initInstance(bct, words, wordCount) || _initThreads(bct))
		return (EXIT_FAILURE);
	time(&bct->ctx.startTime);
	bct->ctx.testPerSecond = ILLUSTRATIVE_TEST_PER_SECOND * bct->data->threadCount;
	bct->ctx.estimateStep = bct->ctx.instance.total / (ILLUSTRATIVE_TEST_PER_SECOND * bct->data->threadCount);
	if (MAX_ESTIMATE_STEP < bct->ctx.estimateStep)
		bct->ctx.estimateStep = MAX_ESTIMATE_STEP;
	bct->ctx.oldEstimate = 0;
	bct->ctx.progressStep = bct->ctx.instance.total / 10000;
	if (MAX_PROGRESS_STEP < bct->ctx.progressStep)
		bct->ctx.progressStep = MAX_PROGRESS_STEP;
	bct->ctx.oldProgress = 0;
	bct->ctx.threadFound = NO_THREAD_FOUND;
	bct->ctx.tested = 0;
	return (EXIT_SUCCESS);
}

static void _runTest(BCT_t *bct, char *key, int threadIndex, BCT_Word_t **words, int wordCount, sem_t *init)
{
	if (init != SEM_FAILED)
		sem_post(init);
	if (bct->data->opt.variants)
		BCVariantsTester(bct, key, threadIndex, words, wordCount);
	else
		BCOrdersTester(bct, key, threadIndex, words, wordCount);
}

static void *_runThreadTest(BCT_ThreadData_t *data)
{
	_runTest(data->bct, data->key, data->threadIndex, data->words, data->wordCount, &data->init);
	return (NULL);
}

static pthread_t _runThread(BCT_t *bct, char *keys, int threadIndex, BCT_Word_t **words, int wordCount)
{
	BCT_ThreadData_t data = {bct, keys + (bct->data->keysize + 1) * threadIndex, threadIndex, words, wordCount, {}};
	pthread_t thread;

	sem_init(&data.init, 0, 0);
	pthread_create(&thread, NULL, (void *(*)(void *))_runThreadTest, &data);
	sem_wait(&data.init);
	sem_destroy(&data.init);
	return (thread);
}

bool BCTester(BCT_Data_t *data, char *key, BCT_Word_t **words, int wordCount)
{
	BCT_t bct;

	if (_initBCT(&bct, data, words, wordCount))
		return (false);

	char *keys = calloc(data->threadCount, data->keysize + 1);
	pthread_t *threads = malloc((data->threadCount - 1) * sizeof(pthread_t));

	for (int i = 0; i < data->threadCount - 1; ++i)
		threads[i] = _runThread(&bct, keys, i + 1, words, wordCount);
	_runTest(&bct, keys, 0, words, wordCount, SEM_FAILED);
	for (int i = 0; i < data->threadCount - 1; ++i)
		pthread_join(threads[i], NULL);
	if (bct.ctx.threadFound != NO_THREAD_FOUND)
		memcpy(key, keys + (bct.data->keysize + 1) * bct.ctx.threadFound, data->keysize);
	if (!bct.data->opt.print)
		write(STDIN_FILENO, "\n", 1);
	free(bct.ctx.threads);
	free(threads);
	free(keys);
	return (bct.ctx.threadFound != NO_THREAD_FOUND);
}