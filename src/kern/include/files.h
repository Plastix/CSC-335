#include <types.h>
#include <vfs.h>
#include <vnode.h>

#define MAX_OPEN_FILES 16

// These are initialized in proc_bootstrap in proc.c, when the kernel process is created
// Global counter for file descriptors
//int fd_counter;
// Global lock to protect the file descriptor count
//struct lock *fd_lock;

// structure of file handle
typedef struct {

    // File descriptor of the current file
    int file_descriptor;

    // Vnode of the current file
    struct vnode *node;

    // Lock for protecting read/writes to the file
    struct lock *lock;

    // File flags (read-only/write-only/etc)
    int flags;

    // Current location in file
    off_t seek_location;


    int ref_count;

} File;


typedef struct {

    // Buffer to hold open files
    File *files[MAX_OPEN_FILES];

    // Number of currently open files
    unsigned num_open_files;

    // Lock for making the file table a monitor
    struct lock *lock;

} File_Table;

/**
 * Internal constructor for creating a File. This should not be called by anyone other than
 * file_table_create_file.
 *
 * @param node vnode of file to add. Must not be NULL.
 * @param flags flags that the vnode was opened.
 * @return Pointer to newly created file object. NULL if an error occurs while creating the file.
 */
File *file_create(struct vnode *node, int flags);
//TODO (Violet): DO we need off_t here?

/**
 * Constructor for creating a new File Table.
 */
File_Table *file_table_create(void);

/**
 * Destructor for a File Table.
 *
 * @param file_table Pointer of File Table to destroy. Must not be NULL.
 */
void file_table_destroy(File_Table *file_table);

/**
 * Checks if specified File Table is full.
 *
 * @param file_table File Table to query. Must not be NULL.
 * @return True if the File Table is full, otherwise False.
 */
bool file_table_is_full(File_Table *file_table);

/**
 * Creates a new File in the File Table with the corresponding flags and vnode.
 * Callers must check file_table_is_full() before calling this function!
 *
 * @param file_table File Table to update. Must not be NULL.
 * @param node vnode of file. Must not be Null.
 * @param flags Flags of file.
 * @return Returns a pointer to the newly created File object in the table. NULL if an error occurs.
 */
File *file_table_create_file(File_Table *file_table, struct vnode *node, int flags);



/* sys_close function  */
int sys_close(int fd);

/* sys_dup2 function  */
int sys_dup2(int oldfd, int newfd, int * retval);

/* sys_lseek function  */
int sys_lseek(int fd, off_t pos, int whence, off_t * retVal64);

/* sys_chdir function  */
int sys_chdir(const_userptr_t pathname);

/* sys_getcwd function  */
int sys_getcwd(userptr_t buf, int * retval);

