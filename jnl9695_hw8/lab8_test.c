#include <unistd.h> 
#include <fcntl.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

int main(int argc, char* argv[]) { 
    if (argc != 2) { 
        printf("Usage: %s <BUFFER_SIZE>\n", argv[0]); 
        return 1; 
    } 
    char buffer[100] = {0}; 
    int bytes_read = 0; 
    int length = atoi(argv[1]); 
    int fd = open("/dev/lab8", O_RDONLY); 
    int i = 0; 
    do { 
        bytes_read = read(fd, buffer + i, length); 
        i += bytes_read; 
        printf("Bytes Read: %i, Buffer: %s\n", bytes_read, buffer); 
    } while (bytes_read > 0); 
    printf("Expected: %i, Read: %s\n", getuid(), buffer); 
}