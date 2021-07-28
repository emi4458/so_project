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
  BuddyAllocator_init(&alloc, 
                      BUDDY_LEVELS,
                      buffer,
                      BUFFER_SIZE,
                      memory,
                      MIN_BUCKET_SIZE);
  printf("DONE\n");

  printf("\n******************************************************CHIEDO E RILASCIO MEMORIA LIVELLO 0******************************************************\n");

  void* m0=BuddyAllocator_malloc(&alloc,530000);
  BitMap_print(&(alloc.bitmap));
  void* m2=BuddyAllocator_malloc(&alloc,530000);
  BuddyAllocator_free(&alloc,m0);
  BitMap_print(&(alloc.bitmap));
  

  // printf("\n******************************************************LIBERO LIVELLO 0******************************************************\n");

  
  // BuddyAllocator_free(&alloc,m);
  
  // BitMap_print(&(alloc.bitmap));

  printf("\n******************************************************RIEMPIMENTO E SVUOTAMENTO INVERSO******************************************************\n");

  // void ** array[16];                                  //riempio e svuoto l'albero in ordine inverso
  // for(int i=0;i<16;i++){
  //   array[i]=BuddyAllocator_malloc(&alloc,800000);
  //   BitMap_print(&(alloc.bitmap));
  // }
  // for(int j=15;j>=0;j--){
  //   BuddyAllocator_free(&alloc,array[j]);
  //   BitMap_print(&(alloc.bitmap));
  // }

  printf("\n******************************************************RIEMPIO L'ALBERO******************************************************\n");

  // for(int i=0;i<16;i++){                              //riempio in modo casuale l albero
  //   array[i]=BuddyAllocator_malloc(&alloc,100000);
  // }
  // BitMap_print(&(alloc.bitmap));

  printf("\n******************************************************SVUOTAMENTO RANDOM******************************************************\n");

  // for(int i=0;i<16;i++){                              //riempio in modo casuale l albero
  //   int random = rand() % 16 + 0;
  //   printf("Tolgo il nodo: %d",random+16);
  //   BuddyAllocator_free(&alloc,array[random]);
  //   BitMap_print(&(alloc.bitmap));
  // }


  void* m1=BuddyAllocator_malloc(&alloc,4000000);
  BitMap_print(&(alloc.bitmap));
}
