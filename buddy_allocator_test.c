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

  // printf("\n******************************************************CHIEDO E RILASCIO MEMORIA LIVELLO 0******************************************************\n");

  // void* m0=BuddyAllocator_malloc(&alloc,530000);
  // BitMap_print(&(alloc.bitmap));
  // BuddyAllocator_free(&alloc,m0);
  // BitMap_print(&(alloc.bitmap));
  

  // printf("\n******************************************************CHIEDO E RILASCIO MEMORIA LIVELLO 1******************************************************\n");

  
  // void* m1=BuddyAllocator_malloc(&alloc,280000);
  // BitMap_print(&(alloc.bitmap));
  // BuddyAllocator_free(&alloc,m1);
  // BitMap_print(&(alloc.bitmap));

  printf("\n******************************************************RIEMPIMENTO E SVUOTAMENTO INVERSO******************************************************\n");

  void* array[600];                                  //riempio e svuoto l'albero in ordine inverso
  for(int i=0;i<16;i++){
    
    void* x=BuddyAllocator_malloc(&alloc,10);
    array[i]=x;
    BitMap_print(&(alloc.bitmap));
    printf("stato pool:\n");
    BitMap_print(&(alloc.list_allocator.bitmap));
    
  }
  for(int j=0;j<16;j++){
    
    void* y=array[j];
    BuddyAllocator_free(&alloc,y);          //CONTINUA DA QUI: A 8 NON GLI DA PIÙ FASTIDIO
    BitMap_print(&(alloc.bitmap));
    printf("stato pool:\n");
    BitMap_print(&(alloc.list_allocator.bitmap));
    
  }
  printf("\n\n\n\n\n\n\n\n\nindici pool impegnati:\n");
  BitMap_print(&(alloc.list_allocator.bitmap));

  // printf("\n******************************************************RIEMPIO L'ALBERO******************************************************\n");

  // for(int i=0;i<16;i++){
  //   array[i]=BuddyAllocator_malloc(&alloc,10);
  //   BitMap_print(&(alloc.bitmap));
  //   printf("indici pool impegnati:\n");
  //   BitMap_print(&(alloc.list_allocator.bitmap));
  // }
  // BitMap_print(&(alloc.bitmap));

  // printf("\n******************************************************SVUOTAMENTO RANDOM******************************************************\n");

  // for(int i=0;i<16;i++){                              //riempio in modo casuale l albero
  //   int random = rand() % 16 + 0;
  //   printf("Tolgo il nodo: %d\n",random+16+1);
  //   BuddyAllocator_free(&alloc,array[random]);
  //   BitMap_print(&(alloc.bitmap));
  // }

  // void* mm=BuddyAllocator_malloc(&alloc,10);
  // BitMap_print(&(alloc.bitmap));
  // printf("stato pool:\n");
  // BitMap_print(&(alloc.list_allocator.bitmap));

  // void* mm1=BuddyAllocator_malloc(&alloc,10);
  // BitMap_print(&(alloc.bitmap));
  // printf("stato pool:\n");
  // BitMap_print(&(alloc.list_allocator.bitmap));

  // BuddyAllocator_free(&alloc,mm1);
  // BitMap_print(&(alloc.bitmap));
  // printf("stato pool:\n");
  // BitMap_print(&(alloc.list_allocator.bitmap));

  // BuddyAllocator_free(&alloc,mm);
  // BitMap_print(&(alloc.bitmap));
  // printf("stato pool:\n");
  // BitMap_print(&(alloc.list_allocator.bitmap));

  
}
