#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Thread safe malloc, free, calloc implementation.
// Based on: https://stackoverflow.com/a/12773678

typedef struct memory {
	size_t size;
	struct memory *next;
} memory;

static pthread_mutex_t mem_lock;

static memory memory_head = { 0, 0 };

static const size_t align_to = 16;

void* mem_alloc(size_t size) {
    size = (size + sizeof(memory) + (align_to - 1)) & ~ (align_to - 1);
    memory* block = memory_head.next;
    memory** head = &(memory_head.next);
    while (block != 0) {
        if (block->size >= size) {
            *head = block->next;
            return ((char*)block) + sizeof(memory);
        }
        head = &(block->next);
        block = block->next;
    }

    block = (memory*)sbrk(size);
    block->size = size;

    return ((char*)block) + sizeof(memory);
}



void *mem_calloc(size_t size)
{
	void *ptr = mem_alloc(size);
	memset(ptr, 0, size);
	return ptr;
}


void mem_free(void *ptr)
{
	if (ptr == NULL)
		return;
        pthread_mutex_lock(&mem_lock);

	memory* block = (memory*)(((char*)ptr) - sizeof(memory));
	block->next = memory_head.next;
	memory_head.next = block;

        pthread_mutex_unlock(&mem_lock);
}

void init_memory()
{
	if (pthread_mutex_init(&mem_lock, NULL) != 0) {
		perror("\n *memory mutex init has failed*\n");
	}	
}

