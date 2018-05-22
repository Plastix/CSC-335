
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

    // Labels this file standard
    // and therefore should not be removed
    bool is_standard;
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

    // Labels this file descriptor as STD*
    // -1 = Not Standard
    // 0 = STDIN
    // 1 = STDOUT
    // 2 = STDERR
    int standard;

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

int local_table_add_file(Local_File_Table *table, File *file, int flags, int *ret);

File_Desc *local_table_get(Local_File_Table *table, int file_handle);

int local_table_close_file(Local_File_Table *table, int file_handle);

int local_table_close_all(Local_File_Table *table);

int local_table_copy(Local_File_Table *src, Local_File_Table *dest);

////////////////////////////////////
// Global File Table Operations
Global_File_Table *global_table_create(void);

int global_table_open_file(char *filename, int flags, File **ret);



// TODO (Aidan) Add monitor methods

