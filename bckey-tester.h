#pragma once

#include "blowfish.h"
#include <stdbool.h>

#define FLAG_COUNT 2
#define ENDIAN_FLAGS 0
#define COMPRESSED_FLAG 1

#define BIG_ENDIAN_FLAG ((uint8_t)0x45)
#define LITTLE_ENDIAN_FLAG ((uint8_t)0x54)

bool BCKeyTester(uint8_t const *keyFile, char *key);