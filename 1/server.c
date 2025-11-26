#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 1024

int main() {
    srand(time(NULL));
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;
    char file1[BUFFER_SIZE], file2[BUFFER_SIZE];
    ssize_t bytes_read;

    write(STDOUT_FILENO, "Enter filename for child1: ", 26);
    bytes_read = read(STDIN_FILENO, file1, BUFFER_SIZE);
    if (bytes_read > 0 && file1[bytes_read-1] == '\n') 
        file1[bytes_read-1] = '\0';

    write(STDOUT_FILENO, "Enter filename for child2: ", 26);
    bytes_read = read(STDIN_FILENO, file2, BUFFER_SIZE);
    if (bytes_read > 0 && file2[bytes_read-1] == '\n') 
        file2[bytes_read-1] = '\0';

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        write(STDERR_FILENO, "Pipe error\n", 11);
        exit(1);
    }

    if ((pid1 = fork()) == -1) {
        write(STDERR_FILENO, "Fork error\n", 11);
        exit(1);
    }

    if (pid1 == 0) {
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        execl("./client", "client", file1, NULL);
        write(STDERR_FILENO, "Execl error\n", 12);
        exit(1);
    }

    if ((pid2 = fork()) == -1) {
        write(STDERR_FILENO, "Fork error\n", 11);
        exit(1);
    }

    if (pid2 == 0) {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);
        execl("./client", "client", file2, NULL);
        write(STDERR_FILENO, "Execl error\n", 12);
        exit(1);
    }

    close(pipe1[0]);
    close(pipe2[0]);

    char buffer[BUFFER_SIZE];
    write(STDOUT_FILENO, "Enter strings (Ctrl+D to stop):\n", 32);
    
    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        int target = (rand() % 100 < 80) ? 1 : 2;
        int *pipe_target = (target == 1) ? pipe1 : pipe2;
        
        if (write(pipe_target[1], buffer, bytes_read) == -1) {
            write(STDERR_FILENO, "Write to pipe error\n", 20);
            break;
        }
    }

    close(pipe1[1]);
    close(pipe2[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    return 0;
}