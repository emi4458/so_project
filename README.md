# so_project

Progetto sistemi operativi 

-Buddy allocator using bitmap: Sono state modificate le strutture di buddy allocator e di pool allocator viste a lezione per funzionare senza l’utilizzo di liste e di alberi implementandola solamente con l’uso di bitmap.
La gestione della memoria di buddy allocator è implementata tramite una bitmap che rappresenta un albero numerato partendo dall’indice 1. 
Sono state aggiunte delle brevi funzioni utili per gestire la bitmap e i rispettivi indici nell’albero. La maggior parte delle funzioni originali e delle strutture sono state modificate e adattate. 

In buddy_allocator_test viene testato il funzionamento del buddy allocator tramite delle richieste di diverso tipo, ordine e dimensione.  Viene effettuata una richiesta di memoria al livello 0 dell’albero e poi rilasciata, stessa cosa con il livello 1. Successivamente viene richiesta memoria al livello più basso fino a riempire l’albero e poi rilasciata in ordine inverso. Infine dopo aver riempito nuovamente l’albero viene richiesta ulteriore memoria, ci aspettiamo quindi di ricevere degli errori.
Le funzioni di test sono le stesse del codice originale: BuddyAllocator_malloc e BuddyAllocator_free. Bitmap_print stampa lo stato della bitmap.

Per provare il codice è sufficiente eseguire make e successivamente il file buddy_allocator_test.
Il programma stamperà lo stato della bitmap ad ogni operazione così da poterne verificare lo stato.




