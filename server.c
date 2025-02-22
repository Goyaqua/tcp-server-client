#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[1024] = {0};
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello, world!";

    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Options to reuse the address and port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Setting server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // Binding the socket to the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listening for client connections
    if (listen(server_fd, 3) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Accepting incoming connections
    puts("Waiting for incoming connections...");
    client_addr_len = sizeof(client_addr);
    client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    puts("Connection accepted");

    // Receiving data from the client
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer) - 1);
    printf("Message from client: %s\n", buffer);

    // Sending response to the client
    if (send(client_socket, response, strlen(response), 0) == -1) {
        perror("Failed to send response");
    } else {
        printf("Response sent to client\n");
    }

    // Closing the connection
    close(client_socket);
    close(server_fd);
    return 0;
}
