#include <types.h>
#include <lib.h>
#include <synch.h>
#include <vfs.h>
#include <proc.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>

// Internal Helper functions

static File *file_create(struct vnode *node, bool is_standard) {
    File *f = kmalloc(sizeof(File));
    if (f == NULL) {
        return NULL;
    }

    f->lk = lock_create("Global file lock");
    if (f->lk == NULL) {
        kfree(f);
        return NULL;
    }

    f->is_standard = is_standard;
    f->node = node;

    return f;
}


static File_Desc *file_desc_create(File *file, int flags, int standard) {
    KASSERT(file != NULL);

    File_Desc *f = kmalloc(sizeof(File_Desc));

    if (f == NULL) {
        return NULL;
    }

    f->lk = lock_create("File desc lock");
    if (f->lk == NULL) {
        kfree(f);
        return NULL;
    }

    f->seek_location = 0;
    f->flags = flags & O_ACCMODE;
    f->file = file;
    f->standard = standard;

    return f;
}

bool file_destroy(File *file) {
    KASSERT(file != NULL);

    bool removed = false;

    // Don't remove STDIN/STDOUT global files
    if (!file->is_standard) {

        struct vnode *vn = file->node;
        KASSERT(vn != NULL);

        lock_acquire(global_file_table->lk);

        // Check if vnode refcount is 1 since vfs_close() will decrement and deallocate itself
        if (vn->vn_refcount == 1) {
            // NUll out slot in global file table
            // We don't know our index here so we need to do a scan
            // Sorry James
            for (int i = 0; i < MAX_GLOBAL_TABLE_SIZE; i++) {
                if (global_file_table->files[i] == file) {
                    global_file_table->files[i] = NULL;
                    global_file_table->num_open_files--;
                    break;
                }
            }

            vfs_close(vn);
            lock_destroy(file->lk);
            kfree(file);

            removed = true;

        } else {
            // Decrement vnode ref count
            // Don't deallocate file descriptor resources
            vfs_close(vn);
        }

        lock_release(global_file_table->lk);
    }

    return removed;
}

// De-allocates the local file descriptor as long as the refcount is 1
static void file_desc_destroy(File_Desc *file_desc) {
    KASSERT(file_desc != NULL);

    // Don't deallocate the global STDIN/OUT/ERR objects
    if (file_desc == stdin_fd || file_desc == stdout_fd || file_desc == stderr_fd) {
        return;
    }

    File *file = file_desc->file;
    if (file != NULL) {
        bool removed = file_destroy(file);
        if (removed) {
            lock_destroy(file_desc->lk);
            kfree(file_desc);
        }
    }
}

////////////////////////////////////
// Local File Table Operations
////////////////////////////////////

Local_File_Table *local_table_create() {
    Local_File_Table *t = kmalloc(sizeof(Local_File_Table));

    if (t == NULL) {
        return NULL;
    }

    t->lk = lock_create("Local File Table Lock");
    if (t->lk == NULL) {
        kfree(t);
        return NULL;
    }

    for (int i = 0; i < MAX_LOCAL_TABLE_SIZE; ++i) {
        t->files[i] = NULL;
    }

    // Set first three open files to global STD file descriptors
    t->files[0] = stdin_fd;
    t->files[1] = stdout_fd;
    t->files[2] = stderr_fd;
    t->num_open_files = 3;

    return t;
}

void local_table_destroy(Local_File_Table *table) {
    KASSERT(table != NULL);
    local_table_close_all(table);
    lock_destroy(table->lk);
    kfree(table);
}

int local_table_add_file(Local_File_Table *table, File *file, int flags, int *ret) {
    KASSERT(table != NULL);
    KASSERT(file != NULL);
    KASSERT(ret != NULL);

    lock_acquire(table->lk);

    if (table->num_open_files == MAX_LOCAL_TABLE_SIZE) {
        lock_release(table->lk);
        return EMFILE;
    }

    File_Desc *f = file_desc_create(file, flags, -1);

    if (f == NULL) {
        lock_release(table->lk);
        return ENOSPC;
    }

    for (int i = 0; i < MAX_LOCAL_TABLE_SIZE; i++) {
        if (table->files[i] == NULL) {
            table->files[i] = f;
            *ret = i;
            lock_release(table->lk);
            return 0;
        }
    }

    lock_release(table->lk);
    return EIO;
}

File_Desc *local_table_get(Local_File_Table *table, int file_handle) {
    KASSERT(table != NULL);

    if (file_handle < 0 || file_handle >= MAX_LOCAL_TABLE_SIZE) {
        return NULL;
    }

    lock_acquire(table->lk);
    File_Desc *desc = table->files[file_handle];
    lock_release(table->lk);

    return desc;
}

static int non_lock_close_file(Local_File_Table *table, int file_handle) {
    if (file_handle < 0 || file_handle >= MAX_LOCAL_TABLE_SIZE) {
        return EBADF;
    }

    File_Desc *desc = table->files[file_handle];
    if (desc == NULL) {
        return EBADF;
    }

    // Null out the slot in our local file table
    table->files[file_handle] = NULL;
    table->num_open_files--;

    // Deallocate resources if needed
    file_desc_destroy(desc);

    return 0;
}

