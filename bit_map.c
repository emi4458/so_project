#include <assert.h>
#include "bit_map.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

void BitMap_alloc(BitMap* bmap, int num_bits) {
  printf("\nalloco la bitmap con %d numero di bits\n",num_bits);
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
  //bit--;
  assert(bit<bmap->num_bits);
  int byte=bit/8;
  int bit_in_byte=bit%8;
  return (bmap->data[byte]&(1<<bit_in_byte))>0;
  
}

void BitMap_setBit(BitMap* bmap, int bit, int value) {
  //bit--;
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
  print_used_idx(bmap);
}


// int nBuddyForLevel(int level){
//   return 1<<level;
// }

int levelBuddy(int idx){
  return floor(log2(idx));
}

void print_used_idx(BitMap* bitmap){    //stampa gli indici occupati
  printf("INDICI OCCUPATI: ");
  for(int i=0;i<bitmap->num_bits;i++){
    if(BitMap_getBit(bitmap,i)==1) printf("%d ",i+1); 
  }
  printf("\n\n");
}