#pragma once

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MAXKEYBYTES 56 /* 448 bits */
#define HALF_BLOCKSIZE 4
#define BLOCKSIZE 8

typedef struct
{
  uint32_t P[16 + 2];
  uint32_t S[4][256];
} BLOWFISH_CTX;

void Blowfish_Init(BLOWFISH_CTX *ctx, uint8_t *key, uint16_t keyLen);

void Blowfish_Encrypt(BLOWFISH_CTX *ctx, uint32_t *xl, uint32_t *xr);

void Blowfish_Decrypt(BLOWFISH_CTX *ctx, uint32_t *xl, uint32_t *xr);
