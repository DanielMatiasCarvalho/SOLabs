#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *path_file = "/f1";
char full_string[] = "ABABAB";
char sub_string[] = "AB";

int fd;

/* This function uses 3 threads to write the substring "AB" to the same file,
 * using the same file handle. The expected result is that the file will contain
 * 3 instances of the substring.*/

/* This function writes 2 bytes to the file */
void *test_write(void *arg) {
    (void)arg;

    assert(tfs_write(fd, sub_string, 2) == 2);

    pthread_exit(NULL);
}

int main() {

    pthread_t th1, th2, th3;

    char buffer[30];
    memset(buffer, 0, 30);

    assert(tfs_init(NULL) != -1);

    fd = tfs_open(path_file, TFS_O_TRUNC | TFS_O_CREAT);
    assert(fd != -1);

    assert(pthread_create(&th1, NULL, test_write, NULL) ==
           0); // Calls 3 threads, each writing 2 bytes
    assert(pthread_create(&th2, NULL, test_write, NULL) == 0);
    assert(pthread_create(&th3, NULL, test_write, NULL) == 0);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);

    assert(tfs_close(fd) != -1);

    fd = tfs_open(path_file, TFS_O_CREAT);
    assert(fd != -1);

    assert(tfs_read(fd, buffer, 6) == 6);

    assert(!strcmp(
        buffer,
        full_string)); // Compares what's on the file with the full string.

    printf("Successful test.\n");

    return 0;
}