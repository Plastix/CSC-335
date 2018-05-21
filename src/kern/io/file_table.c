
#include <filetable.h>
#include <lib.h>
#include <synch.h>

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

int local_table_add_file(Local_File_Table *table, File *file, int *ret) {
    (void) table;
    (void) file;
    (void) ret;
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

    t->num_open_files = 0;

    for (int i = 0; i < MAX_GLOBAL_TABLE_SIZE; ++i) {
        t->files[i] = NULL;
    }

    return t;
}

int global_table_open_file(char *filename, int flags, File **ret) {
    (void) filename;
    (void) flags;
    (void) ret;
    return 0;
}