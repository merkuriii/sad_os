#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define ARRAY_SIZE 1000000

typedef struct {
    int *arr;
    int left;
    int right;
} ThreadArgs;

static sem_t thread_limiter;

static int partition(int *arr, int left, int right) {
    int pivot = arr[(left + right) / 2];
    int i = left;
    int j = right;

    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;

        if (i <= j) {
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }
    return i;
}

static void quicksort_seq(int *arr, int left, int right) {
    if (left >= right) return;

    int index = partition(arr, left, right);
    quicksort_seq(arr, left, index - 1);
    quicksort_seq(arr, index, right);
}

static void *quicksort_par_wrapper(void *arg);

static void quicksort_par(int *arr, int left, int right) {
    if (left >= right) return;

    int index = partition(arr, left, right);

    if (sem_trywait(&thread_limiter) == 0) {
        pthread_t thread;
        ThreadArgs *args = malloc(sizeof(ThreadArgs));

        args->arr = arr;
        args->left = left;
        args->right = index - 1;

        if (pthread_create(&thread, NULL, quicksort_par_wrapper, args) == 0) {
            quicksort_par(arr, index, right);
            pthread_join(thread, NULL);
        } else {
            sem_post(&thread_limiter);
            free(args);
            quicksort_seq(arr, left, right);
        }
    } else {
        quicksort_seq(arr, left, right);
    }
}

static void *quicksort_par_wrapper(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    quicksort_par(args->arr, args->left, args->right);
    free(args);
    sem_post(&thread_limiter);
    return NULL;
}

static void parallel_quicksort(int *arr, int n, int max_threads) {
    if (max_threads <= 1) {
        quicksort_seq(arr, 0, n - 1);
        return;
    }

    sem_init(&thread_limiter, 0, max_threads - 1);
    quicksort_par(arr, 0, n - 1);
    sem_destroy(&thread_limiter);
}

static bool is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++)
        if (arr[i] < arr[i - 1]) return false;
    return true;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int max_threads = atoi(argv[1]);
    if (max_threads < 1) max_threads = 1;

    int *arr_par = malloc(sizeof(int) * ARRAY_SIZE);
    int *arr_seq = malloc(sizeof(int) * ARRAY_SIZE);

    if (!arr_par || !arr_seq) {
        perror("malloc");
        return 1;
    }

    srand((unsigned)time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        arr_par[i] = rand();
        arr_seq[i] = arr_par[i];
    }

    struct timespec start, end;
    // параллельн
    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_quicksort(arr_par, ARRAY_SIZE, max_threads);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double par_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    // последовательн
    clock_gettime(CLOCK_MONOTONIC, &start);
    quicksort_seq(arr_seq, 0, ARRAY_SIZE - 1);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double seq_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Threads: %d\n", max_threads);
    printf("Parallel time:   %.3f ms\n", par_time * 1000);
    printf("Sequential time: %.3f ms\n", seq_time * 1000);
    printf("Speedup:         %.2fx\n", seq_time / par_time);
    printf("Efficiency:      %.3f\n", (seq_time / par_time) / max_threads);
    printf("Correct:         %s\n", is_sorted(arr_par, ARRAY_SIZE) ? "yes" : "no");

    free(arr_par);
    free(arr_seq);
    return 0;
}
