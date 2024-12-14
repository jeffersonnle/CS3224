#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void fibonacci(unsigned long long n) {
    unsigned long long a = 0, b = 1, next;
    printf("child: %llu %llu", a, b);
    for (unsigned long long i = 2; i < n; i++) {
        next = a + b;
        printf(" %llu", next);
        a = b;
        b = next;
    }
    printf("\n");
}
int main(int argc, char *argv[]) {
    if (argc != 2 || atoi(argv[1]) <= 2) {
        printf("Usage: %s <n> (n > 2)\n", argv[0]);
        exit(1);
    }
    unsigned long long n = atoll(argv[1]), a = 0, b = 1, next;
    pid_t pid = fork();
    if (pid == 0) {
        //child process
        //print first fibonacci number
        fibonacci(n);
    } else {
        wait(NULL);  //parent doesnt start until child finishes
        //parent process
        //print next TWO fibonacci numbers
        for (unsigned long long i = 2; i <= n; i++) {
            next = a + b;
            a = b;
            b = next;
        }
        printf("parent: %llu %llu\n", b, a + b);
    }

    return 0;
}