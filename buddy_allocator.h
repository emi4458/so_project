#pragma once
#include "pool_allocator.h"
#include "linked_list.h"
#include "bit_map.h"

#define MAX_LEVELS 16

// one entry of the buddy list
typedef struct BuddyListItem {              
  BitMap* bitmap;
  int idx;   // tree index
  int level; // level for the buddy
  char* start; // start of memory
  int size;
  // struct BuddyListItem* buddy_ptr;            //in teoria non servono
  // struct BuddyListItem* parent_ptr;
} BuddyListItem;


typedef struct  {
  BitMap bitmap;
  int num_levels;
  PoolAllocator memory_item_allocator;    //ex list_allocator
  char* memory; // the memory area to be managed
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;


// // computes the size in bytes for the buffer of the allocator
// int BuddyAllocator_calcSize(int num_levels);                       //da eliminare


// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* buffer,
                         int buffer_size,                        //dovrebbe essere ok
                         char* memory,
                         int min_bucket_size);

// returns (allocates) a buddy at a given level.
// side effect on the internal structures
// 0 id no memory available
BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);  //da modificare per gestire bitmap


// releases an allocated buddy, performing the necessary joins
// side effect on the internal structures
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item);   //da modificare per gestire bitmap

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);   //ok

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);   //ok


