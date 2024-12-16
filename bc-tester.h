#pragma once

#include "bckey-tester.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#define PROGRESS_STEP 200000

#define VARIANT_MAX_COUNT 4
#define VARIANT_NORMAL 0
#define VARIANT_LOWER 1
#define VARIANT_CAPITAL 2
#define VARIANT_UPPER 3
#define VARIANT_CHARS "nlcu"

typedef struct
{
	size_t totalVariants;
	size_t totalOrders;
	size_t totalSeparators;
	size_t divisorVariants;
	size_t divisorOrders;
	size_t divisorSeparators;
	size_t rest;
} BCT_Distribution_t;

typedef struct
{
	size_t tested;
	size_t total;
	size_t startVariants;
	size_t startOrders;
	size_t startSeparators;
	size_t totalVariants;
	size_t totalOrders;
	size_t totalSeparators;
} BCT_Segment_t;

typedef struct
{
	char *str;
	size_t len;
} BCT_Word_t;

typedef struct
{
	bool print;
	bool variants;
	bool combos;
	bool permutes;
} BCT_Opt_t;

typedef struct
{
	BCT_Segment_t instance;
	BCT_Segment_t *threads;
	size_t progressStep;
	size_t oldStep;
} BCT_CTX_t;

typedef struct
{
	BCT_Opt_t opt;
	int variantCount;
	bool variants[VARIANT_MAX_COUNT];
	int separatorCount;
	char *separators;
	int threadCount;
	int instanceTotal;
	int instanceIndex;
	uint8_t *fileKey;
	size_t keysize;
	char *key;
} BCT_Data_t;

typedef struct
{
	BCT_Data_t *data;
	BCT_CTX_t ctx;
} BCT_t;

bool BCTester(BCT_Data_t *data, BCT_Word_t **words, int wordCount);

bool BCVariantsTester(BCT_t *bct, BCT_Word_t **words, int wordCount);
size_t BCVariantsCountTest(BCT_t *bct, int wordCount);

bool BCOrdersTester(BCT_t *bct, BCT_Word_t **words, int wordCount);
size_t BCOrdersCountTest(BCT_t *bct, BCT_Word_t **words, int wordCount);

bool BCCombosTester(BCT_t *bct, BCT_Word_t **words, int wordCount);
size_t BCCombosCountTest(BCT_t *bct, BCT_Word_t **words, int wordCount);

bool BCPermutesTester(BCT_t *bct, BCT_Word_t **words, int wordCount);
size_t BCPermutesCountTest(BCT_t *bct, BCT_Word_t **words, int wordCount);

bool BCSeparatorsTester(BCT_t *bct, BCT_Word_t **words, int wordCount);
size_t BCSeparatorsCountTest(BCT_t *bct);

void progress(BCT_t *bct);

size_t factorial(size_t n);
void factorize(size_t n, void (*callback)(void *, size_t), void *data);