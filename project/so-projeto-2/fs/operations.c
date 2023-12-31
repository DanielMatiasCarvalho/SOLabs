#include "operations.h"
#include "config.h"
#include "state.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "betterassert.h"

tfs_params tfs_default_params() {
    tfs_params params = {
        .max_inode_count = 64,
        .max_block_count = 1024,
        .max_open_files_count = 16,
        .block_size = 1024,
    };
    return params;
}

int tfs_init(tfs_params const *params_ptr) {
    tfs_params params;
    if (params_ptr != NULL) {
        params = *params_ptr;
    } else {
        params = tfs_default_params();
    }

    if (state_init(params) != 0) {
        return -1;
    }

    // create root inode
    int root = inode_create(T_DIRECTORY);
    if (root != ROOT_DIR_INUM) {
        return -1;
    }

    return 0;
}

int tfs_destroy() {
    if (state_destroy() != 0) {
        return -1;
    }
    return 0;
}

static bool valid_pathname(char const *name) {
    return name != NULL && strlen(name) > 1 && name[0] == '/';
}

/**
 * Looks for a file.
 *
 * Note: as a simplification, only a plain directory space (root directory only)
 * is supported.
 *
 * Input:
 *   - name: absolute path name
 *   - root_inode: the root directory inode
 * Returns the inumber of the file, -1 if unsuccessful.
 */
static int tfs_lookup(char const *name, inode_t const *root_inode) {

    ALWAYS_ASSERT(
        root_inode == inode_get(ROOT_DIR_INUM),
        "tfs_lookup: root_inode must be the inode of the root directory");

    if (!valid_pathname(name)) {
        return -1;
    }

    // skip the initial '/' character
    name++;

    return find_in_dir(root_inode, name);
}

int tfs_open(char const *name, tfs_file_mode_t mode) {
    // Checks if the path name is valid
    if (!valid_pathname(name)) {
        return -1;
    }

    inode_t *root_dir_inode = inode_get(ROOT_DIR_INUM);
    ALWAYS_ASSERT(root_dir_inode != NULL,
                  "tfs_open: root dir inode must exist");
    int inum = tfs_lookup(name, root_dir_inode);
    size_t offset;

    if (inum >= 0) {
        // The file already exists
        inode_t *inode = inode_get(inum);
        ALWAYS_ASSERT(inode != NULL,
                      "tfs_open: directory files must have an inode");

        if (inode->i_node_type == T_SYMLINK) {
            char path[inode->i_size + 1];

            void *data = data_block_get(inode->i_data_block);
            ALWAYS_ASSERT(data != NULL,
                          "tfs_open: data block deleted mid-write");

            pthread_rwlock_rdlock(&inode_rwlocks[inum]);

            memcpy(path, data, inode->i_size);

            pthread_rwlock_unlock(&inode_rwlocks[inum]);

            path[inode->i_size] = '\0';

            return tfs_open(path, mode);
        }

        // Truncate (if requested)
        if (mode & TFS_O_TRUNC) {
            pthread_rwlock_wrlock(&inode_rwlocks[inum]);
            if (inode->i_size > 0) {
                data_block_free(inode->i_data_block);
                inode->i_size = 0;
            }
            pthread_rwlock_unlock(&inode_rwlocks[inum]);
        }

        // Determine initial offset
        if (mode & TFS_O_APPEND) {
            offset = inode->i_size;
        } else {
            offset = 0;
        }
    } else if (mode & TFS_O_CREAT) {
        // The file does not exist; the mode specified that it should be created
        // Create inode
        inum = inode_create(T_FILE);
        if (inum == -1) {
            return -1; // no space in inode table
        }

        // Add entry in the root directory
        if (add_dir_entry(root_dir_inode, name + 1, inum) == -1) {
            inode_delete(inum);
            return -1; // no space in directory
        }

        offset = 0;
    } else {
        return -1;
    }

    // Finally, add entry to the open file table and return the corresponding
    // handle
    return add_to_open_file_table(inum, offset);

    // Note: for simplification, if file was created with TFS_O_CREAT and there
    // is an error adding an entry to the open file table, the file is not
    // opened but it remains created
}

