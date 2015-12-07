//
//  COMP1927 Assignment 1 - Vlad: the memory allocator
//  allocator.c ... implementation
//
//  Created by Liam O'Connor on 18/07/12.
//  Modified by John Shepherd in August 2014, August 2015
//  Modified by Timothy Hor in August 2015
//  Copyright (c) 2012-2015 UNSW. All rights reserved.
//
//  T/D = for testing/debugging purposes
//

#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define HEADER_SIZE    sizeof(struct free_list_header)  
#define MAGIC_FREE     0xDEADBEEF
#define MAGIC_ALLOC    0xBEEFDEAD

typedef unsigned char byte;
typedef u_int32_t vlink_t;
typedef u_int32_t vsize_t;
typedef u_int32_t vaddr_t;

typedef struct free_list_header {
   u_int32_t magic;  // ought to contain MAGIC_FREE
   vsize_t size;     // # bytes in this block (including header)
   vlink_t next;     // memory[] index of next free block
   vlink_t prev;     // memory[] index of previous free block
} free_header_t;

// Global data

static byte *memory = NULL;   // pointer to start of allocator memory
static vaddr_t free_list_ptr; // index in memory[] of first block in free list
static vsize_t memory_size;   // number of bytes malloc'd in memory[]

// helper function, sourced from class forum [T/D]
void showHeaderInfo(free_header_t* header) {
   printf("\tChunk index %d, size %d, tag %s, next %d, prev %d\n", (void*)header - (void*)memory, header->size, (header->magic == MAGIC_FREE) ? "FREE" : "ALLOC", header->next, header->prev);
}

// convert index to pointer
free_header_t *indexToPtr(u_int32_t index) {
   assert(index >= 0);
   return (free_header_t *) &memory[index];
}

// convert pointer to index
vaddr_t ptrToIndex(free_header_t *object) {
   return ( (byte *)object - memory );
   // need to subtract 'memory' since it probably won't start at address 0 on an actual computer
}

// Input: size - number of bytes to make available to the allocator
// Output: none              
// Precondition: Size is a power of two.
// Postcondition: `size` bytes are now available to the allocator
// 
// (If the allocator is already initialised, this function does nothing,
//  even if it was initialised with different size)

void vlad_init(u_int32_t size) {
   // size = some_number; // override MEMORY_SIZE for T/D (instead of modifying vlad.c)
   
   if (size < 512) size = 512;
   // check for power of 2 (using bitwise AND comparison)
   if ((size & (size-1)) == 0) {
      // size is a power of 2: no need to modify
   } else {
      // size is not a power of 2: compute the next highest power of 2 of 32-bit unsigned int
      // <snippet> this algorithm is from: http://graphics.stanford.edu/~seander/bithacks.html/RoundUpPowerOf2
      size |= size >> 1;
      size |= size >> 2;
      size |= size >> 4;
      size |= size >> 8;
      size |= size >> 16;
      size++;
      // </snippet>
   }
   
   // allocate the memory block
   memory = (byte *)malloc(size*sizeof(byte));
   if (memory == NULL) {
      fprintf(stderr, "vlad_init: insufficient memory\n");
      abort();
   } else {
      memory_size = size;
      free_list_ptr = 0;
   }
   
   // set up region header
   free_header_t *initHeader = (free_header_t *) memory;
   initHeader->magic = MAGIC_FREE;
   initHeader->size = size;
   initHeader->next = initHeader->prev = 0;
}

// check whether free headers have the correct arbitrary number
void validateFreeHeader(free_header_t *header) {
   if (header->magic != MAGIC_FREE) {
      fprintf(stderr, "Memory corruption\n");
      abort();
   }
}

// check whether allocated headers have the correct arbitrary number
void validateAllocHeader(free_header_t *header) {
   if (header->magic != MAGIC_ALLOC) {
      fprintf(stderr, "Memory corruption\n");
      abort();
   }
}

