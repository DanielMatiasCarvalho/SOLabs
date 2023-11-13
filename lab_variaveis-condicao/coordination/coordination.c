#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int g_value = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void *thr_func(void *ptr) {
    // first step: wait until the value is positive
    do {
        if (pthread_mutex_lock(&g_mutex) != 0) {
            exit(EXIT_FAILURE);
        }

        if (g_value > 0) {
            break;
        } else {
            if (pthread_mutex_unlock(&g_mutex) != 0) {
                exit(EXIT_FAILURE);
                //fprintf(stdout, "active wait: value=0, so I'll try again\n");
            }
        }
    } while (true);

    // second step: increment the value
    fprintf(stdout, "[thread %ld] read value=%d, will increment\n",
            pthread_self(), g_value);
    g_value++;

    if (pthread_mutex_unlock(&g_mutex) != 0) {
        exit(EXIT_FAILURE);
    }

    return NULL;
}

int main() {
    pthread_t tid;

    // spin new thread
    int error_num = pthread_create(&tid, NULL, thr_func, NULL);
    if (error_num != 0) {
        fprintf(stderr, "error creating thread: strerror(%s)\n", strerror(error_num));
        return -1;
    }

    // artificial delay
    sleep(5);

    // increment the value
    if (pthread_mutex_lock(&g_mutex) != 0) {
        return -1;
    }
    g_value++;
    if (pthread_mutex_unlock(&g_mutex) != 0) {
        return -1;
    }

    // wait for the other thread to finish
    if (pthread_join(tid, NULL) != 0) {
        return -1;
    }

    if (pthread_mutex_lock(&g_mutex) != 0) {
        return -1;
    }

    fprintf(stdout, "[main thread] value=%d\n", g_value);

    if (pthread_mutex_unlock(&g_mutex) != 0) {
        return -1;
    }

    return 0;
}
