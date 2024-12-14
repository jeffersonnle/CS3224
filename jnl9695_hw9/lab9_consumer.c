#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

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

void print_fibonacci(int n, shared_mem_t *shared_mem) {
    for (int i = 0; i < n; i++) {
        while (shared_mem->count == 0) {
            usleep(100); // Wait if buffer is empty
        }

        // Read from shared buffer
        long long value = shared_mem->buffer[shared_mem->read_index];
        shared_mem->read_index = (shared_mem->read_index + 1) % BUF_SZ;
        shared_mem->count--;

        // Get timestamp
        clock_t current_time = clock();
        printf("(%lf) consumed [%d]: %lld\n",
               (double)current_time / CLOCKS_PER_SEC, i, value);
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

    // Open shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    shared_mem_t *shared_mem = mmap(0, sizeof(shared_mem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Print addresses
    printf("Consumer: Address of n: %p\n", (void*)&n);
    printf("Consumer: Shared buffer start address: %p\n", (void*)shared_mem);

    // Print Fibonacci numbers
    print_fibonacci(n, shared_mem);

    // Cleanup
    munmap(shared_mem, sizeof(shared_mem_t));
    close(shm_fd);
    shm_unlink(SHM_NAME); // Remove shared memory
    return 0;
}
