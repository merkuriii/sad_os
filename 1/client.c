#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 4096
int is_vowel(char c) {
    const char *vowels = "aeiouyAEIOUY";
    return strchr(vowels, c) != NULL;
    }
int main(int argc, char **argv) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: child <output_file>\n", 27);
        exit(EXIT_FAILURE);
    }
    int file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (file == -1) {
        write(STDERR_FILENO, "open failed\n", 12);
        exit(EXIT_FAILURE);
    }
    char buf[BUF_SIZE];
    char out[BUF_SIZE];
    ssize_t bytes;

    while ((bytes = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        int j = 0;
        for (int i = 0; i < bytes; i++) {
            if (!is_vowel(buf[i])) {
                out[j++] = buf[i];
            }
        }

        write(file, out, j);
    }
    if (bytes < 0) {
        write(STDERR_FILENO, "read error\n", 11);
    }
    close(file);
    return 0;
}