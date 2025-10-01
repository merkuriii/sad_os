#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 4096

int main(void) {
    int pipe1[2], pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        write(STDERR_FILENO, "pipe failed\n", 12);
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));

    pid_t child1 = fork();

    if (child1 == -1) {
        write(STDERR_FILENO, "fork failed\n", 12);
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) {
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);

        close(pipe2[0]);
        close(pipe2[1]);

        execl("./client", "child", "out1.txt", NULL);
        write(STDERR_FILENO, "exec failed\n", 12);
        exit(EXIT_FAILURE);
    }

    pid_t child2 = fork();

    if (child2 == -1) {
        write(STDERR_FILENO, "fork failed\n", 12);
        exit(EXIT_FAILURE);
    }

    if (child2 == 0) {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);

        close(pipe1[0]);
        close(pipe1[1]);

        execl("./client", "child", "out2.txt", NULL);
        write(STDERR_FILENO, "exec failed\n", 12);
        exit(EXIT_FAILURE);
    }
    close(pipe1[0]);
    close(pipe2[0]);
    char buf[BUF_SIZE];
    ssize_t bytes;

    while ((bytes = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        int r = rand() % 100;

        if (r < 80) {
            write(pipe1[1], buf, bytes);
        } else {
            write(pipe2[1], buf, bytes);
        }
    }
    if (bytes < 0) {
        write(STDERR_FILENO, "read error\n", 11);
    }
    close(pipe1[1]);
    close(pipe2[1]);
    wait(NULL);
    wait(NULL);
    return 0;
}
