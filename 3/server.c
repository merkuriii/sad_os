#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 4096

typedef struct {
    char buffer[BUF_SIZE];
    int terminate;
} shm_data;

void create_ipc(const char *shm_name, const char *sem_empty_name, const char *sem_full_name, shm_data **data, sem_t **sem_empty, sem_t **sem_full) {
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600);
    ftruncate(shm_fd, sizeof(shm_data));

    *data = mmap(NULL, sizeof(shm_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    (*data)->terminate = 0;

    *sem_empty = sem_open(sem_empty_name, O_CREAT, 0600, 1);
    *sem_full  = sem_open(sem_full_name,  O_CREAT, 0600, 0);
}

int main() {
    srand(time(NULL));
    pid_t pid = getpid();

    char shm1[64], shm2[64];
    char sem1_empty[64], sem1_full[64];
    char sem2_empty[64], sem2_full[64];

    snprintf(shm1, sizeof(shm1), "/shm_%d_1", pid);
    snprintf(shm2, sizeof(shm2), "/shm_%d_2", pid);

    snprintf(sem1_empty, sizeof(sem1_empty), "/sem_%d_1_e", pid);
    snprintf(sem1_full,  sizeof(sem1_full),  "/sem_%d_1_f", pid);

    snprintf(sem2_empty, sizeof(sem2_empty), "/sem_%d_2_e", pid);
    snprintf(sem2_full,  sizeof(sem2_full),  "/sem_%d_2_f", pid);

    shm_data *d1, *d2;
    sem_t *e1, *f1, *e2, *f2;

    create_ipc(shm1, sem1_empty, sem1_full, &d1, &e1, &f1);
    create_ipc(shm2, sem2_empty, sem2_full, &d2, &e2, &f2);

    if (fork() == 0) {
        execl("./client", "client", shm1, sem1_empty, sem1_full, "out1.txt", NULL);
        exit(1);
    }

    if (fork() == 0) {
        execl("./client", "client", shm2, sem2_empty, sem2_full, "out2.txt", NULL);
        exit(1);
    }

    char buf[BUF_SIZE];

    while (fgets(buf, sizeof(buf), stdin)) {
        int r = rand() % 100;

        shm_data *d   = (r < 80) ? d1 : d2;
        sem_t   *emp = (r < 80) ? e1 : e2;
        sem_t   *ful = (r < 80) ? f1 : f2;

        sem_wait(emp);
        strcpy(d->buffer, buf);
        sem_post(ful);
    }

    sem_wait(e1);
    d1->terminate = 1;
    sem_post(f1);

    sem_wait(e2);
    d2->terminate = 1;
    sem_post(f2);

    sleep(1);

    shm_unlink(shm1);
    shm_unlink(shm2);

    sem_unlink(sem1_empty);
    sem_unlink(sem1_full);
    sem_unlink(sem2_empty);
    sem_unlink(sem2_full);

    return 0;
}
