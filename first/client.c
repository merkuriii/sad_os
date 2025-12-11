#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int is_vowel(char c) {
    char vowels[] = "aeiouyAEIOUY";
    for (int i = 0; vowels[i]; i++) {
        if (c == vowels[i]) return 1;
    }
    return 0;
}

void remove_vowels(const char* input, ssize_t len, char* output, ssize_t* out_len) {
    ssize_t j = 0;
    for (ssize_t i = 0; i < len; i++) {
        if (!is_vowel(input[i])) {
            output[j++] = input[i];
        }
    }
    *out_len = j;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        const char msg[] = "error: expected filename argument\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    int file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        const char msg[] = "error: failed to open file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    char input_buf[4096];
    char output_buf[4096];
    ssize_t bytes, out_len;

    while ((bytes = read(STDIN_FILENO, input_buf, sizeof(input_buf))) > 0) {
        remove_vowels(input_buf, bytes, output_buf, &out_len);
        
        if (out_len > 0) {
            write(STDOUT_FILENO, output_buf, out_len);
            write(file, output_buf, out_len);
        }
    }

    close(file);
    return 0;
}