#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>

#define PORT 8080  // Define port number for the server
#define BACKLOG 3  // Number of connections allowed in the queue

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void generate_fibonacci(int n, int client_socket) {
    long long fib1 = 0, fib2 = 1, next;

    for (int i = 0; i < n; i++) {
        if (i == 0) {
            next = fib1;
        } else if (i == 1) {
            next = fib2;
        } else {
            next = fib1 + fib2;
            fib1 = fib2;
            fib2 = next;
        }

        // Write Fibonacci number to the socket, check if the write was successful
        if (write(client_socket, &next, sizeof(next)) <= 0) {
            perror("Write to socket failed");
            break;
        }

        sleep(rand() % 3);  // Simulate delay 
    }

    close(client_socket);  // Close the client socket
}

void print_fibonacci(int n, int client_socket) {
    long long value;

    for (int i = 0; i < n; i++) {
        // Read the Fibonacci number from the socket and check for errors
        if (read(client_socket, &value, sizeof(value)) <= 0) {
            perror("Read from socket failed");
            break;
        }
        printf("%lld\n", value);
    }

    close(client_socket);  // Close the client socket
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n < 2) {
        printf("n must be greater than 1.\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process: acts as the server (producer)
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) error("Server socket creation failed");

        // Enable address reuse to avoid "Address already in use" errors
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            error("setsockopt failed");
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));  // Clear memory
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);

        // Bind the socket to the specified port
        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            error("Bind failed");
        }

        // Listen for incoming connections
        if (listen(server_fd, BACKLOG) < 0) error("Listen failed");

        srand(time(NULL));
        sleep(rand() % 3);  // Random delay before accepting connections

        int client_socket = accept(server_fd, NULL, NULL);
        if (client_socket < 0) {
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        generate_fibonacci(n, client_socket);
        close(server_fd);  // Close the server socket
    } else {
        // Parent process: acts as the client (consumer)
        sleep(1);  // Give the server time to start

        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0) error("Client socket creation failed");

        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address));  // Clear memory
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Use loopback address

        // Attempt to connect to the server, with retries on failure
        int connect_status;
        do {
            connect_status = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
            if (connect_status < 0) usleep(100000);  // Wait 100 ms before retrying
        } while (connect_status < 0);

        print_fibonacci(n, client_socket);
    }

    return 0;
}