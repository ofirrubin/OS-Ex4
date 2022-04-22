/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "Stack.h"
#include "stackShellLib.h"

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 32 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sock_send(const char *respond, int size, int *sockfd)
{
        if (send(*sockfd, respond, size, 0) == -1)
		perror("send");
}

void receive(int sockfd, char **buffer, int size_p1, int *input_size)
{
	if ((*input_size = recv(sockfd, buffer, size_p1-1, 0)) == -1) {
	    perror("recv");
	    return;
	}
	buffer[*input_size] = '\0';
}

int main(int argc, char *argv[])
{
	int sockfd;  

	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure


        char buffer[MAXDATASIZE];
	
	char *cmd;
	int size, recv_size, loop = 1;
	do
	{
		printf("Enter command: "); // Get command from the client input
		get_command(&cmd, &size);
		sock_send(cmd, size, &sockfd); // Send the command over TCP
		if (cmd_eq(cmd, "exit")){
			loop = 0;
		}
		else
		{
			receive(sockfd, (char **)&buffer, MAXDATASIZE - 1, &recv_size); // Get respond
			if (recv_size > 0) // If any respond, print it.
			{
				printf("%s", buffer);
				memset(buffer, 0, MAXDATASIZE); // Reset the buffer
			}
		}
		free(cmd); // Free the command, we don't need it anymore
	}
	while(loop);
	
	close(sockfd);
	return 0;
}
