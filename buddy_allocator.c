#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(int idx){    
  if(idx==0){
    return 0;
  }
  return (int)floor(log2(idx+1));   
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx+1;         
  }
  return idx-1;
}

int parentIdx(int idx){   //modificata per numerazione albero da 0
  if(idx%2==0){
	return idx/2-1;          
  }
  else return idx/2;
}

int firstSonIdx(int idx){
  return idx*2+1;
}

int nBuddyForLevel(int level){
  return 1<<level;
}

int emptyIdx(BitMap* bitmap, int level){                                     //trova il primo indice vuoto del livello, -1 se non c'è
  for(int i=nBuddyForLevel(level)-1 ; i<nBuddyForLevel(level)*2-1 ; i++){
    if(BitMap_getBit(bitmap,i)==0) return i;                                
  }
  return -1;
}

// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int list_items=1<<(num_levels+1); // maximum number of allocations, used to determine the max list items
  int list_alloc_size=(sizeof(MemoryItem)+sizeof(int))*list_items;
  return list_alloc_size;
}


// creates an item from the index
MemoryItem* BuddyAllocator_createMemoryItem(BuddyAllocator* alloc,int idx){
  MemoryItem* item=(MemoryItem*)PoolAllocator_getBlock(&alloc->p_allocator);
  item->idx=idx;
  item->level=levelIdx(idx);
  if(idx==0){
    item->start=alloc->memory;
  }
  else{
    item->start= alloc->memory + (((idx+1)-(1<<levelIdx(idx))) << (alloc->num_levels-item->level) )* alloc->min_bucket_size;  
  }
  item->size=(1<<(alloc->num_levels-item->level))*alloc->min_bucket_size;
  printf("Creating Item. idx:%d, level:%d, start:%p, size:%d\n", item->idx, item->level, item->start, item->size);
  return item;
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
  BitMap_alloc(&(alloc->bitmap),(1<<(num_levels+1))-1);
  assert (num_levels<MAX_LEVELS);
  assert (buffer_size>=BuddyAllocator_calcSize(num_levels));
  
  int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
  int list_alloc_size=(sizeof(MemoryItem)+sizeof(int)+sizeof(int))*list_items;    //ho aggiunto un sizeof int perchè sizeof int è 4 e a me serve 8

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d", num_levels);
  printf("\tmax list entries %d bytes\n", list_alloc_size);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
  char *list_start=buffer;
  PoolAllocatorResult init_result=PoolAllocator_init(&alloc->p_allocator,
						     sizeof(MemoryItem),
						     list_items,
						     list_start,
						     list_alloc_size);
  printf("%s\n",PoolAllocator_strerror(init_result));
};


MemoryItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  if (level<0){
    return 0;
  }
  assert(level <= alloc->num_levels);
  //printf("-Il primo nodo libero al livello %d è %d \n",level,emptyIdx(&(alloc->bitmap),level)+1);
  if (emptyIdx(&(alloc->bitmap),level)==-1 ) { // no buddies on this level
    MemoryItem* parent_ptr=BuddyAllocator_getBuddy(alloc, level-1);
    if (! parent_ptr){
      return 0;
    }
  }
  int idx=emptyIdx(&(alloc->bitmap),level);
  assert(idx!=-1);   //controllo che vada tutto bene
  BitMap_setBit(&(alloc->bitmap), idx , 1);     
  int idx_temp=idx;
  //setto tutti i bit dei genitori a 1
  if(level>0){    //se non ha genitori è inutile                      
    while(BitMap_getBit(&(alloc->bitmap),parentIdx(idx_temp))==0 && idx_temp>=1){   
      BitMap_setBit(&(alloc->bitmap), parentIdx(idx_temp),1);
      idx_temp=parentIdx(idx_temp);
    }
  }
  //setto tutti i bit dei figli a 1
  if(idx!=alloc->bitmap.num_bits-1){ //se non ha figli è inutile
    int aux=2;
    int n_bits=alloc->bitmap.num_bits;
    for(int idx2=idx*2+1;aux+idx2<=n_bits;idx2=(idx2*2)+1){         
      for(int i=0;i<aux;i++){
        BitMap_setBit(&(alloc->bitmap),i+idx2,1);
      }
      aux*=2;
    }
  }
  MemoryItem* item=BuddyAllocator_createMemoryItem(alloc,idx);
  return item;
}


void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, MemoryItem* item){
  BitMap_setBit(&(alloc->bitmap),item->idx,0);
  int idx=item->idx;
  int idxx=idx;
  while(idx>0){
    if(BitMap_getBit(&(alloc->bitmap),buddyIdx(idx))==0){                  //il fratello è vuoto -> join
      BitMap_setBit(&(alloc->bitmap),parentIdx(idx),0);
      idx=parentIdx(idx);
    }
    else idx=0;
  }
  int aux=2;
  int n_bits=alloc->bitmap.num_bits;
  for(int idx2=idxx*2+1;aux+idx2<=n_bits;idx2=(idx2*2)+1){         //setto tutti i bit dei figli a 0
    for(int i=0;i<aux;i++){
      BitMap_setBit(&(alloc->bitmap),i+idx2,0);
    }
    aux*=2;
  }
  PoolAllocatorResult release_result=PoolAllocator_releaseBlock(&alloc->p_allocator, item); 
  printf("%s\n",PoolAllocator_strerror(release_result));
}



//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  // we determine the level of the page
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
  int  level=floor(log2(mem_size/(size+8)));
  if(level<0){
    return 0;
  }
  // if the level is too small, we pad it to max
  if (level>alloc->num_levels) level=alloc->num_levels;
  printf("requested: %d bytes, level %d \n", size, level);
  // we get a buddy of that size;
  MemoryItem* buddy=BuddyAllocator_getBuddy(alloc, level);
  if (! buddy){
    printf("---NON HO ABBASTANZA MEMORIA---");
    return 0;
  }
  // we write in the memory region managed the buddy address
  MemoryItem** target= (MemoryItem**)(buddy->start);
  *target=buddy;
  return buddy->start+8;
}



//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  if(mem==0){
    return;
  }
  printf("\nfreeing %p", mem);
  // we retrieve the buddy from the system
  char* p=(char*) mem;
  p=p-8;
  MemoryItem** buddy_ptr=(MemoryItem**)p;
  MemoryItem* buddy=*buddy_ptr;
  printf(" level %d idx: %d\n", buddy->level,buddy->idx);
  // sanity check;
  assert(buddy->start==p);
  BuddyAllocator_releaseBuddy(alloc, buddy);
}