// Input: n - number of bytes requested
// Output: p - a pointer, or NULL
// Precondition: n is < size of memory available to the allocator
// Postcondition: If a region of size n or greater cannot be found, p = NULL 
//                Else, p points to a location immediately after a header block
//                      for a newly-allocated region of some size >= 
//                      n + header size.

void *vlad_malloc(u_int32_t n) {
   // declarations
   int regionFound = 0; // default: no free region found
   free_header_t *curr = indexToPtr(free_list_ptr);
   
   // traverse the free list
   for (curr = curr; curr->next != free_list_ptr; curr = indexToPtr(curr->next)) {
      validateFreeHeader(curr);
      if (curr->size >= HEADER_SIZE+n) {
         regionFound = 1;
         break;
      }
   }
   // account for the "final" node (or initially, the only/single node) which would fail the loop condition 'curr->next != free_list_ptr'
   if (regionFound == 0 && curr->size >= HEADER_SIZE+n) {
      validateFreeHeader(curr);
      regionFound = 1;
   }
   
   // no suitable region found
   if (regionFound == 0) {
      // printf("No region found: returned NULL\n"); // T/D
      return NULL;
   }
   
   // check if desired memory size could fit within half the region
   // repeatedly split, until there is a region of size s such that: s >= HEADER_SIZE+n but s/2 < HEADER_SIZE+n
   vsize_t halfRegionSize = curr->size >> 1; // '>> 1' is the same as '/2'
   while (curr->size >= HEADER_SIZE+n && halfRegionSize >= HEADER_SIZE+n) {
      validateFreeHeader(curr);
      free_header_t *halfHeader = indexToPtr(halfRegionSize);
      halfHeader->magic = MAGIC_FREE;
      halfHeader->size = halfRegionSize;
      
      free_header_t *origNext = indexToPtr(curr->next); // keep track of the header pointed to by curr->next before the split
      
      validateFreeHeader(halfHeader);
      halfHeader->next = curr->next;     // done this way because curr is a free_header_t * whereas prev
      halfHeader->prev = origNext->prev; // is an int index - so 'halfHeader->prev = curr' wouldn't work
      
      // change properties of previous header
      curr->size = halfRegionSize;
      curr->next = halfRegionSize;
      
      // change properties of next header
      // only splitting one side so origNext->size doesn't change
      origNext->prev = halfRegionSize;
      
      // halve size again
      halfRegionSize = curr->size >> 1;
   }
   
   // enforce that the free list must always contain at least one region
   validateFreeHeader(curr);
   vaddr_t currIndex = ptrToIndex(curr);
   if (curr->next == currIndex && curr->prev == currIndex) {
      printf("This was the only free region...returned NULL\n");
      return NULL;
   }
   
   // perform the allocation
   free_header_t *nextHeader = indexToPtr(curr->next);
   free_header_t *prevHeader = indexToPtr(curr->prev);
   nextHeader->prev = curr->prev; // adjust next and prev indices
   prevHeader->next = curr->next; // on the neighbouring headers
   curr->magic = MAGIC_ALLOC;
   
   // showHeaderInfo(curr); // T/D

   // if the global pointer free_list_ptr no longer points to a free region header (because it now points to an allocated header), adjust the pointer so that it does
   free_header_t *flp = indexToPtr(free_list_ptr);
   while (flp->magic == MAGIC_ALLOC) {
      validateAllocHeader(flp);
      free_list_ptr = flp->next;
      flp = indexToPtr(free_list_ptr);
      // there won't be an infinite loop because there must at least be one free region in the list
   }
   validateFreeHeader(flp);
   
   // T/D
   /* printf("free_list_ptr = %d\n", free_list_ptr);
   printf("List Traversal after ALLOC\n");
   free_header_t *tmp = curr;
   for (tmp = indexToPtr(free_list_ptr); tmp->next != free_list_ptr; tmp = indexToPtr(tmp->next))
      showHeaderInfo(tmp);
   showHeaderInfo(tmp); */
   
   // return a pointer sizeof(curr)*HEADER_SIZE bytes ahead
   return ((void *)((byte *)curr + HEADER_SIZE));
   // the (byte*) cast sets sizeof(curr) back to 1 so the pointer arithmetic works
}

