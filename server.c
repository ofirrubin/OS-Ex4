#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h> // Override signal
#include <unistd.h> // Socket close
#include "TCPServer.h"
#include "Stack.h"
#include "stackShellLib.h"

#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_INPUT 1024
#define MAX_RESPOND 32

struct Stack *s;
pthread_mutex_t lock;

void *client_handler(void *args)
{
	printf("Client accepted\n");
	int sockfd = *(int *)args;
	// Input variables        
        char cmd[MAX_INPUT + 1];
        int size;

	// Output variables
	int write_size;
	char buffer[MAX_RESPOND]; // Output to user
	
	// If exit command
	int fb = 1; // feedback
	do
	{       // Reset input & output buffers
		memset(cmd, 0, MAX_INPUT); 
		memset(buffer, 0, MAX_RESPOND);
                receive(sockfd, (char **)&cmd, MAX_INPUT + 1, &size);
                if (size > 0)
                {
			fb = stack_command_handler(s, &lock, cmd, size, buffer, &write_size);
			buffer[write_size + 1] = 0;  // Make sure it's printable
			if (fb && write_size > 0)
			{
				printf("DEBUG: Client %d request: |%s| Respond: %s\n", sockfd, cmd, buffer);
				sock_send(buffer, sockfd);
			}
			else if (fb)
			{
				printf("DEBUG: Sending no respond [Client %d]\n", sockfd);
				sock_send("No respond\n", sockfd);
			}
		}
		else
			fb = 0;
	}
	while(fb);
	printf("DEBUG: Server closing connection to client %d\n", sockfd);
	close(sockfd);
	return 0;
}


int main(void)
{
    s = (struct Stack *)create_stack();
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    struct sigaction *sa = calloc(sizeof(struct sigaction), 1);
    char s[INET_ADDRSTRLEN];
    
    // Create and bind server socket
    int sockfd = create_server(sa, (char **)&s);
    if (sockfd <= 0 )
    {
    	printf("Error creating socket\n");
    }
    // Listen to the server
    server_listen(sockfd, BACKLOG);
    // Reap dead processes
    reap_dead_processes(sa);

    printf("server: waiting for connections...\n");
    handle_forever(sockfd, (char **)&s, client_handler);
    
    free(sa);
    return 0;
}
