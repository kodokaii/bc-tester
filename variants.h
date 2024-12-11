#pragma once

#include "bc-tester.h"

#define VARIANT_NORMAL 0
#define VARIANT_LOWER 1
#define VARIANT_CAPITAL 2
#define VARIANT_UPPER 3
#define VARIANT_CHARS "nlcu"

bool BCVariantsTester(BCT_t *bct, BCT_Word_t **words, int wordCount);