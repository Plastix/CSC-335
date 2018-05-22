#include <types.h>
#include <lib.h>
#include <synch.h>
#include <vfs.h>
#include <proc.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>

// Internal Helper functions

static File *global_table_get_standard(int file_handle) {

    lock_acquire(global_file_table->lk);
    File *desc = global_file_table->files[file_handle];
    lock_release(global_file_table->lk);

    return desc;
}

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
    f->flags = flags;
    f->file = file;
    f->standard = standard;

    return f;
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

    t->files[0] = file_desc_create(global_table_get_standard(STDIN_FILENO), O_RDONLY, STDIN_FILENO);
    t->files[1] = file_desc_create(global_table_get_standard(STDOUT_FILENO), O_WRONLY, STDOUT_FILENO);
    t->files[2] = file_desc_create(global_table_get_standard(STDERR_FILENO), O_WRONLY, STDIN_FILENO);

    t->num_open_files = 3;

    // Creating STDIN/STDOUT/STDERR failed
    if (t->files[0] == NULL || t->files[1] == NULL || t->files[2] == NULL) {
        kfree(t->lk);
        kfree(t);
        return NULL;
    }

    return t;
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

    if (file_handle < 0 || file_handle >= table->num_open_files) {
        return NULL;
    }

    lock_acquire(table->lk);
    File_Desc *desc = table->files[file_handle];
    lock_release(table->lk);

    return desc;
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
            t->files[i] = file_create(NULL, true);
            // If we failed at creating, panic
            if (t->files[i] == NULL) {
                panic("Failed to create STDIN/STDOUT/STDERR");
            }
        } else {
            t->files[i] = NULL;
        }
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

    global_file_table->num_open_files++;

    lock_release(global_file_table->lk);
    return 0;
}