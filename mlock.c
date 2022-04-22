#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Thread safe malloc, free, calloc implementation.
// Based on: https://medium.com/@andrestc/implementing-malloc-and-free-ba7e7704a473

typedef struct memory {
	size_t size;
	struct memory *next;
	struct memory *prev;
} memory;

#ifndef ALLOC_UNIT
#define ALLOC_UNIT sizeof(size_t) + 2 * sizeof(memory)
#endif

static memory *__memory_head = NULL;
static pthread_mutex_t mem_lock;

void memory_link(memory * block) // Link memory block to __memory_head list
{
	block->prev = NULL; // Reset old relations
	block->next = NULL;
	if (!__memory_head || (unsigned long)block < (unsigned long)__memory_head)
	{ // Make block __memory_head if no __memory_head, swap block and __memory_head if block is before __memory_head.
		if (__memory_head) // Making block as __memory_head essentially, set is as previous
			__memory_head->prev = block;
		block->next = __memory_head; // Set the opposite side (make __memory_head its next)
		__memory_head = block; // Swap __memory_head with block
	} 
	else
	{ // Find block position and set it as next
		memory *curr = __memory_head;
		while (curr->next && (unsigned long)curr->next < (unsigned long)block) 
			curr = curr->next;
		block->next = curr->next;
		curr->next = block;
	}
}

void memory_unlink(memory * block) // Unlink memory block from the __memory_head list
{
	if (block->prev) // If it previous, we just skipping this block
		block->prev->next = block->next;
	else // Otherwise it's actually __memory_head, then we skip this block if has next.
		__memory_head = block->next ? block->next: NULL;
	
	if (block->next) // If it has next we also need to update the next previous:
		block->next->prev = block->prev;
}

// Create block after <size>, return new block
memory * split_block(memory * block, size_t size)
{
	void *mem_block = (void *)((unsigned long)block + sizeof(memory));
	memory *newptr = (memory *) ((unsigned long)mem_block + size);
	newptr->size = block->size - (size + sizeof(memory));
	block->size = size;
	return newptr;
}

void *mem_alloc(size_t size)
{
        pthread_mutex_lock(&mem_lock);
	void *block_mem;
	memory *ptr, *newptr;
	size_t alloc_size = size >= ALLOC_UNIT ? (size + sizeof(memory) + 1) : ALLOC_UNIT; // We got minimum size thus setting it.
	ptr = __memory_head;
	if (alloc_size < 1024)
	{
		while (ptr) {
			if (ptr->size >= size + sizeof(memory)) {
				block_mem = ((void *)(unsigned long)ptr + sizeof(memory));
				memory_unlink(ptr);
				if (ptr->size == size)  // We found the same block size as requested
				{
					pthread_mutex_unlock(&mem_lock);
					return block_mem;
				}
				newptr = split_block(ptr, size); // We got a block in size bigger than requested, we split it to two blocks.
				memory_link(newptr);
				pthread_mutex_unlock(&mem_lock);
				return block_mem;
			} 
			else 
				ptr = ptr->next;
		}
	}
	ptr = sbrk(size + sizeof(struct memory));	// No free block found, request from the system.
	if (!ptr) { // Error if we can't retreive memory
		perror("failed to alloc\n");
	        pthread_mutex_unlock(&mem_lock);
		return NULL;
	}
        pthread_mutex_unlock(&mem_lock);
	return (void *)((unsigned long)ptr + sizeof(memory));
}

void *mem_calloc(size_t size)
{
	void *ptr = mem_alloc(size);
	memset(ptr, 0, size);
	return ptr;
}


void mem_free(void *ptr)
{
        pthread_mutex_lock(&mem_lock);
	memory_unlink((void *)((unsigned long)ptr - sizeof(memory)));

        pthread_mutex_unlock(&mem_lock);
}

void init_memory()
{
	if (pthread_mutex_init(&mem_lock, NULL) != 0) {
		perror("\n mutex init has failed\n");
	}	
}

