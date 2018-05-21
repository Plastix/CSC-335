
#include <types.h>
#include <vnode.h>

#define MAX_LOCAL_TABLE_SIZE 16
#define MAX_GLOBAL_TABLE_SIZE 64

////////////////////////////////
// Global File Table

typedef struct {
    // Currently open vnode
    // Vnode contains refcount internally
    struct vnode *node;

    // Lock for sync (read/write)
    struct lock *lk;
} File;


typedef struct {

    // Array of open file objects
    File *files[MAX_GLOBAL_TABLE_SIZE];

    // Number of currently open files
    volatile int num_open_files;

    // Lock for sync
    struct lock *lk;

} Global_File_Table;

////////////////////////////////////
// Local File Table

typedef struct {

    // Pointer to file in global file table
    File *file;

    // File flags (read-only, write-only,etc)
    int flags;

    off_t seek_location;

    // Lock for sync
    struct lock *lk;

} File_Desc;

typedef struct {

    // Array of open file objects
    File_Desc *files[MAX_LOCAL_TABLE_SIZE];

    // Number of currently open files
    volatile int num_open_files;

    // Lock for sync
    struct lock *lk;

} Local_File_Table;

////////////////////////////////////
// Local File Table Operations
Local_File_Table *local_table_create(void);

File_Desc *file_desc_create(File *file, int flags);

int local_table_add_file(Local_File_Table *table, File *file, int flags, int *ret);

File_Desc *local_table_get(Local_File_Table *table, int file_handle);

////////////////////////////////////
// Global File Table Operations
Global_File_Table *global_table_create(void);

File *file_create(struct vnode *node);

int global_table_open_file(char *filename, int flags, File **ret);



// TODO (Aidan) Add monitor methods

