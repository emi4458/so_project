#include "pool_allocator.h"
#include "bit_map.h"

static const char* PoolAllocator_strerrors[]=
  {"Success",
   "NotEnoughMemory",
   "UnalignedFree",
   "OutOfRange",
   "DoubleFree",
   0
  };

const char* PoolAllocator_strerror(PoolAllocatorResult result) {
  return PoolAllocator_strerrors[-result];
}

PoolAllocatorResult PoolAllocator_init(PoolAllocator* a,
		       int item_size,
		       int num_items,
		       char* memory_block,
		       int memory_size) {

        
  // we first check if we have enough memory
  // for the bookkeeping
  int requested_size= num_items*(item_size+sizeof(int));
  if (memory_size<requested_size)
    return NotEnoughMemory;

  BitMap_alloc(&(a->bitmap),num_items);
  a->item_size=item_size;
  a->size=num_items;                     
  a->buffer_size=item_size*num_items+1; //ho dovuto mettere +1
  a->size_max = num_items;
  a->buffer=memory_block; 
  return Success;
}

void* PoolAllocator_getBlock(PoolAllocator* a) {
  
  int free=firstFreeIdx(&(a->bitmap));
  if(free==-1) return 0;
  --a->size;
  BitMap_setBit(&(a->bitmap),free,1);
  char* block_address=a->buffer+(free*a->item_size);
  return block_address;
  
}

PoolAllocatorResult PoolAllocator_releaseBlock(PoolAllocator* a, void* block_){
  //we need to find the index from the address
  char* block=(char*) block_;
  int offset=block - a->buffer;

  //sanity check, we need to be aligned to the block boundaries
  if (offset%a->item_size)
    return UnalignedFree;

  int idx=offset/a->item_size;

  //sanity check, are we inside the buffer?
  if (idx<0 || idx>=a->size_max)
    return OutOfRange;

  //is the block detached?
  if (BitMap_getBit(&(a->bitmap),idx)==0)
    return DoubleFree;

  // all fine, we insert in the head
  BitMap_setBit(&(a->bitmap),idx,0);
  ++a->size;
  return Success;
}

int firstFreeIdx(BitMap* bitmap){              //trova il primo indice libero (a 0), -1 se piena
  for(int i=0;i<bitmap->num_bits;i++){
    if(BitMap_getBit(bitmap,i)==0) return i;
  }
  return -1;
}