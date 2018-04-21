#include <types.h>
#include <shared_buffer.h>

Shared_Buffer *shared_buffer_create(int size) {
    (void) size;
    return NULL;
}


void shared_buffer_destroy(Shared_Buffer *buffer) {
    (void) buffer;
}


void shared_buffer_produce(Shared_Buffer *buffer, char input) {
    (void) buffer;
    (void) input;
}


char shared_buffer_consume(Shared_Buffer *buffer) {
    (void) buffer;
    return 0;
}