/*
 * thread.c - simple example demonstrating the creation of threads
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

/* global value */
int g_value = 0;

void* thr_func() {
    g_value = 1;
    return NULL;
}

int main() {
    int pid = fork();
    int status;
    if (pid==-1) {
        fprintf(stderr, "error creating process.\n");
        return -1;
    }
    g_value = 2;
    if (pid==0) {
        thr_func();
    } else {
        wait(&status);
        printf("value = %d\n", g_value);
    }
    
    return 0;
}
