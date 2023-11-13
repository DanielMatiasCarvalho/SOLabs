#include "fs/operations.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

char const file1[] = "/f1";
char const file2[] = "/f2";
char const sym_link1[] = "/sl1";
char const text[] = "test";

int main() {
    char buffer[100];

    memset(buffer, 0, 100);

    assert(tfs_init(NULL) != -1);

    int f;

    f = tfs_open(file1, TFS_O_TRUNC | TFS_O_CREAT);
    assert(f != -1);

    assert(tfs_unlink(file1) == -1); // Cant unlink a file that is open.

    assert(tfs_close(f) != -1);

    assert(tfs_unlink(file1) != -1); // After we close it, now we can unlink.

    f = tfs_open(file2, TFS_O_TRUNC | TFS_O_CREAT);

    assert(f != -1);

    assert(tfs_sym_link(file2, sym_link1) !=
           -1); // Creates a symbolic link to the file

    assert(tfs_close(f) != -1);

    f = tfs_open(sym_link1,
                 TFS_O_TRUNC |
                     TFS_O_CREAT); // Opens the file via the symbolic link.

    assert(f != -1);

    assert(tfs_unlink(sym_link1) !=
           -1); // Tries to unlink the symbolic link and succedes, since whats
                // open if the original file.

    assert(tfs_write(f, text, 5) == 5); // Writes something to the file.

    assert(tfs_close(f) != -1);

    f = tfs_open(file2,
                 TFS_O_CREAT); // Opens the file via the original hard link.

    assert(tfs_read(f, buffer, 5) == 5);

    assert(strcmp(buffer, text) ==
           0); // Compares the content read to the writen.

    assert(tfs_close(f) != -1);

    // destroy TécnicoFS
    assert(tfs_destroy() != -1);

    printf("Successful test.\n");

    return 0;
}