int tfs_sym_link(char const *target, char const *link_name) {
    inode_t *root_dir_inode = inode_get(ROOT_DIR_INUM);
    ALWAYS_ASSERT(root_dir_inode != NULL,
                  "tfs_sym_link: root dir inode must exist");

    if (tfs_lookup(target, root_dir_inode) == -1) {
        return -1;
    }

    if (tfs_lookup(link_name, root_dir_inode) != -1) {
        return -1;
    }

    int inum = inode_create(T_SYMLINK);
    if (inum == -1) {
        return -1;
    }

    int dblock = data_block_alloc();
    if (dblock == -1) {
        inode_delete(inum);
        return -1;
    }

    inode_t *sym_link_inode = inode_get(inum);
    pthread_rwlock_wrlock(&inode_rwlocks[inum]);

    sym_link_inode->i_data_block = dblock;

    void *data = data_block_get(sym_link_inode->i_data_block);
    ALWAYS_ASSERT(data != NULL, "tfs_sym_link: data block deleted mid-write");

    size_t name_size = strlen(target);

    memcpy(data, target, name_size);

    sym_link_inode->i_size = name_size;

    pthread_rwlock_unlock(&inode_rwlocks[inum]);

    add_dir_entry(root_dir_inode, link_name + 1, inum);

    return 0;
}

int tfs_link(char const *target, char const *link_name) {
    inode_t *root_dir_inode = inode_get(ROOT_DIR_INUM);
    ALWAYS_ASSERT(root_dir_inode != NULL,
                  "tfs_link: root dir inode must exist");

    int inumber = tfs_lookup(target, root_dir_inode);

    if (inumber == -1) {
        return -1;
    }

    if (tfs_lookup(link_name, root_dir_inode) != -1) {
        return -1;
    }

    inode_t *file_inode = inode_get(inumber);
    ALWAYS_ASSERT(file_inode != NULL, "tfs_link: target inode must exist");

    if (file_inode->i_node_type == T_SYMLINK) {
        return -1;
    }

    if (add_dir_entry(root_dir_inode, link_name + 1, inumber) == -1) {
        return -1;
    }

    pthread_rwlock_wrlock(&inode_rwlocks[inumber]);
    file_inode->number_hard_links++;
    pthread_rwlock_unlock(&inode_rwlocks[inumber]);

    return 0;
}

