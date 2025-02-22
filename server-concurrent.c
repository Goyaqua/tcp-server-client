#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 8888
#define BACKLOG 5  // Max pending connections

// Global server socket descriptor
int socket_desc;

// Function to handle SIGINT (Ctrl+C)
void handle_sigint(int sig) {
    printf("\nShutting down server...\n");
    close(socket_desc);
    exit(0);
}

// Function to handle client communication
void handle_client(int client_socket) {
    char *message = "Hello Client, I have received your connection.\n";
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);  // Catch Ctrl+C
    signal(SIGCHLD, SIG_IGN);       // Ignore zombie processes

    struct sockaddr_in server, client;
    socklen_t client_len;
    char client_ip[INET_ADDRSTRLEN];

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Allow address reuse to prevent "Address already in use" error
    int opt = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(socket_desc);
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Bind failed");
        close(socket_desc);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(socket_desc, BACKLOG) == -1) {
        perror("Listen failed");
        close(socket_desc);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Main loop to accept multiple connections
    while (1) {
        client_len = sizeof(client);
        int new_socket = accept(socket_desc, (struct sockaddr *)&client, &client_len);
        if (new_socket == -1) {
            perror("Accept failed");
            continue;
        }

        // Convert client IP to readable format
        inet_ntop(AF_INET, &client.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("New connection from %s\n", client_ip);

        // Fork a new process for the client
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(new_socket);
        } else if (pid == 0) {  // Child process
            close(socket_desc); // Close the listening socket in the child
            handle_client(new_socket);
        } else { // Parent process
            close(new_socket); // Parent closes the client socket
        }
    }

    close(socket_desc);
    return 0;
}
