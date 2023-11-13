#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *path_file = "/f1";

void *test_open(void *arg) {
    (void)arg;
    int f1;
    int *ret = (int *)malloc(sizeof(int));
    f1 = tfs_open(path_file, TFS_O_APPEND | TFS_O_CREAT);
    assert(f1 != -1);
    *ret = f1;
    pthread_exit((void *)ret);
}

int main() {

    pthread_t th1, th2, th3;

    char *str_ext_file = "BBB!";
    char buffer[40];

    int *fd1 = (int *)malloc(sizeof(int));
    int *fd2 = (int *)malloc(sizeof(int));
    int *fd3 = (int *)malloc(sizeof(int));

    assert(tfs_init(NULL) != -1);

    assert(pthread_create(&th1, NULL, test_open, NULL) ==
           0); // two threads to create/open the same file
    assert(pthread_create(&th2, NULL, test_open, NULL) == 0);
    assert(pthread_create(&th3, NULL, test_open, NULL) == 0);

    pthread_join(th1, (void **)&fd1);
    pthread_join(th2, (void **)&fd2);
    pthread_join(th3, (void **)&fd3);

    assert(tfs_write(*fd1, str_ext_file, 5) ==
           5); // checking if both file handle point to the same inode

    assert(tfs_read(*fd2, buffer, 5) == 5);
    assert(strcmp(buffer, str_ext_file) == 0);

    assert(tfs_read(*fd3, buffer, 5) == 5);
    assert(strcmp(buffer, str_ext_file) == 0);

    assert(tfs_close(*fd1) != -1);
    assert(tfs_close(*fd2) != -1);
    assert(tfs_close(*fd3) != -1);

    free(fd1);
    free(fd2);
    free(fd3);

    printf("Successful test.\n");

    return 0;
}