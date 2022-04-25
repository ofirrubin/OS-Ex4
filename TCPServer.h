int INET_ADDRSTRLEN;

int create_server(struct sigaction *sa, char *s[INET_ADDRSTRLEN]);

void reap_dead_processes(struct sigaction *sa);
 
void server_listen(int sockfd, int backlog);

void handle_forever(int sockfd, char *s[INET_ADDRSTRLEN], void *(* f)(void *));

int sock_send(const char *respond, int size, int sockfd);

int receive(int sockfd, char **buffer, int size_p1, int *input_size);
