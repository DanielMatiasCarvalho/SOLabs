#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *path_file = "/f1";

/*This test checks what happens when you try to open/create the same file
 * through two threads*/

/* This function opens a new file handle to f1, and returns 1 if it succedes, 0
 * if it doesn't*/
void *test_open(void *arg) {
    (void)arg;
    int f1;
    int *ret = (int *)malloc(sizeof(int));

    f1 = tfs_open(path_file, TFS_O_APPEND | TFS_O_CREAT);

    if (f1 != -1) {
        *ret = 1;
    } else {
        *ret = 0;
    }

    pthread_exit((void *)ret);
}

int main() {

    pthread_t th1, th2, th3;

    tfs_params params = tfs_default_params();
    params.max_open_files_count =
        2; // Limits the max open files to 2 to test if it only opens the file
           // in 2 of the 3 threads.

    assert(tfs_init(&params) != -1);

    int fd = tfs_open(path_file, TFS_O_CREAT);
    assert(fd != -1);
    assert(tfs_close(fd) != -1);

    int *result1, *result2, *result3;

    assert(pthread_create(&th1, NULL, test_open, NULL) ==
           0); // three threads to create/open the same file
    assert(pthread_create(&th2, NULL, test_open, NULL) == 0);
    assert(pthread_create(&th3, NULL, test_open, NULL) == 0);
    // ret = 0x003dae
    pthread_join(th1, (void **)&result1);
    pthread_join(th2, (void **)&result2);
    pthread_join(th3, (void **)&result3);

    assert((*result1 + *result2 + *result3) ==
           2); // Checks if the sum of the results from the 3 threads equals 2
               // (2 threads return 1, 1 thread returns 0)

    free(result1);
    free(result2);
    free(result3);

    printf("Successful test.\n");

    return 0;
}