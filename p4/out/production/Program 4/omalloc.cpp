/***********************************************
 * Author:     Malik Mohamedali
 * Assignment: Program 4
 * Class:       CSI 4337
 ************************************************/


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>

using namespace std;

typedef size_t BlockSize;

// Record for a free memory block
struct MemBlock {
  BlockSize size;  // Size (in bytes) of this block
  BlockSize reqDiff; // tracks difference in bytes requested and allocated (used in fragTrack var)
  MemBlock *next;  // Next free memory block
};

// Pointer to the list of free blocks, sorted by starting address
static MemBlock *free_list = nullptr;
static BlockSize allocatedBytes = 0;
static BlockSize allocatedBlocks = 0;
static BlockSize fragTrack = 0;

// Substitute for malloc
// cated 1 byte(s)..." << endl;
void *malloc(size_t size) throw() {
    size_t need = max(sizeof(MemBlock), size + sizeof(BlockSize)); // How much memory do we need?
    size_t sizeDiff = -1, currDiff;        // store current best fit size Difference
    MemBlock **bptr = nullptr;   // for modifying MemBlock list structure of free_list
    MemBlock **bestFit = nullptr; // store current address for best fitting MemBlock
    BlockSize *p = nullptr;      // address of start of MemBlock that gets returned (MemBlock - 1)
    /// Find the best fit block among those that are large enough
    for (bptr = &free_list; *bptr; bptr = &((*bptr)->next)) {
        if ((*bptr)->size >= need) {
            /// Store the difference in the sizes if it's better than our current best fit, and its location
            currDiff = (*bptr)->size - need;
            if (sizeDiff > currDiff or sizeDiff == -1) {
                sizeDiff = currDiff;
                bestFit = bptr;
            }
        }
	// cout << "iterating thorough free_list; current block size: " << (*bptr)->size << endl;
    }

    /// if a bestFit space was found, Remove block from free_list; bestFit is currently points to a block in free_list
    if (bestFit) {
        p = (BlockSize *) (*bestFit);
        (*bestFit)->reqDiff = sizeDiff;
        *bestFit = (*bestFit)->next;
        
	if (sizeDiff > 0)
	    fragTrack += sizeDiff;
	allocatedBlocks++;
        allocatedBytes += *p;

	return p + 1;
    }
    //cout << "no best fit -- new allocation occuring..." << endl;
    /// Ask for memory that's just as big as we need if best fit was not found
    p = (BlockSize *) sbrk(need); // FIXME sbrk() not in C++?

    /// Fill in the size field (for subsequent call to free)
    *p = need;

    /// Return a pointer to the new memory (memory for storage is directly after BlockSize[+1])
    allocatedBlocks++; // increment number of blocks we've allocated
    allocatedBytes += *p; // add size of alloc'd block to our current count
    ((MemBlock *) p)->reqDiff = 0;
    return p + 1;
}

// Substitute for free
void free(void *ptr) throw () {
    /// If ptr is null, do nothing
    if (ptr == nullptr) return;
    /// Get a pointer to the start of the node for our freelist
    MemBlock *block = (MemBlock *) (((BlockSize *)ptr) - 1);
    bool leftFree = false, rightFree = false; //
    MemBlock *lastNode = nullptr, *rpredecessorNode = nullptr, *lpredecessorNode = nullptr;

    ///Coalesce algorithm --> check if ptr address +- 1 corresponds to any address in freelist
    for (MemBlock **bptr = &free_list; *bptr; bptr = &((*bptr)->next)) {
        if (block - 1 == *bptr) {
            leftFree = true;
            lpredecessorNode = lastNode;
        }
        if (block + 1 == *bptr) {
            rightFree = true;
            rpredecessorNode = lastNode;
        }
        lastNode = *bptr;
    }
    ///If so, Join those blocks instead of moving the freed one to the front of the list
    if (leftFree and !rightFree) {
        //cout << "coalescing for left neighbor..." << endl;
        if (lpredecessorNode) {
            lpredecessorNode->next = (block - 1)->next;
            (block - 1)->next = free_list;
	    free_list = block - 1;
        }
        (block - 1)->size += block->size;
	//cout << "successful left join..." << endl;
    }
    /// if blocks right neighbor is in free list -- block's next is it's neighbor, neighbor's predecessor points @ block
    if (rightFree and rpredecessorNode and !leftFree) { // only works if the block being coalesced isn't the start of free_list
        //cout << "coalescing for right neighbor..." << endl;
        if (rpredecessorNode)
		rpredecessorNode->next = (block + 1)->next;
        block->next = free_list;
        free_list = block;
        block->size += (block + 1)->size;
        //cout << "successful right join..." << endl;
    }
    if (leftFree and rightFree) {
	//cout << "coalescing for both neighbors..." << endl;
	if (lpredecessorNode) {
        	lpredecessorNode->next = (block + 1)->next;
        	(block - 1)->next = free_list;
	}
        free_list = block - 1;
        (block - 1)->size += (block->size + (block + 1)->size);
	//cout << "successful dual join..." << endl;
    }
    /// move block to front of free_list if its neighbors aren't already free
    if (!leftFree and !rightFree) {
        block->next = free_list;
        free_list = block;
	//cout << "moved block to top of free_list!" << endl;
    }
    fragTrack -= block->reqDiff;
    allocatedBytes -= block->size;
    allocatedBlocks--;
}

// Substitute for calloc
void *calloc(size_t nmemb, size_t size) throw() {
    // Allocate a sufficiently big block
    void *ptr = malloc(nmemb*size);

    // Fill fresh block of memory with zeros and return a pointer to it
    bzero(ptr, size);
    return ptr;
}

// We have to supply a realloc since client code may also expect to use it
// This implementation doesn't offer any performance advantage over malloc/free
void *realloc(void *ptr, size_t size) throw() {
    // Handle NULL ptr
    if (ptr == nullptr) {
        return malloc(size);
    }

    // If size 0 and ptr is not null (already known), free memory
    if (size == 0) {
        free(ptr);
        return nullptr;
    }

    // Recover the old size (so we know what to copy)
    size_t old_size = ((int *)ptr)[-1];

    // Get a new buffer
    void *p2 = malloc(size);

    // Copy everything over
    memcpy(p2, ptr, old_size);

    // Free the old buffer and return the new one
    free(ptr);
    return p2;
}

void memReport() {
    MemBlock *bptr;
    int freeBlocks = 0;
    size_t bytes = 0;

    /// print allocated things (USES STATIC VARS IN STRUCT TO TRACK ALLOCATED BYTES)
    cout << "Allocated" << endl << "*********" << endl;
    cout << "Blocks: " << allocatedBlocks << endl;
    cout << "Used Bytes: " << allocatedBytes - fragTrack << endl;
    cout << "Unused bytes: " << fragTrack << endl;

    /// print free things
    cout << "Free" << endl << "****" << endl;
    if (free_list) {
    	for (bptr = free_list; bptr->next; bptr = bptr->next, freeBlocks++) {
            if (bptr->next) {
                cout << bptr->size << ",";
            }
            bytes += bptr->size;
        }
	if (bptr) {
            cout << bptr->size << endl;
	    freeBlocks++;
	    bytes += bptr->size;
	}
    }
    cout << "Blocks: " << freeBlocks << endl;
    cout << "Bytes: " << bytes << endl << endl;
}
