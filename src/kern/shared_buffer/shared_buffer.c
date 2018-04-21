#include <types.h>
#include <shared_buffer.h>
#include <lib.h>
#include <synch.h>

Shared_Buffer *shared_buffer_create(int size) {
    Shared_Buffer *buffer = kmalloc(sizeof(Shared_Buffer));
    if (buffer == NULL) {
        return NULL;
    }

    buffer->buffer = kmalloc(size * sizeof(char));
    if (buffer->buffer == NULL) {
        kfree(buffer);
        return NULL;
    }

    buffer->lock = lock_create("shared buffer lock");
    if (buffer->lock == NULL) {
        kfree(buffer);
        kfree(buffer->buffer);
        return NULL;
    }

    buffer->size = size;
    buffer->count = 0;
    buffer->in = 0;
    buffer->out = 0;

    return buffer;
}


void shared_buffer_destroy(Shared_Buffer *buffer) {
    kfree(buffer->buffer);
    lock_destroy(buffer->lock);
    kfree(buffer);
}


void shared_buffer_produce(Shared_Buffer *buffer, char input) {
    lock_acquire(buffer->lock);

    while (buffer->size == buffer->count);
    // TODO don't spinwait

    buffer->buffer[buffer->in] = input;
    buffer->in = (buffer->in + 1) % buffer->size;
    buffer->count++;

    lock_release(buffer->lock);
}


char shared_buffer_consume(Shared_Buffer *buffer) {
    lock_acquire(buffer->lock);

    while (buffer->count == 0);
    // TODO don't spinwait

    char result = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % buffer->size;
    buffer->count--;

    lock_release(buffer->lock);

    return result;
}