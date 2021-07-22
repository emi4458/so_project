#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}

   //calcolo l indice all'interno della bitmap
   int BitMap_getIdx(int level,int idx){

   }

// // computes the size in bytes for the allocator
// int BuddyAllocator_calcSize(int num_levels) {
//   int list_items=1<<(num_levels+1); // maximum number of allocations, used to determine the max list items   //da eliminare
//   int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;
//   return list_alloc_size;
// }

// creates an item from the index
// and puts it in the corresponding list
BuddyListItem* BuddyAllocator_createListItem(BuddyAllocator* alloc,
                                             int idx,
                                             ){
  BuddyListItem* item=(BuddyListItem*)PoolAllocator_getBlock(&alloc->memory_item_allocator);
  item->idx=idx;
  item->level=levelIdx(idx);
  item->start= alloc->memory + ((idx-(1<<levelIdx(idx))) << (alloc->num_levels-item->level) )*alloc->min_bucket_size;   //ok
  item->size=(1<<(alloc->num_levels-item->level))*alloc->min_bucket_size;
  BitMap_setBit(&alloc->bitmap,idx,1);
  printf("Creating Item. idx:%d, level:%d, start:%p, size:%d\n", 
         item->idx, item->level, item->start, item->size);
  return item;
};

// detaches and destroys an item in the free lists 
void BuddyAllocator_destroyListItem(BuddyAllocator* alloc, BuddyListItem* item){
  int level=item->level;
  BitMap_setBit(&alloc->bitmap, item->idx,0);
  printf("Destroying Item. level:%d, idx:%d, start:%p, size:%d\n",                                      //ok
         item->level, item->idx, item->start, item->size);
  PoolAllocatorResult release_result=PoolAllocator_releaseBlock(&alloc->memory_item_allocator, item);
  assert(release_result==Success);

};



void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size){

  // we need room also for level 0
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->min_bucket_size=min_bucket_size;
  assert (num_levels<MAX_LEVELS);
  //controllo che ci sia abbastanza memoria             
  assert (buffer_size>=BitMap_getBytes(1<<num_levels));

  int bitmap_items=(1<<(num_levels+1))-1; // maximum number of allocations, used to size the list    //ex list_items
  int bitmap_alloc_size=BitMap_getBytes(bitmap_items);     //bitmap_items=nodi dell'albero      //ex list_alloc_size

  printf("BUDDY_BITMAP INITIALIZING\n");
  printf("\tlevels: %d", num_levels);
  printf("\tmax bitmap entries %d bytes\n", bitmap_alloc_size);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
  
  // the buffer for the list starts where the bitmap ends
  char *bitmap_start=buffer;
  PoolAllocatorResult init_result=PoolAllocator_init(&alloc->memory_item_allocator,
						     sizeof(BuddyListItem),
						     bitmap_items,
						     bitmap_start,
						     bitmap_alloc_size);
  printf("%s\n",PoolAllocator_strerror(init_result));

   //inizializzo la bitmap
  BitMap_init(alloc->bitmap,bitmap_alloc_siz,buffer );
  

  // we allocate a list_item to mark that there is one "materialized" list
  // in the first block                                                       //non ho capito a cosa serve
  //BuddyAllocator_createListItem(alloc, 1, 0);
};


BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){       //da fare dopo studio
  if (level<0)
    return 0;
  assert(level <= alloc->num_levels);

  if (! alloc->free[level].size ) { // no buddies on this level
    BuddyListItem* parent_ptr=BuddyAllocator_getBuddy(alloc, level-1);
    if (! parent_ptr)
      return 0;

    // parent already detached from free list
    int left_idx=parent_ptr->idx<<1;
    int right_idx=left_idx+1;
    
    printf("split l:%d, left_idx: %d, right_idx: %d\r", level, left_idx, right_idx);
    BuddyListItem* left_ptr=BuddyAllocator_createListItem(alloc,left_idx   );
    BuddyListItem* right_ptr=BuddyAllocator_createListItem(alloc,right_idx   );
    // we need to update the buddy ptrs
    left_ptr->buddy_ptr=right_ptr;
    right_ptr->buddy_ptr=left_ptr;
  }
  // we detach the first
  if(alloc->free[level].size) {
    BuddyListItem* item=(BuddyListItem*)List_popFront(alloc->free+level);
    return item;
  }
  assert(0);
  return 0;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item){

  BuddyListItem* parent_ptr=item->parent_ptr;
  BuddyListItem *buddy_ptr=item->buddy_ptr;
  
  // buddy back in the free list of its level
  List_pushFront(&alloc->free[item->level],(ListItem*)item);

  // if on top of the chain, do nothing
  if (! parent_ptr)
    return;
  
  // if the buddy of this item is not free, we do nothing
  if (buddy_ptr->list.prev==0 && buddy_ptr->list.next==0) 
    return;
  
  //join
  //1. we destroy the two buddies in the free list;
  printf("merge %d\n", item->level);
  BuddyAllocator_destroyListItem(alloc, item);
  BuddyAllocator_destroyListItem(alloc, buddy_ptr);
  //2. we release the parent
  BuddyAllocator_releaseBuddy(alloc, parent_ptr);

}

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  // we determine the level of the page
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;           //ok
  int  level=floor(log2(mem_size/(size+8)));

  // if the level is too small, we pad it to max
  if (level>alloc->num_levels)                                          //ok
    level=alloc->num_levels;

  printf("requested: %d bytes, level %d \n",
         size, level);

  // we get a buddy of that size;
  BuddyListItem* buddy=BuddyAllocator_getBuddy(alloc, level);
  if (! buddy)
    return 0;

  // we write in the memory region managed the buddy address
  BuddyListItem** target= (BuddyListItem**)(buddy->start);
  *target=buddy;
  return buddy->start+8;
}
//releases allocated memory                                             
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {                  //seems ok
  printf("freeing %p", mem);
  // we retrieve the buddy from the system
  char* p=(char*) mem;
  p=p-8;
  BuddyListItem** buddy_ptr=(BuddyListItem**)p;
  BuddyListItem* buddy=*buddy_ptr;
  //printf("level %d", buddy->level);
  // sanity check;
  assert(buddy->start==p);
  BuddyAllocator_releaseBuddy(alloc, buddy);
  
}
