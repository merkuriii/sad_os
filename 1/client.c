#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int is_vowel(char c) {
    char vowels[] = "aeiouyAEIOUY";
    for (int i = 0; vowels[i] != '\0'; i++) {
        if (c == vowels[i]) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: client <filename>\n", 25);
        exit(1);
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        write(STDERR_FILENO, "File open error\n", 16);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    char processed[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        int j = 0;

        for (int i = 0; i < bytes_read; i++) {
            if (!is_vowel(buffer[i])) {
                processed[j++] = buffer[i];
            }
        }
        
        if (write(fd, processed, j) == -1) {
            write(STDERR_FILENO, "File write error\n", 17);
            break;
        }
        
        write(STDOUT_FILENO, "Processed: ", 11);
        write(STDOUT_FILENO, processed, j);
        write(STDOUT_FILENO, "\n", 1);
    }

    close(fd);
    return 0;
}