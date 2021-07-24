#pragma once
#include <stdint.h>

// simple bit array
typedef struct {
  int num_bits;
  int num_bytes;
  uint8_t * data;
} BitMap;

void BitMap_alloc(BitMap* bmap, int num_bits);

void BitMap_free(BitMap* bmap, int num_bits);

int BitMap_getBit(BitMap* bmap, int bit);

void BitMap_setBit(BitMap* bmap, int bit, int value);

void BitMap_print(BitMap* bmap);

void BitMap_defrag(BitMap* bmap);