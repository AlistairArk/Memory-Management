#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

void * _malloc(size_t size);
void _free(void * ptr);

// Struct block header (bh)
typedef struct block{
    char *lb;  // LAST block pointer
    char *nb;  // NEXT block pointer
    char *lib;  // LAST in block pointer (points to last maloc in current block)
    char *nib;  // NEXT in block pointer (points to next maloc in current block)
    int  s;   // Block SIZE
    int  f;   // FREE size 				(For use in the first block. Determins the amount of memory to be freed)
    int  i;   // Block INDEX			(Index of item in block)

}block;

char *l = NULL; 	// LAST block
char *fb = NULL; 	// FIRST BLOCK

char *glib = NULL; // Global pointers used for linking after a split
char *gnib = NULL; // Global pointers used for linking after a split


long mm=0; 		// Memory made (Counter made for testing so I can ensure all memory allocated is freed)
bool debug=1; 	// Set to 1 to enable debugging (Tests and prints will be enabled)
