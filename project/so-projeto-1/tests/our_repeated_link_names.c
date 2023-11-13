#include "fs/operations.h"
#include <assert.h>
#include <stdio.h>

char const file1[] = "/f1";
char const file2[] = "/f2";
char const hard_link1[] = "/hl1";
char const hard_link2[] = "/hl2";
char const sym_link1[] = "/sl1";

int main() {
    assert(tfs_init(NULL) != -1);

    int f;

    f = tfs_open(file1, TFS_O_CREAT);
    assert(f != -1);

    f = tfs_open(file2, TFS_O_CREAT);
    assert(f != -1);

    f = tfs_link(file1, file2); // Tests that you can't create a hard link with
                                // the same name as a file.
    assert(f == -1);

    f = tfs_sym_link(file1, file2); // Tests that you can't create a symbolic
                                    // link with the same name as a file.
    assert(f == -1);

    f = tfs_link(file1, hard_link1); // Creates a hard link to file 1.
    assert(f != -1);

    f = tfs_link(file2, hard_link1); // Tests that you can't create a hard link
                                     // that has the same name as another file.
    assert(f == -1);

    f = tfs_sym_link(file1, sym_link1); // Creates a symbolic link to file1.
    assert(f != -1);

    f = tfs_sym_link(file2,
                     sym_link1); // Tests that you can't create a symbolic link
                                 // that has the same name as another file.
    assert(f == -1);

    f = tfs_unlink(hard_link2); // Tests that you can't destroy a link/file that
                                // doesn't exist.
    assert(f == -1);

    // destroy TécnicoFS
    assert(tfs_destroy() != -1);

    printf("Successful test.\n");

    return 0;
}