#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *path_file = "/f1";
char full_string[] = "ABCDEF";
char sub_strings[3][3] = {"AB", "CD", "EF"};

int fd;

/*This test checks what happens when you try to read in different threads, from
 * the same file handle, the test writes a 6 char string in a file, then reads 2
 * chars in each thread and confirms that all three pieces of the string are
 * present.*/

/* This function creates a buffer, reads 2 bytes from the file and returns the
 * buffer. */
void *test_read(void *arg) {
    (void)arg;
    char *buffer = (char *)malloc(3 * sizeof(char));
    memset(buffer, 0, 3);

    assert(tfs_read(fd, buffer, 2) == 2);

    pthread_exit((void *)buffer);
}

int main() {

    pthread_t th1, th2, th3;

    char *res1;
    char *res2;
    char *res3;

    assert(tfs_init(NULL) != -1);

    fd = tfs_open(path_file, TFS_O_TRUNC | TFS_O_CREAT);
    assert(fd != -1);
    assert(tfs_write(fd, full_string, 7) == 7);
    assert(tfs_close(fd) != -1);

    fd = tfs_open(path_file, TFS_O_CREAT);
    assert(pthread_create(&th1, NULL, test_read, NULL) ==
           0); // Calls 3 threads, each reading 2 bytes
    assert(pthread_create(&th2, NULL, test_read, NULL) == 0);
    assert(pthread_create(&th3, NULL, test_read, NULL) == 0);

    pthread_join(th1, (void **)&res1);
    pthread_join(th2, (void **)&res2);
    pthread_join(th3, (void **)&res3);

    assert(!strcmp(res1, sub_strings[0]) || !strcmp(res2, sub_strings[0]) ||
           !strcmp(res3,
                   sub_strings[0])); // Checks if any of the 3 results if "AB"
    assert(!strcmp(res1, sub_strings[1]) || !strcmp(res2, sub_strings[1]) ||
           !strcmp(res3,
                   sub_strings[1])); // Checks if any of the 3 results if "CD"
    assert(!strcmp(res1, sub_strings[2]) || !strcmp(res2, sub_strings[2]) ||
           !strcmp(res3,
                   sub_strings[2])); // Checks if any of the 3 results if "EF"

    // By doing these 3 asserts, we can confirm that all three substrings are
    // present in the results, meaning that the threads didn't read the same
    // part of the file.

    free(res1);
    free(res2);
    free(res3);

    printf("Successful test.\n");

    return 0;
}