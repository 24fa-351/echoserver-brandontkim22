#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define LISTEN_BACKLOG 5

void* handleConnection(void* a_client_ptr)
{
    int a_client = *(int*)a_client_ptr;
    free(a_client_ptr);
    
    while (1) {
        char buffer[1024];
        int bytes_read = read(a_client, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            printf("Connection closed on socket_fd %d\n", a_client);
            break;
        }
        printf("Received from socket_fd %d: %s\n", a_client, buffer);
        write(a_client, buffer, bytes_read);
        bytes_read = read(a_client, buffer, sizeof(buffer));
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    char* p;
    long port = strtol(argv[1], &p, 10);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    int returnval;

    returnval = bind(
        socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));
    if (returnval == -1) {
        perror("bind");
        return 1;
    }

    returnval = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;

    while (1) {
        socklen_t client_address_len = sizeof(client_address);
        int client_fd = accept(
            socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        if (client_fd == -1) {
            perror("accept");
            return 1;
        }
        int* client_fd_ptr = (int*)malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        pthread_t thread;
        pthread_create(&thread, NULL, handleConnection, (void*)client_fd_ptr);
    }
   
    return 0;
}