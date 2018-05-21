#include <types.h>
#include <lib.h>
#include <synch.h>
#include <vfs.h>
#include <proc.h>
#include <kern/errno.h>

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

    t->num_open_files = 0;

    for (int i = 0; i < MAX_LOCAL_TABLE_SIZE; ++i) {
        t->files[i] = NULL;
    }

    return t;
}

File_Desc *file_desc_create(File *file, int flags) {
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

    return f;
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

    File_Desc *f = file_desc_create(file, flags);

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
    KASSERT(file_handle >= 0);
    KASSERT(file_handle < MAX_LOCAL_TABLE_SIZE);

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

    t->num_open_files = 0;

    for (int i = 0; i < MAX_GLOBAL_TABLE_SIZE; ++i) {
        t->files[i] = NULL;
    }

    return t;
}

File *file_create(struct vnode *node) {
    KASSERT(node != NULL);

    File *f = kmalloc(sizeof(File));

    f->lk = lock_create("Global file lock");
    if (f->lk == NULL) {
        return NULL;
    }

    f->node = node;

    return f;
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

    File *f = file_create(vn);
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