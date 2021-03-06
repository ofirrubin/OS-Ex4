#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "Stack.h"
#include "stackShellLib.h"
#include "mlock.h"
#include <string.h>


#ifndef MAX_SIZE
#define MAX_SIZE 1032
#endif

// Recycled from Shell assignment, minor changes (such as length)
void get_command(char **cmd, int *cmd_len)
{
	char *c = mem_calloc(MAX_SIZE);
	if (!c){
		printf("ERROR: Error allocating memory for command input\n");
		exit(0);
	}
	// Get input from the user
	if (!fgets(c, MAX_SIZE, stdin))
	{
		mem_free(c);
	}
	*cmd_len = strlen(c);
	if (*cmd_len > 0)
	{ // Remove trailling (new line etc)
		while ((char *)cmd_len > (char *)c && c[*cmd_len] == 0)
			*cmd_len = *cmd_len - 1;
	}
	*(c+*cmd_len) = 0;
	*cmd = c;
}


// START Recycling | Recycled from Shell assignment
int cmd_cmp(char *actual, char *expected, int starts_with){ // Assuming <expected> is lowered case.
	int pos = 0;
	int d = 'A' - 'a';
	while (actual[pos] != 0){
		if (expected[pos] == 0 || !(actual[pos] == expected[pos] || expected[pos] + d == actual[pos]))
			return starts_with && expected[pos] == 0;
		pos += 1;
	}
	return expected[pos] == 0 && actual[pos] == 0;
}

int cmd_eq(char *cmd, char *cmp){ // Command equals to
	return cmd_cmp(cmd, cmp, 0);
}

int cmd_sw(char *cmd, char *cmp){ // Command Starts with
	return cmd_cmp(cmd, cmp, 1);
}
// END Recycling | End of recycling from Shell assignment

int stack_command_handler(struct Stack *s, pthread_mutex_t *lock, char *cmd, int size, char output[MAX_SIZE], int *write_size)
{
	if (cmd_eq(cmd, "exit"))
	{
		*write_size = 0;
		return 0;
	}
	pthread_mutex_lock(lock);
	if (cmd_sw(cmd, "push"))
	{
		int push_size = size - sizeof("push");
		if (push_size > 0)
		{
			if(push_copy(s, cmd + sizeof("push"), size - sizeof("push")))
				*write_size = sprintf(output, "OUTPUT: Pushed\n");
			else
				*write_size = sprintf(output, "OUTPUT: Push failed\n");
		}
		else
		{
			printf("DEBUG: Push size is lower than 0\n");
			*write_size = sprintf(output, "OUTPUT: Can't be empty\n");
		}
	}
	else if (cmd_eq(cmd, "pop"))
	{
		if (pop(s))
			*write_size = sprintf(output, "OUTPUT: Popped\n");
		else
			*write_size = sprintf(output, "OUTPUT: Failed - Empty\n");
	
	}
	else if (cmd_eq(cmd, "top"))
	{
		if (!is_empty(s)) // To prevent trying to print NULL
			*write_size = sprintf(output, "OUTPUT: %s\n", top(s));
		else
			*write_size = sprintf(output, "OUTPUT: Stack is empty\n");
	}
	pthread_mutex_unlock(lock);
	return 1;
}

