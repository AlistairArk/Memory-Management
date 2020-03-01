#include "memory_management.h"
int brk(void *addr);
void *sbrk(intptr_t increment);
/*
Author: sc17jhd
*/



int blockMultiple(int blockSize,int multiple){
    /*
    Ensures block sizes are multiples of 4096
    Padds requested memory for alignment (i.e. requested memory is made to multiples of 8)
	*/
    if (blockSize<multiple){
        blockSize = multiple;
    }else{
        if (blockSize%multiple){
            blockSize = ((blockSize/multiple)+1)*multiple;       // Too large to store as one block so split into multiple and link together
        }
    }
    return(blockSize);

}









void allData(void * ptr){
	/*
	The following is a test function used for displaying linked lists throughout a contiguous block of memory.
	By handing it a pointer to an allocated address, the function will output an ordered list of each blocks header data.
	*/

    char* head = ptr-sizeof(block);
    block *p = (block *)head;   // Check struct header and get size of block


    // Check block for links to see if it needs to be freed
    p = (block *)(head - p->i); 		// Jump to first item in linked list  (i.e. start of memory block)
    int allocated=p->s;;				// Amount of memory allocated in linked block
    int toFree=p->f;					// The amount of memory to be freed if block is empty
    while (p->nib){ 					// Loop until end of block is reached
    	p = (block *)p->nib; 			// go to next block
    	allocated+=p->s;
    }
}











char* find(size_t size){
	/*
	This function will traverse the linked list to find a free space.
	If a free space in an already existing block is found return pointer to the space,
	otherwise allocate normally and return the head
	*/

	if (! fb){ // IF fist block has yet to be created
	    return (NULL);
	}





	bool found=0;
    char* head = fb;    // Point to the current block header as we traverse the blocks (start with first block)
    char* nb  = NULL; 	// Points to next block
    char* nib = NULL;	// Points to next in block
    char* lib = NULL;	// Points to last in block
	int blockSize;		// Size of current block
	int freeSpace;		// Free space in block
	int blockAddress;   // Points to stat

	block *p = (block *)head;

	int indexPointer = 0; // Points to the index of a header in block. Grows and shrinks as the header is moved. Index is used in free space calculations.

	/*
	Using NSB and LSB we will use the indexes to deduce the space between blocks in case something can go between them.

	head will point to current header
	*/


	while (1){

		if (! p->i){ 						// If index 0 get block size
			blockSize = p->f;
			nb = p->nb; 					// Set next block so it can be traversed after nib is finished
			nib = p->nib;
			lib = NULL;
			indexPointer = 0;				// Reset index pointer
		}


		// Check for free available space in block
		if (nib){							// Check for space in between current and next block
			block* p2 = (block *)nib; 		// Pointer points to the next block
			freeSpace = (p->i + p->s) - (p2->i); 	// (End index of current block) - (Start index of next block)

		}else{		// If no next block exists check from current block to end of free
			freeSpace = blockSize - (p->i + p->s);
		}



		// PEFORM SPLIT
		if (freeSpace>=size){
			indexPointer = p->i + p->s;
			found = 1;				// Trigger found flag
			char* lastHead = head;  // Store head to seat as lib for new block
			head = head+p->s; 		// Set new head end of current block

			// Swap around pointers
			p->nib = head;			// Make prev block point to new block
			if (nib){ 				// If next item in block is not null (next block exists)
				p = (block *)nib; 	// Connect next block to new block
				p->lib = head;
			}

			// Set pointers for new block to finalize doubly linked list
			p = (block *)head;
			p->lib = lastHead; 		// Head is the last block
			p->nib = nib;			// Point to nib to connect to any block in front of new block
			p->s = size;
			p->i = indexPointer;

			l=head; 				// Set last block pointer so it can be returned

		}


	    if (found){
		    return (head);
	    }else{
	    	// If there are no more blocks to traverse through
	    	if (!nib && !nb){
		    	return (NULL);
	    	}else{

	    		// CHECK nsb nb pointers
	    		if (nib){ // Do nib before moving to the next block
	    			head = nib;
		    		p = (block *)nib;
					lib = head;
					nib = p->nib;

	    		}else{
	    			p = (block *)nb;	// Point to next block and start traversal over
					head = nb;
	    		}
	    	}
	    }
	}
}



bool mergeBlock(char* ptr,size_t size){
	/*
	Loop until end of allocated memory is reached or a suitable block to merge with is found
	If a merge is going to occur increase merge segments and return 1
	else, if suitable adjacent block to merge with was not fount return 0.
	*/

	if (fb){
		block *p = (block *)fb; // Point to the first block
		int endReached=0;
		char* head = fb;		// Initialize pointer on first block

		while (!endReached){ 			    // Loop until last block is reached

			// (If, start of a allocated memory segment) && (Segment End == start of newly allocated memory)
			if (p->f && ((head+p->f)==ptr)){
                p->f += size;				// Grow Segment to merge
                if (debug) printf("\n Will Merge");
				return(1);
			}

	    	if (!p->nb){					// Check if next block exists
	    		endReached=1;
	    	}else{
		    	head = p->nb; 				// set head as next block pointer
		    	p = (block *) p->nb; 		// go to next block
	    	}
	    }
	}

    return(0);
}



