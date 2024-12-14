#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void generate_fibonacci(int n, int pipe_fd[2]) {
    long long fib1 = 0, fib2 = 1, next;

    // Close the read end of the pipe in the child process
    close(pipe_fd[0]);

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

        // Write the Fibonacci number to the pipe
        write(pipe_fd[1], &next, sizeof(long long));

        // Simulate delay
        sleep(rand() % 3);
    }

    // Close the write end of the pipe when done
    close(pipe_fd[1]);

    // Child process will exit naturally
}

void print_fibonacci(int n, int pipe_fd[2]) {
    long long value;

    // Close the write end of the pipe in the parent process
    close(pipe_fd[1]);

    for (int i = 0; i < n; i++) {
        // Read the Fibonacci number from the pipe
        read(pipe_fd[0], &value, sizeof(long long));

        // Print the Fibonacci number
        printf("%lld\n", value);
    }

    // Close the read end of the pipe after done reading
    close(pipe_fd[0]);

    // Parent process will exit naturally
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n < 2) {
        printf("n must be greater than 1.\n");
        exit(1);
    }

    // Create a pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child process: generate Fibonacci numbers
        generate_fibonacci(n, pipe_fd);
    } else {
        // Parent process: print Fibonacci numbers
        print_fibonacci(n, pipe_fd);
    }

    return 0;
}