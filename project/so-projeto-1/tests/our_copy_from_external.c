#include "fs/operations.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* This test checks what happens when you try to copy a file that is bigger than
   1024 bytes. In our implementation of tfs, it should only copy the first 1024
   bytes that are read.*/
int main() {

    char *str_ext_file =
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! AAA! "
        "AAA! ";
    char *path_copied_file = "/f1";
    char *path_src = "tests/file_to_copy_over1024.txt";
    char buffer[1471];

    assert(tfs_init(NULL) != -1);

    // Creates the file.
    FILE *fd = fopen(path_src, "w");
    assert(fd != NULL);
    size_t i = fwrite(str_ext_file, sizeof(char), 1470, fd);
    assert(i != -1);
    fclose(fd);

    int f;
    ssize_t r;

    f = tfs_copy_from_external_fs(path_src, path_copied_file);
    assert(f != -1);

    f = tfs_open(path_copied_file, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_read(f, buffer,
                 sizeof(buffer) - 1); // Tries to read a maximum of 1470 bytes.
    assert(r == 1024);                // Asserts that it only read 1024 bytes.
    assert(!memcmp(buffer, str_ext_file,
                   (size_t)r)); // Compares the buffer content with the first
                                // 1024 bytes of the original string.

    printf("Successful test.\n");

    return 0;
}
