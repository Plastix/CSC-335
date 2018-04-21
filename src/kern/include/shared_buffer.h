//
// Shared Buffer Interface
// Aidan Pieper
//

typedef struct {
    char *buffer; // Array of characters
    volatile int in;
    volatile int out;
    volatile int count;
    int size;
    struct lock *lock;
} Shared_Buffer;

/**
 * Creates a shared buffer of {@param size} characters.
 * @param size Size of buffer.
 * @return Pointer to new shared buffer struct. Returned pointer is NULL if allocation fails.
 */
Shared_Buffer *shared_buffer_create(int size);

/**
 * Destroys the specified shared buffer and frees all allocated memory.
 * @param buffer Pointer to shared buffer to destroy.
 */
void shared_buffer_destroy(Shared_Buffer *buffer);

/**
 * Adds {@param input} to the specified shared buffer.
 * @param buffer Buffer to add item to.
 * @param input Character to add.
 */
void shared_buffer_produce(Shared_Buffer *buffer, char input);

/**
 * Consumes an item from the specified shared buffer.
 * @param buffer Buffer to retrieve item from.
 * @return The removed character.
 */
char shared_buffer_consume(Shared_Buffer *buffer);
