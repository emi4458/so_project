#include "buddy_allocator.h"
#include <stdio.h>


#define BUFFER_SIZE 102400
#define BUDDY_LEVELS 4
#define MEMORY_SIZE (1024*1024)
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS))

char buffer[BUFFER_SIZE]; // 100 Kb buffer to handle memory should be enough
char memory[MEMORY_SIZE];

BuddyAllocator alloc;
int main(int argc, char** argv) {

  //1 we see if we have enough memory for the buffers
  int req_size=BuddyAllocator_calcSize(BUDDY_LEVELS);
  printf("size requested for initialization: %d/BUFFER_SIZE\n", req_size);

  //2 we initialize the allocator
  printf("init... ");
  BuddyAllocator_init(&alloc, BUDDY_LEVELS,
                      buffer,
                      BUFFER_SIZE,
                      memory,
                      MIN_BUCKET_SIZE);
  printf("DONE\n");

  // void* p1=BuddyAllocator_malloc(&alloc, 100);
  // BitMap_print(&(alloc.bitmap));
  // void* p2=BuddyAllocator_malloc(&alloc, 100);
  // BitMap_print(&(alloc.bitmap));
  
  // //void* p3=BuddyAllocator_malloc(&alloc, 100000);
  // BitMap_print(&(alloc.bitmap));
  
  // BuddyAllocator_free(&alloc, p1);
  // BitMap_print(&(alloc.bitmap));
 
  // BuddyAllocator_free(&alloc, p2);
  // BitMap_print(&(alloc.bitmap));
  
  // //BuddyAllocator_free(&alloc, p3);
  // BitMap_print(&(alloc.bitmap));
  // void* p5=BuddyAllocator_malloc(&alloc, 100000000);
  // printf("%d\n",p5);

  void ** array[10];
  for(int i=0;i<8;i++){
    array[i]=BuddyAllocator_malloc(&alloc,100);
  }
  BitMap_print(&(alloc.bitmap));
  printf("LIBERO INDICE 12 E 13, QUINDI ANCHE IL 6");
  BuddyAllocator_free(&alloc,array[4]);
  BuddyAllocator_free(&alloc,array[5]);
  BitMap_print(&(alloc.bitmap));
  printf("RICHIEDO UN NODO AL LIVELLO 4\n");
  BuddyAllocator_malloc(&alloc,100);
  BitMap_print(&(alloc.bitmap));
  printf("RICHIEDO UN NODO TROPPO GRANDE\n");
  //BuddyAllocator_malloc(&alloc,10000000000000000000000);
  printf("RILASCIO DUE VOLTE LO STESSO (NODO 8)\n");
  BuddyAllocator_free(&alloc,array[5]);
  BuddyAllocator_free(&alloc,array[5]);

  for (int j=0;j<8;j++){  
    BuddyAllocator_free(&alloc,array[j]);
  }

  // for(int i=0;i<9;i++){
  //   array[i]=BuddyAllocator_malloc(&alloc,200000);
  //   BitMap_print(&(alloc.bitmap));
  // }
  //int* a=(int*)malloc(sizeof(int)*2);

  BuddyAllocator_malloc(&alloc,200000);
  BitMap_print(&(alloc.bitmap));
  BuddyAllocator_malloc(&alloc,200000);
  BitMap_print(&(alloc.bitmap));
  BuddyAllocator_malloc(&alloc,200000);
  BitMap_print(&(alloc.bitmap));
  BuddyAllocator_malloc(&alloc,200000);
  BitMap_print(&(alloc.bitmap));
 
}
