#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For string copy
// Lib required for sleep: 
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "mlock.h"


struct Stack
{
	char *value;
	int locked; // Is locked for changes
	struct Stack *next;	
};

void *calloc_func(size_t size)
{
	// return calloc(1, size);
	return mem_calloc(size);
}

void free_func(void *ptr)
{
	// free(ptr);
	return mem_free(ptr);
}


struct Stack *create_stack(void)
{
	struct Stack *s = NULL;
	s = calloc_func(sizeof(struct Stack));
	if (!s)
		return NULL;
	s->locked = 0;
	s->value = NULL;
	s->next = NULL;
	return s;
}	

// Essentialy - read command is free to use but write commands are not thread safe by nature, we protect thread-safe usage by wait&lock
int push(struct Stack *stack, char *val)
{
	if (!stack)
	{
		return 0;
	}
	// Push here
	if (! stack->value) // Empty stack
	{
		stack->value = val;
		stack->locked = 0; // Unlock stack
		return 1;
	}
	struct Stack *n = create_stack();
	if (!n) // If unable to allocate stack new element
	{
		stack->locked = 0; // Unlock stack
		return 0;
	}
	n->value = stack->value;
	n->next = stack->next;
	
	stack->next = n;
	stack->value = val;
	
	stack->locked = 0; // Free the resource
	return 1;
}

int push_copy(struct Stack *stack, char *val, int size)
{
	char *tmp = mem_calloc(size + 1);
	if (!tmp)
		return 0;
	strncpy(tmp, val, size);
	return push(stack, tmp);
}

int pop(struct Stack *stack) // Unlike traditional high-level language, here pop is not returning the value as it needs to free it from the memory.
{	
	if (!stack)
		return 0;

	// Pop here
	if (!stack->value) // If stack is empty
	{	
		stack->locked = 0; // Unlock stack
		return 0;
	}
	free_func(stack->value);
	stack->value = NULL;	
	if (stack->next)
	{
		struct Stack *ptr = stack->next;
		stack->value = ptr->value;
		stack->next = ptr->next;
		free_func(ptr); // We are removing the next struct duplicate
	}
	stack->locked = 0; // Free the resource
	return 1;
}

char *top(struct Stack *stack)
{
	return stack->value;
}

int is_empty(struct Stack *stack)
{
	return !(stack && stack->value);
}

void free_stack(struct Stack *stack)
{
	struct Stack *pointer;
	while(stack)
	{
		if (stack->value)
			free_func(stack->value);
		pointer = stack->next;
		free_func(stack);
		stack = pointer;
	}
}

