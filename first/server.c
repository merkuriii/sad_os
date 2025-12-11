#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static char CLIENT_PROGRAM_NAME[] = "client";

void get_program_path(char* buffer, size_t size) {
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len == -1) {
        const char msg[] = "error: failed to read program path\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    while (buffer[len] != '/') --len;
    buffer[len] = '\0';
}

pid_t create_child_process(int* pipe_read, int* pipe_write, const char* filename, const char* progpath) {
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        const char msg[] = "error: failed to create pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    pid_t child = fork();
    if (child == -1) {
        const char msg[] = "error: failed to fork\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);

        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", progpath, CLIENT_PROGRAM_NAME);
        
        char *args[] = {CLIENT_PROGRAM_NAME, (char*)filename, NULL};
        execv(path, args);
        
        const char msg[] = "error: failed to exec\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    close(pipe1[0]);
    close(pipe2[1]);

    *pipe_read = pipe2[0];
    *pipe_write = pipe1[1];
    return child;
}

int main() {
    srand(time(NULL));
    char progpath[1024];
    get_program_path(progpath, sizeof(progpath));

    char file1[256], file2[256];
    write(STDOUT_FILENO, "Enter filename for child1: ", 27);
    ssize_t len1 = read(STDIN_FILENO, file1, sizeof(file1) - 1);
    file1[len1 - 1] = '\0'; // убираем \n

    write(STDOUT_FILENO, "Enter filename for child2: ", 27);
    ssize_t len2 = read(STDIN_FILENO, file2, sizeof(file2) - 1);
    file2[len2 - 1] = '\0';

    int pipe1_read, pipe1_write;
    int pipe2_read, pipe2_write;
    
    pid_t child1 = create_child_process(&pipe1_read, &pipe1_write, file1, progpath);
    pid_t child2 = create_child_process(&pipe2_read, &pipe2_write, file2, progpath);

    char buffer[4096];
    ssize_t bytes;

    write(STDOUT_FILENO, "Enter lines (empty line to exit):\n", 35);

    while ((bytes = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        if (bytes == 1 && buffer[0] == '\n') {
           break; 
        }
        int target_pipe = (rand() % 100 < 80) ? pipe1_write : pipe2_write;
        
        if (write(target_pipe, buffer, bytes) != bytes) {
            const char msg[] = "error: failed to write to pipe\n";
            write(STDERR_FILENO, msg, sizeof(msg));
        }

        char result_buf[1024];
        ssize_t result_bytes;
        
        while ((result_bytes = read(pipe1_read, result_buf, sizeof(result_buf))) > 0) {
            write(STDOUT_FILENO, "Child1: ", 8);
            write(STDOUT_FILENO, result_buf, result_bytes);
        }
        
        while ((result_bytes = read(pipe2_read, result_buf, sizeof(result_buf))) > 0) {
            write(STDOUT_FILENO, "Child2: ", 8);
            write(STDOUT_FILENO, result_buf, result_bytes);
        }
    }

    close(pipe1_write);
    close(pipe2_write);
    close(pipe1_read);
    close(pipe2_read);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}