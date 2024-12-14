#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SZ 5

// Shared memory structure
typedef struct {
    long long buffer[BUF_SZ];
    int write_index;
    int read_index;
    int count; // Keeps track of the number of elements in the buffer
} shared_mem_t;

void generate_fibonacci(int n, shared_mem_t *shared_mem) {
    long long fib1 = 0, fib2 = 1, next;

    for (int i = 0; i < n; i++) {
        // Wait until there is space in the buffer
        while (shared_mem->count == BUF_SZ) {
            usleep(100); // Briefly wait
        }

        if (i == 0) {
            next = fib1;
        } else if (i == 1) {
            next = fib2;
        } else {
            next = fib1 + fib2;
            fib1 = fib2;
            fib2 = next;
        }

        // Write to the buffer
        shared_mem->buffer[shared_mem->write_index] = next;
        shared_mem->write_index = (shared_mem->write_index + 1) % BUF_SZ;
        shared_mem->count++; // Increment buffer count

        // Simulate delay
        sleep(rand() % 3);
    }

    // No need for explicit termination, child will exit after generating all Fibonacci numbers
}

void print_fibonacci(int n, shared_mem_t *shared_mem) {
    for (int i = 0; i < n; i++) {
        // Wait until there is data in the buffer
        while (shared_mem->count == 0) {
            usleep(100); // Briefly wait
        }

        // Read from the buffer
        long long value = shared_mem->buffer[shared_mem->read_index];
        shared_mem->read_index = (shared_mem->read_index + 1) % BUF_SZ;
        shared_mem->count--; // Decrement buffer count

        printf("%lld\n", value);
    }

    // Parent process will finish naturally after printing all Fibonacci numbers
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

    // Create shared memory
    shared_mem_t *shared_mem = mmap(NULL, sizeof(shared_mem_t), PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Initialize shared memory variables
    shared_mem->write_index = 0;
    shared_mem->read_index = 0;
    shared_mem->count = 0; // Initially, the buffer is empty

    pid_t pid = fork();

    if (pid == 0) {
        // Child process: generate Fibonacci numbers
        generate_fibonacci(n, shared_mem);
    } else {
        // Parent process: print Fibonacci numbers
        print_fibonacci(n, shared_mem);
    }

    return 0;
}

