#include "stackShellLib.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	struct Stack *s = create_stack();
	char *cmd;
	int size;
	int write_size;
	char buffer[MAX_SIZE];
	int fb = 1; // feedback
	do
	{
		get_command(&cmd, &size);
		fb = stack_command_handler(s, cmd, size, buffer, &write_size);
		free(cmd); // Free the command
		buffer[write_size + 1] = 0; 
		if (fb && write_size > 0)
			printf("%s", buffer);
	}
	while(fb);
	free_stack(s); // Free stack "object"
	return 0;
}
