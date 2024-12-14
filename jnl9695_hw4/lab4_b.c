#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid1, pid2, pid3, pid4;
    // parent creating first child
    pid1 = fork();
    if (pid1 == 0) {
        printf("Child 1: PID = %d Parent = %d\n", getpid(), getppid());
        //child 1 creating child 3
        pid3 = fork();
        if (pid3 == 0) {
            printf("Child 3: PID = %d Parent = %d\n", getpid(), getppid());
        }
    } else if (pid1 > 0) { // because if pid is 0, you are inside the child process
        // parent creating second child
        pid2 = fork();
        if (pid2 == 0) {
            printf("Child 2: PID = %d Parent = %d\n", getpid(), getppid());
            // child 2 creating child 4
            pid4 = fork();
            if (pid4 == 0) {
                printf("Child 4: PID = %d Parent = %d\n", getpid(), getppid());
            }
        } else if (pid4 > 0) { // if pid == 0, we are in the child
            // now inside parent
            printf("Parent: PID = %d\n", getpid());
        }
    }
    sleep(1);

    return 0;
}