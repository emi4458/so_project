#pragma once
#include "bit_map.h"

typedef enum {
  Success=0x0,
  NotEnoughMemory=-1,
  UnalignedFree=-2,
  OutOfRange=-3,
  DoubleFree=-4
} PoolAllocatorResult;

typedef struct PoolAllocator{
  
  char* buffer;        //contiguous buffer managed by the system
  int buffer_size;     //size of the buffer in bytes

  int size;            //number of free blocks
  int size_max;        //maximum number of blocks
  int item_size;       //size of a block
  
  BitMap bitmap;       //bitmap al posto della lista
} PoolAllocator;

PoolAllocatorResult PoolAllocator_init(PoolAllocator* allocator,
			int item_size,
			int num_items,
			char* memory_block,
			int memory_size);

void* PoolAllocator_getBlock(PoolAllocator* allocator);

PoolAllocatorResult PoolAllocator_releaseBlock(PoolAllocator* allocator, void* block);
			
const char* PoolAllocator_strerror(PoolAllocatorResult result);

int firstFreeIdx(BitMap* bitmap); //trova il primo indice libero (a 0), -1 se piena