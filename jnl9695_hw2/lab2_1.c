#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {  // make sure only three arguments are provided
        perror("Usage: my copy <input_file> <output_file>");
        return 1;
    }

    char *name = "Jefferson Le"; // name (OK to be hardcoded)
    ssize_t read_bytes; 
    char buffer[BUFFER_SIZE];
    uid_t user_id = getuid(); // user ID
    int input_fd = open(argv[1], O_RDONLY);
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int name_len = snprintf(buffer, sizeof(buffer), "%s\n%d\n", name, user_id);

    if (input_fd ==  -1) {
        // check if opening input file worked
        perror("Error opening input file.");
        return 1;
    }
    if (input_fd == -1)  {
        // check if creating output file worked (or opening it)
        perror("Error creating and opening output file.");
        close(input_fd);
        return 1;

    }
    // write hardcoded name and user ID into new file
    write(output_fd, buffer, name_len);

    
    while ((read_bytes = read(input_fd, buffer, sizeof(buffer))) > 0) {
        // copy lines from input to output files
        write(output_fd, buffer, read_bytes);
    }
    if (read_bytes == -1) {
        perror("Error reading input file");
    }

    // close both files
    close(input_fd);
    close(output_fd);

}