// for each region in the free list, merge any adjacent regions with the same size and magic == MAGIC_FREE
void mergeRegions() {
   // traverse the free list
   free_header_t *curr;
   for (curr = indexToPtr(free_list_ptr); curr->next != free_list_ptr; curr = indexToPtr(curr->next)) {
      validateFreeHeader(curr);
      // note: merging regions must be of the same size (since they were originally split into two equal regions)
      if (/* adjacency check */ ptrToIndex(curr) + curr->size == curr->next &&
          /* header check */ curr->magic == MAGIC_FREE && indexToPtr(curr->next)->magic == MAGIC_FREE &&
          /* size check */ curr->size == indexToPtr(curr->next)->size) {
         
         // T/D
         /* printf("\nMAGIC_FREE = %d\ncurr->magic = %d\ncurr->next->magic = %d\n\n", MAGIC_FREE, curr->magic, indexToPtr(curr->next)->magic);
         printf("Regions to merge: %d <-> %d\n", ptrToIndex(curr), curr->next);
         printf("\n--------------------------------------------------\n"); */
         
         // do the merge ...
         curr->size = curr->size << 1; // multiply by 2
         vlink_t newNext = indexToPtr(curr->next)->next;
         curr->next = newNext;
         indexToPtr(newNext)->prev = ptrToIndex(curr);
      }
   }
   // any code in the for loop repeated here to account for the last node
   validateFreeHeader(curr);
   // merging regions must be of the same size (since they were originally split into two equal regions)
   if (/* adjacency check */ ptrToIndex(curr) + curr->size == curr->next &&
       /* header check */ curr->magic == MAGIC_FREE && indexToPtr(curr->next)->magic == MAGIC_FREE &&
       /* size check */ curr->size == indexToPtr(curr->next)->size) {
          
      // T/D
      /* printf("\nMAGIC_FREE = %d\ncurr->magic = %d\ncurr->next->magic = %d\n\n", MAGIC_FREE, curr->magic, indexToPtr(curr->next)->magic);
      printf("Regions to merge: %d <-> %d\n", ptrToIndex(curr), curr->next);
      printf("\n--------------------------------------------------\n"); */
         
      // do the merge ...
      curr->size = curr->size << 1; // multiply by 2
      vlink_t newNext = indexToPtr(curr->next)->next;
      curr->next = newNext;
      indexToPtr(newNext)->prev = ptrToIndex(curr);
   }
   
   // recursively merge previously combined regions
   vaddr_t currIndex = ptrToIndex(curr);
   // printf("\ncurrIndex = %d\ncurr->next = %d\ncurr->prev = %d\n", currIndex, curr->next, curr->prev); // T/D
   if (curr->next != currIndex || curr->prev != currIndex) {
      // traverse the list and check if there are any more regions that can be merged
      for (curr = indexToPtr(free_list_ptr); curr->next != free_list_ptr; curr = indexToPtr(curr->next)) {
         validateFreeHeader(curr);
         if (/* adjacency check */ ptrToIndex(curr) + curr->size == curr->next &&
             /* header check */ curr->magic == MAGIC_FREE && indexToPtr(curr->next)->magic == MAGIC_FREE &&
             /* size check */ curr->size == indexToPtr(curr->next)->size) {
            // printf("MERGE AGAIN\n"); // T/D
            mergeRegions();
         }
      }
   }
}

// used for adjusting header values of the region being freed in vlad_free
free_header_t* nextFreeHeader(free_header_t *objHeader) {
   if (indexToPtr(objHeader->next)->magic == MAGIC_ALLOC)
      return nextFreeHeader(indexToPtr(objHeader->next));
   else
      return indexToPtr(objHeader->next);
}

// used for adjusting header values of the region being freed in vlad_free
free_header_t* prevFreeHeader(free_header_t *objHeader) {
   if (indexToPtr(objHeader->prev)->magic == MAGIC_ALLOC)
      return prevFreeHeader(indexToPtr(objHeader->prev));
   else
      return indexToPtr(objHeader->prev);
}

