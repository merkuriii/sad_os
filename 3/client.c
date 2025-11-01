#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

#define BUF_SIZE 4096

typedef struct {
    char buffer[BUF_SIZE];
    int terminate;
} shm_data;

int is_vowel(char c) {
    return strchr("aeiouyAEIOUY", c) != NULL;
}

int main(int argc, char **argv) {
    char *shm_name = argv[1];
    char *sem_empty_name = argv[2];
    char *sem_full_name  = argv[3];
    char *filename = argv[4];

    int shm_fd = shm_open(shm_name, O_RDWR, 0600);
    shm_data *data = mmap(NULL, sizeof(shm_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *sem_empty = sem_open(sem_empty_name, 0);
    sem_t *sem_full  = sem_open(sem_full_name,  0);

    int file = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    while (1) {
        sem_wait(sem_full);

        if (data->terminate)
            break;

        char out[BUF_SIZE];
        int j = 0;

        for (int i = 0; data->buffer[i]; i++)
            if (!is_vowel(data->buffer[i])) {
                out[j++] = data->buffer[i];
            }

        write(file, out, j);
        sem_post(sem_empty);
    }

    close(file);
    munmap(data, sizeof(shm_data));
    sem_close(sem_empty);
    sem_close(sem_full);

    return 0;
}