int local_table_close_file(Local_File_Table *table, int file_handle) {
    KASSERT(table != NULL);

    lock_acquire(table->lk);

    int err = non_lock_close_file(table, file_handle);

    lock_release(table->lk);

    return err;
}

int local_table_close_all(Local_File_Table *table) {
    KASSERT(table != NULL);

    lock_acquire(table->lk);

    for (int i = 0; i < MAX_LOCAL_TABLE_SIZE; i++) {
        File_Desc *f = table->files[i];
        // Null out slot in our local file table
        table->files[i] = NULL;

        if (f != NULL) {
            // Deallocate resources if needed
            file_desc_destroy(f);
        }
    }

    table->num_open_files = 0;

    lock_release(table->lk);

    return 0;

}

int local_table_copy(Local_File_Table *src, Local_File_Table *dest) {
    KASSERT(src != NULL);
    KASSERT(dest != NULL);

    lock_acquire(src->lk);

    memcpy(dest->files, src->files, sizeof(src->files));

    // Increment ref counts on all copied files
    for (int i = 3; i < MAX_LOCAL_TABLE_SIZE; i++) {
        File_Desc *file_desc = src->files[i];
        // Super ugly null checking
        if (file_desc != NULL) {
            File *file = file_desc->file;
            if (file != NULL) {
                struct vnode *vn = file->node;
                if (vn != NULL) {
                    VOP_INCREF(vn);
                }
            }

        }
    }

    dest->num_open_files = src->num_open_files;
    lock_release(src->lk);

    return 0;
}

int local_table_dup2(Local_File_Table *table, int old_file_handle, int new_file_handle) {
    KASSERT(table != NULL);

    if (old_file_handle == new_file_handle) {
        return 0;
    }

    int err;
    lock_acquire(table->lk);

    File_Desc *desc = table->files[old_file_handle];
    if (desc == NULL) {
        lock_release(table->lk);
        return EBADF;
    }

    File_Desc *n_desc = table->files[new_file_handle];
    if (n_desc != NULL) {
        err = non_lock_close_file(table, new_file_handle);
        if (err) {
            lock_release(table->lk);
            return err;
        }
    }

    table->files[new_file_handle] = desc;

    // Only non-standard file_descs have an active vndoe
    if (desc->standard == -1) {
        VOP_INCREF(desc->file->node);
    }

    table->num_open_files++;

    lock_release(table->lk);

    return 0;
}

////////////////////////////////////
// Global File Table Operations
////////////////////////////////////

Global_File_Table *global_table_create() {
    Global_File_Table *t = kmalloc(sizeof(Global_File_Table));

    if (t == NULL) {
        return NULL;
    }

    t->lk = lock_create("Global File Table Lock");
    if (t->lk == NULL) {
        kfree(t);
        return NULL;
    }

    for (int i = 0; i < MAX_GLOBAL_TABLE_SIZE; ++i) {
        // Create special global files for STD*
        if (i >= 0 && i < 3) {
            File *file = file_create(NULL, true);
            t->files[i] = file;

            // If we failed at creating STD*, panic
            if (file == NULL) {
                panic("Failed to create STDIN/STDOUT/STDERR");
            }
        } else {
            t->files[i] = NULL;
        }
    }

    stdin_fd = file_desc_create(t->files[STDIN_FILENO], O_RDONLY, STDIN_FILENO);
    stdout_fd = file_desc_create(t->files[STDOUT_FILENO], O_WRONLY, STDOUT_FILENO);
    stderr_fd = file_desc_create(t->files[STDERR_FILENO], O_WRONLY, STDERR_FILENO);

    if (stderr_fd == NULL || stdout_fd == NULL || stderr_fd == NULL) {
        panic("Failed to create STDIN/STDOUT/STDERR");
    }

    t->num_open_files = 3;

    return t;
}

int global_table_open_file(char *filename, int flags, File **ret) {
    KASSERT(filename != NULL);
    KASSERT(ret != NULL);

    int err;

    struct vnode *vn;
    // This destroys the filename buffer passed in
    err = vfs_open(filename, flags, 0, &vn);

    if (err) {
        return err;
    }

    lock_acquire(global_file_table->lk);

    if (global_file_table->num_open_files == MAX_GLOBAL_TABLE_SIZE) {
        lock_release(global_file_table->lk);
        return ENFILE;
    }

    // Scan array to see if file is already open globally
    for (int i = 0; i < MAX_GLOBAL_TABLE_SIZE; i++) {
        File *file = global_file_table->files[i];

        if (file != NULL && file->node == vn) {
            *ret = file;
            lock_release(global_file_table->lk);
            return 0;
        }
    }

    File *f = file_create(vn, false);
    if (f == NULL) {
        lock_release(global_file_table->lk);
        return ENOSPC;
    }

    // Put newly created file in first open slot
    for (int i = 0; i < MAX_GLOBAL_TABLE_SIZE; i++) {
        File *file = global_file_table->files[i];
        if (file == NULL) {
            global_file_table->files[i] = f;
            break;
        }
    }

    *ret = f;
    global_file_table->num_open_files++;

    lock_release(global_file_table->lk);
    return 0;
}