// Input: object, a pointer.
// Output: none
// Precondition: object points to a location immediately after a header block
//               within the allocator's memory.
// Postcondition: The region pointed to by object can be re-allocated by 
//                vlad_malloc

void vlad_free(void *object) {
   // T/D
   /* printf("List Traversal before FREE\n");
   free_header_t *curr;
   for (curr = indexToPtr(free_list_ptr); curr->next != free_list_ptr; curr = indexToPtr(curr->next))
      showHeaderInfo(curr);
   showHeaderInfo(curr); */
   
   // enforce that object points to an allocated region
   free_header_t *objHeader = object - HEADER_SIZE;
   if (objHeader->magic != MAGIC_ALLOC) {
      fprintf(stderr, "Attempt to free non-allocated memory\n");
      abort();
   }
   
   // insert this header back into the free list:
   // 1. adjust values in objHeader (because they were left as-is during vlad_malloc and only neighbouring headers were changed)
   objHeader->next = ptrToIndex(nextFreeHeader(objHeader));
   objHeader->prev = ptrToIndex(prevFreeHeader(objHeader));
   // 2. adjust values in neighbouring headers
   free_header_t *nextHeader = indexToPtr(objHeader->next);
   free_header_t *prevHeader = indexToPtr(objHeader->prev);
   nextHeader->prev = prevHeader->next = ptrToIndex(objHeader);
   objHeader->magic = MAGIC_FREE;
   
   free_list_ptr = ptrToIndex(objHeader);
   
   // T/D
   /* printf("List Traversal before MERGE\n");
   for (curr = indexToPtr(free_list_ptr); curr->next != free_list_ptr; curr = indexToPtr(curr->next))
      showHeaderInfo(curr);
   showHeaderInfo(curr); */
   
   mergeRegions();
   
   // T/D
   /* printf("List Traversal after MERGE\n");
   for (curr = indexToPtr(free_list_ptr); curr->next != free_list_ptr; curr = indexToPtr(curr->next))
      showHeaderInfo(curr);
   showHeaderInfo(curr); */
}


// Stop the allocator, so that it can be init'ed again:
// Precondition: allocator memory was once allocated by vlad_init()
// Postcondition: allocator is unusable until vlad_int() executed again

void vlad_end(void) {
   free(memory);
   memory = NULL;
}


// Precondition: allocator has been vlad_init()'d
// Postcondition: allocator stats displayed on stdout

void vlad_stats(void) {
   // see showHeaderInfo() instead
   return;
}


//
// All of the code below here was written by Alen Bou-Haidar, COMP1927 14s2
//

//
// Fancy allocator stats
// 2D diagram for your allocator.c ... implementation
// 
// Copyright (C) 2014 Alen Bou-Haidar <alencool@gmail.com>
// 
// FancyStat is free software: you can redistribute it and/or modify 
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or 
// (at your option) any later version.
// 
// FancyStat is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>


#include <string.h>

#define STAT_WIDTH  32
#define STAT_HEIGHT 16
#define BG_FREE      "\x1b[48;5;35m" 
#define BG_ALLOC     "\x1b[48;5;39m"
#define FG_FREE      "\x1b[38;5;35m" 
#define FG_ALLOC     "\x1b[38;5;39m"
#define CL_RESET     "\x1b[0m"


typedef struct point {int x, y;} point;

static point offset_to_point(int offset,  int size, int is_end);
static void fill_block(char graph[STAT_HEIGHT][STAT_WIDTH][20], 
                        int offset, char * label);



