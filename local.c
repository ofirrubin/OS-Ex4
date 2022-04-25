#include "stackShellLib.h"
#include "mlock.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	struct Stack *s = create_stack();
	pthread_mutex_t lock;
	init_memory();
	if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    	}
	char *cmd;
	int size;
	int write_size;
	char buffer[MAX_SIZE];
	int fb = 1; // feedback
	do
	{
		get_command(&cmd, &size);
		if (size > 0 && (cmd_sw(cmd, "push ") || cmd_eq(cmd, "pop") || cmd_eq(cmd, "top") || cmd_eq(cmd, "exit")))
		{
			fb = stack_command_handler(s, &lock, cmd, size, buffer, &write_size);
			buffer[write_size + 1] = 0; 
			if (fb && write_size > 0)
				printf("%s", buffer);
		}
		else
			printf("Invalid commannd, check syntax and try again\n");
		mem_free(cmd); // Free the command
	}
	while(fb);
	free_stack(s); // Free stack "object"
	return 0;
}
