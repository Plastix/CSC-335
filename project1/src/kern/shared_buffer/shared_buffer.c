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
        kfree(buffer->buffer);
        kfree(buffer);
        return NULL;
    }

    buffer->cv_producer = cv_create("shared buffer producer cv");
    if (buffer->cv_producer == NULL) {
        kfree(buffer->buffer);
        kfree(buffer->lock);
        kfree(buffer);
        return NULL;
    }

    buffer->cv_consumer = cv_create("shared buffer consumer cv");
    if (buffer->cv_consumer == NULL) {
        kfree(buffer->buffer);
        kfree(buffer->lock);
        kfree(buffer->cv_producer);
        kfree(buffer);
        return NULL;
    }

    buffer->size = size;
    buffer->count = 0;
    buffer->in = 0;
    buffer->out = 0;

    return buffer;
}


void shared_buffer_destroy(Shared_Buffer *buffer) {
    KASSERT(buffer != NULL);

    kfree(buffer->buffer);
    lock_destroy(buffer->lock);
    kfree(buffer);
}


void shared_buffer_produce(Shared_Buffer *buffer, char input) {
    KASSERT(buffer != NULL);

    lock_acquire(buffer->lock);

    while (buffer->size == buffer->count) {
        cv_wait(buffer->cv_producer, buffer->lock);
    }

    buffer->buffer[buffer->in] = input;
    buffer->in = (buffer->in + 1) % buffer->size;
    buffer->count++;

    cv_signal(buffer->cv_consumer, buffer->lock);
    lock_release(buffer->lock);
}


char shared_buffer_consume(Shared_Buffer *buffer) {
    KASSERT(buffer != NULL);

    lock_acquire(buffer->lock);

    while (buffer->count == 0) {
        cv_wait(buffer->cv_consumer, buffer->lock);
    }

    char result = buffer->buffer[buffer->out];
    buffer->out = (buffer->out + 1) % buffer->size;
    buffer->count--;

    cv_signal(buffer->cv_producer, buffer->lock);
    lock_release(buffer->lock);

    return result;
}