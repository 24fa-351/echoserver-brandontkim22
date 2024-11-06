#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTEN_BACKLOG 5

void handleConnection(int a_client)
{
    char buffer[1024];
    
    while (1) {
        int bytes_read = read(a_client, buffer, sizeof(buffer));
        if (bytes_read <= 0) {
            break;
        }
        printf("Received %s\n", buffer);
        write(a_client, buffer, bytes_read);
        bytes_read = read(a_client, buffer, sizeof(buffer));
    }
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

    returnval = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;

    while (1) {
        socklen_t client_address_len = sizeof(client_address);
        int client_fd = accept(
            socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        handleConnection(client_fd);
        close(client_fd);
    }
   
    return 0;
}