void nalloc(size_t size){
	/*
	nalloc (normal allocate)
	Performs new memory allocation to heap and merging of new memory blocks...
	*/

    char* head = sbrk(blockMultiple(size,4096));	// Create new block of multiple 4096

    if (debug){
        mm += blockMultiple(size,4096);
        printf("\n ALLOCATED MEMORY: %li",mm);
        printf("\n     malloc(%i)",blockMultiple(size,4096));
    }


	if (mergeBlock(head,blockMultiple(size,4096))){// Check if head is end of another block
		find(size); // Call find to assign pointer to the new block space

	} else {

	    block *p = (block *)head;

	    p->lb = l;                				    // LAST block pointer
	    p->nb = NULL;								// NEXT block pointer
	    p->lib = NULL;							    // LAST in block pointer (points to next maloc in current block)
	    p->nib = NULL;							    // NEXT in block pointer (points to last maloc in current block)
	    p->f  = blockMultiple(size,4096);			// FREE size (for later use when freeing memory)
	    p->s  = size;
	    p->i  = 0;								    // Set index in block as 0



	    if (l){ //If last block is not NULL
		    // Link last allocated block to current block
		    block *p2 = (block *)l;
		    p2->nb = head; 							// NEXT block pointer

		    // Set last block as current block
		    l = head;
	    }else{
	    	fb = head; 								// Set pointer to First Block
	    	l = head; 								// Set pointer to First Block
	    }
	}
}

void *_malloc(size_t size){


    if(size <= 0){ //If the requested size is 0, then malloc() returns either Null, or a unique pointer value that can later be successfully passed to free
        return NULL;
    }

    size = blockMultiple((size + sizeof(block)), 8);    // Add header size onto block & padd out to multiple of 8
    size_t s = size;

    char* head = find(size);
    if (!head){					 // If no free space was found

    	nalloc(size); 			 // Allocate memory normally
    }
    return (l+sizeof(block));    // Return block pointer to free
}




void _free(void * ptr){        // Take pointer and free

    char* head = ptr-sizeof(block);
    block *p = (block *)head;   // Check struct header and get size of block

	p->s = 0; //Set size zero so it's seen as free space

    block *lib = (block *)p->lib; // get block
	block *nib = (block *)p->nib; // get block

	// Merge space between lib and nib
	if (p->lib && p->nib){
    	lib->nib = p->nib; 		// Last in block points to next block
		nib->lib = p->lib; 	    // Next in block points to last block
	}else{
		if (p->lib){			// If there is a last block but no next block set pointers null
    		lib->nib = NULL;
    	}

    	/* If there is a next block but no last block set pointers null
    	 Don't clear pointer if header is at the start of allocated memory */
    	if (p->nib && p->i){
    		nib->lib = NULL; // Shrink front
		}
	}

	// Link the blocks together
	block *lb = (block *)p->lb;		// get block
	block *nb = (block *)p->nb;		// get block
    if (p->lb&&p->nb){ 				// If there is a last block and next block connect them
    	lb->nb = p->nb; 			// Link pointers together
		nb->lb = p->lb; 			// Link pointers together
    }else{
    	if (p->lb){ 				// If there is a last block but no next block set pointers null
    		lb->nb = NULL;
    		l = p->lb;// This must be the end of the list. Push pointer back
    	}
    	if (p->nb){ 				// If there is a next block but no last block set pointers null
    		nb->nb = NULL;
    	}
    }
	/* Block is now "free" as it's removed from the chain in linked list.
	 To save on system calls, memory will remain allocated until a contiguous block of memory has been completely freed */




    // Check contiguous block for links to see if it needs to be freed
    p = (block *)(head - p->i); 		// Jump to first item in linked list  (i.e. start of memory block)
    int allocated=p->s; 				// Amount of memory allocated in linked block
    int toFree=p->f;					// The amount of memory to be freed if block is empty
    char* fbPush=p->nb;					// Next block if fb needs to be moved forward

    while (p->nib){ 					// Loop until end of block is reached
    	p = (block *)p->nib; 			// go to next block
    	allocated+=p->s;

    }


    /*
    If none of the memory allocated in the block is in use, free the space with sbrk
    else, if allocated memory is still in use don't free.

	This is done to reduce system calls.
    */

    if (!allocated){






        sbrk(-toFree);

        if (debug){
            mm-=toFree;
            printf("\n     free(%i)",toFree);
            printf("\n ALLOCATED MEMORY: %li",mm);
        }


    	// }
    	/*
    	If block containing first pointer was freed push the first pointer into the next available block
    	else, set first pointer to null.
    	(i.e. setting the pointer will prevent accidental reassignment and it will be set to null once all memory has been freed)
    	*/
        if (head==fb)fb=fbPush;
    }
}








void main(){

    if (debug){ // MALLOC FREE TEST FUNCTION
        char* t1 = _malloc(4000);
        // char* t10 = _malloc(4000);
        _free(t1);
        // printf("\n ALLOCATED MEMORY - A: %li",mm);
        // char* t2 = _malloc(4100);
        // char* t3 = _malloc(20);
        // char* t4 = _malloc(20);
        // char* t5 = _malloc(12222);
        // char* t6 = _malloc(20);
        // char* t7 = _malloc(20);
        // printf("\n ALLOCATED MEMORY - B: %li",mm);
        // _free(t7);
        // _free(t6);
        // _free(t5);
        // _free(t4);
        // _free(t3);
        // _free(t2);
        // _free(t10);
        // printf("\n ALLOCATED MEMORY - C: %li",mm);
        printf("\n\n");
    }

}
