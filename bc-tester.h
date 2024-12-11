#pragma once

#include "bckey-tester.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VARIANT_MAX_COUNT 4

typedef struct
{
	char *str;
	size_t len;
} BCT_Word_t;

typedef struct
{
	bool combo;
	bool permute;
	bool variant;
	bool print;
} BCT_Opt_t;

typedef struct
{
	BCT_Opt_t opt;
	char *separator;
	bool variants[VARIANT_MAX_COUNT];
	uint8_t *fileKey;
	size_t keysize;
	char *key;
	size_t testedKeysCount;
	size_t totalKeysCount;
} BCT_t;

#include "variants.h"
#include "order.h"
#include "separator.h"

bool BCTester(BCT_t *bct, BCT_Word_t **words, int wordCount);
