// Jefferson Le
// Intro to Operating Systems
// October 29, 2024
// Mansour
// Midterm 1 Programming Section

// Note; I couldn't test it in my VM because we weren't allowed to communicate,
// so I wasn't able to email it to myself to test in my VM. However,
// I tested on my computer and it worked, so I'm assuming it would be similarly
// successful on my VM. For my HWs I usually email to myself and test on VM,
// but here I couldn't. I hope that this is fine

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define NUM_TRIALS 1000000 // 1000000 trials run per thread
#define NUM_PROCS 4        // 4 threads
// shared variable to count successful trials
int nhits = 0;
sem_t semaphore; // semaphore for access to nhits
// check for duplicate birthdays
bool has_matching_birthday(int *birthdays, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (birthdays[i] == birthdays[j]) {
                return true;
            }
        }
    }
    return false;
}

// done by each thread
void *run_sim(void *arg){
    int n = *(int *)arg;
    int local_hits = 0;

    for (int i = 0; i < NUM_TRIALS; i++) {
        int *birthdays = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            birthdays[j] = rand() % 365; // random birthday
        }
        // dupes
        if (has_matching_birthday(birthdays, n)) {
            local_hits++;
        }
        free(birthdays); // free up memory
    }

    // nhits var
    sem_wait(&semaphore); // semaphore
    nhits += local_hits;  // shared count
    sem_post(&semaphore); // release

    return NULL;
}
int main(int argc, char *argv[]) {
    srand(time(NULL) + getpid());
    pthread_t threads[NUM_PROCS];
    int n;

    sem_init(&semaphore, 0, 1);

    int class_size = atoi(argv[1]); // Parse class size from command line
    // n = 23;
    // n = 49;
    printf("run simulation for n = %d students:\n", class_size);


    // create threads  
    for (int i = 0; i < NUM_PROCS; i++) {
        pthread_create(&threads[i], NULL, run_sim, &class_size);
    }

    // join threads 
    for (int i = 0; i < NUM_PROCS; i++) {
        pthread_join(threads[i], NULL);
    }

    double probability = (double)nhits / (NUM_PROCS * NUM_TRIALS);
    printf("probability for %d students: %.4f\n", class_size, probability);

    sem_destroy(&semaphore);
    return 0;
}
