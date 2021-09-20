# so_project

Progetto sistemi operativi 

-Buddy allocator using bitmap: Sono state modificate le strutture del buddy allocator e del pool allocator per funzionare senza l’utilizzo di liste e alberi implementandola solamente con l’uso di bitmap.
L’albero rappresentato dalla bitmap è numerato partendo dall’indice 1 e gestito in modo tale che nel momento in cui viene utilizzato un nodo vengano occupati anche i corrispettivi figli e genitori.


In buddy_allocat_test viene testato il funzionamento del buddy allocator tramite delle richieste di diverso tipo, ordine e dimensione. 
Viene allocata memoria tramite void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

Viene rilasciata la memoria con void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);

Dopo ogni operazione viene stampata la bitmap per controllarne lo stato  tramite la funzione  void BitMap_print(BitMap* bmap);





