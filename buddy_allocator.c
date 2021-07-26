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
    return idx+1;          //ho scambiato + e -
  }
  return idx-1;
}

int parentIdx(int idx){   //modificata per numerazione albero da 0
  if(idx%2==0){
	return idx/2-1;          
  }
  else return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}

int nBuddyForLevel(int level){
  return 1<<level;
}

int firstIdx(int level){
  return 1<<level;
}

int emptyIdx(BitMap* bitmap, int level){                                     //trova il primo indice vuoto del livello, -1 se non c'è
  //printf("entro in emptyidx...\n");
  for(int i=nBuddyForLevel(level)-1 ; i<nBuddyForLevel(level)*2-1 ; i++){
    if(BitMap_getBit(bitmap,i)==0) return i;                                //testata:funziona
  }
  return -1;
}

int buddyByLevels(int num_levels){   //calcola il numero di nodi max
  num_levels++;
  return 1<<num_levels;
}


// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int list_items=1<<(num_levels+1); // maximum number of allocations, used to determine the max list items
  int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;
  return list_alloc_size;
}

// creates an item from the index
// and puts it in the corresponding list
BuddyListItem* BuddyAllocator_createListItem(BuddyAllocator* alloc,
                                             int idx
                                             ){
  //printf("\nentro in BuddyAllocator_createListItem\n");
  BuddyListItem* item=(BuddyListItem*)PoolAllocator_getBlock(&alloc->list_allocator);
  item->idx=idx;
  item->level=levelIdx(idx);
  item->start= alloc->memory + ((idx-(1<<levelIdx(idx))) << (alloc->num_levels-item->level) )*alloc->min_bucket_size;
  item->size=(1<<(alloc->num_levels-item->level))*alloc->min_bucket_size;
  printf("Creating Item. idx:%d, level:%d, start:%p, size:%d\n", item->idx, item->level, item->start, item->size);
  return item;
  return 0;
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
  BitMap_alloc(&(alloc->bitmap),1<<(num_levels+1));
  assert (num_levels<MAX_LEVELS);

  int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
  int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d", num_levels);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
  char *list_start=buffer;
  PoolAllocatorResult init_result=PoolAllocator_init(&alloc->list_allocator,
						     sizeof(BuddyListItem),
						     list_items,
						     list_start,
						     list_alloc_size);
  printf("%s\n",PoolAllocator_strerror(init_result));
  
};


BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  //printf("entro in get buddy...\n");
  //BitMap_print(&(alloc->bitmap));
  if (level<0)
    return 0;
  assert(level <= alloc->num_levels);
  printf("il primo indirizzo libero al livello %d è %d \n",level,emptyIdx(&(alloc->bitmap),level));
  if (emptyIdx(&(alloc->bitmap),level)==-1 ) { // no buddies on this level
    //printf("non ci sono buddy in questo livello");
    BuddyListItem* parent_ptr=BuddyAllocator_getBuddy(alloc, level-1);
    if (! parent_ptr)
      return 0;

  }
  int idx=emptyIdx(&(alloc->bitmap),level);
  assert(idx!=-1);   //controllo che vada tutto bene
  BitMap_setBit(&(alloc->bitmap), idx , 1);       
  int idx_temp=idx;                          
  while(BitMap_getBit(&(alloc->bitmap),parentIdx(idx_temp))==0 && idx_temp>=1){   //setto tutti i bit dei genitori a 1
    BitMap_setBit(&(alloc->bitmap), parentIdx(idx_temp),1);
    idx_temp=parentIdx(idx_temp);
  }
  //printf("\n indirizzo temp: %d    idx= %d",idx_temp,idx);
  BuddyListItem* item=BuddyAllocator_createListItem(alloc,idx);
  return item;
}


void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item){
  BitMap_setBit(&(alloc->bitmap),item->idx,0);
  int idx=item->idx;
  while(idx>0){
    if(BitMap_getBit(&(alloc->bitmap),buddyIdx(idx))==0){                  //il fratello è vuoto -> join
      BitMap_setBit(&(alloc->bitmap),parentIdx(idx),0);
      idx=parentIdx(idx);
    }
    else return;
  }
}

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  //printf("1");
  // we determine the level of the page
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  int  level=floor(log2(mem_size/(size+8)));
  //printf("2");
  // if the level is too small, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;
  //printf("3");
  printf("requested: %d bytes, level %d \n",
         size, level);

  // we get a buddy of that size;
  BuddyListItem* buddy=BuddyAllocator_getBuddy(alloc, level);
  if (! buddy)
    return 0;
  //printf("4");
  // we write in the memory region managed the buddy address
  BuddyListItem** target= (BuddyListItem**)(buddy->start);
  *target=buddy;
  return buddy->start+8;
  
}

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  printf("\nfreeing %p", mem);
  // we retrieve the buddy from the system
  char* p=(char*) mem;
  p=p-8;
  BuddyListItem** buddy_ptr=(BuddyListItem**)p;
  BuddyListItem* buddy=*buddy_ptr;
  printf(" level %d", buddy->level);
  // sanity check;
  assert(buddy->start==p);
  BuddyAllocator_releaseBuddy(alloc, buddy);
  
}

