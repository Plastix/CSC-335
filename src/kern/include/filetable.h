
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

} File_Table;

////////////////////////////////////
// Local File Table

typedef struct {

    // Pointer to file in global file table
    File *file;

    // File flags (read-only, write-only,etc)
    int flags;

    off_t seek_location;


} File_Desc;

typedef struct {

    // Array of open file objects
    File_Desc *files[MAX_LOCAL_TABLE_SIZE];

    // Number of currently open files
    volatile int num_open_files;

    // Lock for sync
    struct lock *lk;

} File_Desc_Table;

// TODO (Aidan) Add monitor methods

