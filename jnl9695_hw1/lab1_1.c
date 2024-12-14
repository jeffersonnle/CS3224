#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    printf("Hello World! This is CS3224, Fall 2024! \n");
    int randNum = (rand()% 200);
    printf("Jefferson Le ");
    printf("%d", randNum);
}