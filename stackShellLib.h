#define MAX_SIZE 1032
#include "Stack.h"

int stack_command_handler(struct Stack *s, char *cmd, int size, char output[MAX_SIZE], int *write_size);
void get_command(char **cmd, int *cmd_len);
int cmd_eq(char *cmd, char *cmp);
