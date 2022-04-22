#define MAX_SIZE 1032
#include "Stack.h"
#include <pthread.h>
int stack_command_handler(struct Stack *s, pthread_mutex_t *lock, char *cmd, int size, char output[MAX_SIZE], int *write_size);
void get_command(char **cmd, int *cmd_len);
int cmd_eq(char *cmd, char *cmp);
int cmd_sw(char *cmd, char *cmp);