// Print fancy 2D view of memory
// Note, This is limited to memory_sizes of under 16MB
void vlad_reveal(void *alpha[26]) {
    int i, j;
    vlink_t offset;
    char graph[STAT_HEIGHT][STAT_WIDTH][20];
    char free_sizes[26][32];
    char alloc_sizes[26][32];
    char label[3]; // letters for used memory, numbers for free memory
    int free_count, alloc_count, max_count;
    free_header_t * block;

	/*/ TODO
	// REMOVE these statements when your vlad_malloc() is done
    printf("vlad_reveal() won't work until vlad_malloc() works\n");
    return;*/

    // initilise size lists
    for (i=0; i<26; i++) {
        free_sizes[i][0]= '\0';
        alloc_sizes[i][0]= '\0';
    }

    // Fill graph with free memory
    offset = 0;
    i = 1;
    free_count = 0;
    while (offset < memory_size){
        block = (free_header_t *)(memory + offset);
        if (block->magic == MAGIC_FREE) {
            snprintf(free_sizes[free_count++], 32, 
                "%d) %d bytes", i, block->size);
            snprintf(label, 3, "%d", i++);
            fill_block(graph, offset,label);
        }
        offset += block->size;
    }

    // Fill graph with allocated memory
    alloc_count = 0;
    for (i=0; i<26; i++) {
        if (alpha[i] != NULL) {
            offset = ((byte *) alpha[i] - (byte *) memory) - HEADER_SIZE;
            block = (free_header_t *)(memory + offset);
            snprintf(alloc_sizes[alloc_count++], 32, 
                "%c) %d bytes", 'a' + i, block->size);
            snprintf(label, 3, "%c", 'a' + i);
            fill_block(graph, offset,label);
        }
    }

    // Print all the memory!
    for (i=0; i<STAT_HEIGHT; i++) {
        for (j=0; j<STAT_WIDTH; j++) {
            printf("%s", graph[i][j]);
        }
        printf("\n");
    }

    //Print table of sizes
    max_count = (free_count > alloc_count)? free_count: alloc_count;
    printf(FG_FREE"%-32s"CL_RESET, "Free");
    if (alloc_count > 0){
        printf(FG_ALLOC"%s\n"CL_RESET, "Allocated");
    } else {
        printf("\n");
    }
    for (i=0; i<max_count;i++) {
        printf("%-32s%s\n", free_sizes[i], alloc_sizes[i]);
    }

}

// Fill block area
static void fill_block(char graph[STAT_HEIGHT][STAT_WIDTH][20], int offset, char * label) {
    point start, end;
    free_header_t * block;
    char * color;
    char text[3];
    block = (free_header_t *)(memory + offset);
    start = offset_to_point(offset, memory_size, 0);
    end = offset_to_point(offset + block->size, memory_size, 1);
    color = (block->magic == MAGIC_FREE) ? BG_FREE: BG_ALLOC;

    int x, y;
    for (y=start.y; y < end.y; y++) {
        for (x=start.x; x < end.x; x++) {
            if (x == start.x && y == start.y) {
                // draw top left corner
                snprintf(text, 3, "|%s", label);
            } else if (x == start.x && y == end.y - 1) {
                // draw bottom left corner
                snprintf(text, 3, "|_");
            } else if (y == end.y - 1) {
                // draw bottom border
                snprintf(text, 3, "__");
            } else if (x == start.x) {
                // draw left border
                snprintf(text, 3, "| ");
            } else {
                snprintf(text, 3, "  ");
            }
            sprintf(graph[y][x], "%s%s"CL_RESET, color, text);            
        }
    }
}

// Converts offset to coordinate
static point offset_to_point(int offset,  int size, int is_end) {
    int pot[2] = {STAT_WIDTH, STAT_HEIGHT}; // potential XY
    int crd[2] = {0};                       // coordinates
    int sign = 1;                           // Adding/Subtracting
    int inY = 0;                            // which axis context
    int curr = size >> 1;                   // first bit to check
    point c;                                // final coordinate
    if (is_end) {
        offset = size - offset;
        crd[0] = STAT_WIDTH;
        crd[1] = STAT_HEIGHT;
        sign = -1;
    }
    while (curr) {
        pot[inY] >>= 1;
        if (curr & offset) {
            crd[inY] += pot[inY]*sign; 
        }
        inY = !inY; // flip which axis to look at
        curr >>= 1; // shift to the right to advance
    }
    c.x = crd[0];
    c.y = crd[1];
    return c;
}
