#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

#define SHM_NAME "/lab9_shm"
#define BUF_SZ 5

typedef struct {
    long long buffer[BUF_SZ];
    int write_index;
    int read_index;
    int count;
} shared_mem_t;

// Global variable in the data section
int n = 10; // Default value

void generate_fibonacci(int n, shared_mem_t *shared_mem) {
    long long fib1 = 0, fib2 = 1, next;

    for (int i = 0; i < n; i++) {
        while (shared_mem->count == BUF_SZ) {
            usleep(100); // Wait if buffer is full
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

        // Add to shared buffer
        shared_mem->buffer[shared_mem->write_index] = next;
        shared_mem->write_index = (shared_mem->write_index + 1) % BUF_SZ;
        shared_mem->count++;

        // Get timestamp
        clock_t current_time = clock();
        printf("(%lf) produced [%d]: %lld\n",
               (double)current_time / CLOCKS_PER_SEC, i, next);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        n = atoi(argv[1]);
    }
    if (n < 2) {
        printf("n must be greater than 1.\n");
        exit(1);
    }

    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(shared_mem_t));
    shared_mem_t *shared_mem = mmap(0, sizeof(shared_mem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Initialize shared memory
    memset(shared_mem, 0, sizeof(shared_mem_t));

    // Print addresses
    printf("Producer: Address of n: %p\n", (void*)&n);
    printf("Producer: Shared buffer start address: %p\n", (void*)shared_mem);

    // Generate Fibonacci numbers
    generate_fibonacci(n, shared_mem);

    // Cleanup
    munmap(shared_mem, sizeof(shared_mem_t));
    close(shm_fd);
    return 0;
}
