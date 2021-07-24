#include <assert.h>
#include "bit_map.h"
#include<stdlib.h>


void BitMap_alloc(BitMap* bmap, int num_bits) {
  int num_bytes=(num_bits/8)+((num_bits%8)>0);
  bmap->num_bits=num_bits;
  bmap->num_bytes=num_bytes;
  bmap->data=(uint8_t*) malloc(num_bytes);
  memset(bmap->data, 0, num_bytes);
}

void BitMap_free(BitMap* bmap, int num_bits) {
  bmap->num_bits=0;
  bmap->num_bytes=0;
  if(bmap->data)
    free(bmap->data);
  bmap->data=0;
}

int BitMap_getBit(BitMap* bmap, int bit) {
  assert(bit<bmap->num_bits);
  int byte=bit/8;
  int bit_in_byte=bit%8;
  return (bmap->data[byte]&(1<<bit_in_byte))>0;
  
}

void BitMap_setBit(BitMap* bmap, int bit, int value) {
  assert(bit<bmap->num_bits);
  int byte=bit/8;
  int bit_in_byte=bit%8;
  uint8_t mask=1<<bit_in_byte;
  if (value) {
    bmap->data[byte]|=mask;
  } else {
    bmap->data[byte]&=(~mask);
  }
}

void BitMap_print(BitMap* bmap) {
  printf("BMap [ ptr: %xl, n_bytes: %d, n_bits: %d]\n[",
         bmap, bmap->num_bytes, bmap->num_bits);
  for(int i=0; i<bmap->num_bits; ++i) {
    printf("%d",  BitMap_getBit(bmap, i));
  }
  printf("]\n");
}