int tfs_close(int fhandle) {
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1; // invalid fd
    }

    remove_from_open_file_table(fhandle);

    return 0;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t to_write) {
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    pthread_mutex_lock(&open_file_entry_mutex[fhandle]);

    int inum = file->of_inumber;

    //  From the open file table entry, we get the inode
    inode_t *inode = inode_get(inum);
    ALWAYS_ASSERT(inode != NULL, "tfs_write: inode of open file deleted");

    // Determine how many bytes to write
    size_t block_size = state_block_size();
    if (to_write + file->of_offset > block_size) {
        to_write = block_size - file->of_offset;
    }

    if (to_write > 0) {
        pthread_rwlock_wrlock(&inode_rwlocks[inum]);

        if (inode->i_size == 0) {
            // If empty file, allocate new block
            int bnum = data_block_alloc();
            if (bnum == -1) {
                pthread_rwlock_unlock(&inode_rwlocks[inum]);
                pthread_mutex_unlock(&open_file_entry_mutex[fhandle]);

                return -1; // no space
            }

            inode->i_data_block = bnum;
        }

        void *block = data_block_get(inode->i_data_block);
        ALWAYS_ASSERT(block != NULL, "tfs_write: data block deleted mid-write");

        // Perform the actual write
        memcpy(block + file->of_offset, buffer, to_write);

        // The offset associated with the file handle is incremented accordingly
        file->of_offset += to_write;
        if (file->of_offset > inode->i_size) {
            inode->i_size = file->of_offset;
        }
        pthread_rwlock_unlock(&inode_rwlocks[inum]);
    }

    pthread_mutex_unlock(&open_file_entry_mutex[fhandle]);

    return (ssize_t)to_write;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    open_file_entry_t *file = get_open_file_entry(fhandle);
    if (file == NULL) {
        return -1;
    }

    // From the open file table entry, we get the inode
    pthread_mutex_lock(&open_file_entry_mutex[fhandle]);

    int inum = file->of_inumber;

    inode_t const *inode = inode_get(inum);
    ALWAYS_ASSERT(inode != NULL, "tfs_read: inode of open file deleted");

    pthread_rwlock_rdlock(&inode_rwlocks[inum]);

    // Determine how many bytes to read
    size_t to_read = inode->i_size - file->of_offset;
    if (to_read > len) {
        to_read = len;
    }

    if (to_read > 0) {
        void *block = data_block_get(inode->i_data_block);
        ALWAYS_ASSERT(block != NULL, "tfs_read: data block deleted mid-read");

        // Perform the actual read
        memcpy(buffer, block + file->of_offset, to_read);
        // The offset associated with the file handle is incremented accordingly
        file->of_offset += to_read;
    }

    pthread_mutex_unlock(&open_file_entry_mutex[fhandle]);
    pthread_rwlock_unlock(&inode_rwlocks[inum]);

    return (ssize_t)to_read;
}

int tfs_unlink(char const *target) {
    inode_t *root_dir_inode = inode_get(ROOT_DIR_INUM);
    ALWAYS_ASSERT(root_dir_inode != NULL,
                  "tfs_unlink: root dir inode must exist");
    int inumber = tfs_lookup(target, root_dir_inode);
    if (inumber == -1) {
        return -1;
    }

    /*We removed the search for the file descriptor of the file in the open file
    table so that we can unlink(delete) any file/box whenever we want*/

    inode_t *file_inode = inode_get(inumber);
    ALWAYS_ASSERT(file_inode != NULL, "tfs_unlink: target inode must exist");

    if (clear_dir_entry(root_dir_inode, target + 1) == -1) {
        return -1;
    }

    pthread_rwlock_wrlock(&inode_rwlocks[inumber]);
    if ((--file_inode->number_hard_links) == 0) {

        if (file_inode->i_size > 0) {
            size_t block_size = state_block_size();
            void *block = data_block_get(file_inode->i_data_block);
            memset(block, 0, block_size);
        }

        inode_delete(inumber);
        remove_inode_from_open_file_table(inumber);
    }

    pthread_rwlock_unlock(&inode_rwlocks[inumber]);

    return 0;
}

int tfs_copy_from_external_fs(char const *source_path, char const *dest_path) {
    int source_fd = open(source_path, O_RDONLY);

    if (source_fd < 0) {
        return -1;
    }

    int dest_fd = tfs_open(dest_path, TFS_O_CREAT | TFS_O_TRUNC);

    if (dest_fd < 0) {
        close(source_fd);
        return -1;
    }

    size_t size = state_block_size();
    char *buffer[size];

    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = read(source_fd, buffer, size);

    if (bytes_read < 0) {
        close(source_fd);
        tfs_close(dest_fd);
        return -1;
    }

    ssize_t bytes_writen = tfs_write(dest_fd, buffer, (size_t)bytes_read);

    if (bytes_writen < 0) {
        close(source_fd);
        tfs_close(dest_fd);
        return -1;
    }

    if (close(source_fd) < 0) {
        tfs_close(dest_fd);
        return -1;
    }

    if (tfs_close(dest_fd) < 0) {
        return -1;
    }

    return 0;
}
