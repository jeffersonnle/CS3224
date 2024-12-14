#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define NUM_THREADS 4
#define NUM_TRIALS 100000 // Number of trials per thread
#define DAYS_IN_YEAR 365  // Days in a year (0-364)

// Shared variables
int nhits = 0;        // Total number of successes
pthread_mutex_t lock; // Mutex lock to protect shared variable

// Function to check if there are any matching birthdays
bool has_matching_birthday(int *birthdays, int n){
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (birthdays[i] == birthdays[j])
            {
                return true; // Found a match
            }
        }
    }
    return false;
}

// Worker thread function
void *WorkerThread(void *arg)
{
    int n = *((int *)arg); // Number of students in the class
    unsigned int rand_state = (unsigned int)time(NULL) + pthread_self();
    int local_hits = 0; // Local count of successes

    for (int i = 0; i < NUM_TRIALS; ++i)
    {
        int birthdays[n];

        // Generate random birthdays
        for (int j = 0; j < n; ++j)
        {
            birthdays[j] = rand_r(&rand_state) % DAYS_IN_YEAR;
        }

        // Check if there's a matching birthday
        if (has_matching_birthday(birthdays, n))
        {
            local_hits++;
        }
    }

    // Lock the mutex before updating the shared variable
    pthread_mutex_lock(&lock);
    nhits += local_hits;
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <class_size>\n", argv[0]);
        return 1;
    }

    int class_size = atoi(argv[1]); // Parse class size from command line
    if (class_size <= 0)
    {
        fprintf(stderr, "Class size must be a positive integer.\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];

    // Initialize the mutex
    pthread_mutex_init(&lock, NULL);

    // Create the worker threads
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_create(&threads[i], NULL, WorkerThread, &class_size);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    // Calculate and print the probability
    double probability = (double)nhits / (NUM_THREADS * NUM_TRIALS);
    printf("Estimated Probability for %d students: %.6f\n", class_size, probability);

    // Destroy the mutex
    pthread_mutex_destroy(&lock);

    return 0;
}
