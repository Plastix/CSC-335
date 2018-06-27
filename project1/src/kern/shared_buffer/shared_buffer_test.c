#include <types.h>
#include <test.h>
#include <lib.h>
#include <shared_buffer.h>
#include <synch.h>
#include <testlib.h>
#include <thread.h>

#define BUFFER_SIZE 10

// Function signatures
void produce_thread(void *args, unsigned long count);

void consume_thread(void *args, unsigned long count);

static void wait(int);

//
// Test Util Functions
//
static Shared_Buffer *buffer;
static struct semaphore *tsem = NULL;

static void test_setup(void) {
    buffer = shared_buffer_create(BUFFER_SIZE);

    tsem = sem_create("Test semaphore", 0);
    if (tsem == NULL) {
        panic("linked list test: failed to create semaphore!");
    }
}

static void test_teardown(void) {
    shared_buffer_destroy(buffer);
    sem_destroy(tsem);
}


static void wait(int thread_num) {
    for (int i = 0; i < thread_num; i++) {
        P(tsem);
    }
}

TEST(new_shared_buffer) {
    ASSERT_NOT_NULL(buffer);
    ASSERT_INT_EQ(10, buffer->size);
    ASSERT_INT_EQ(0, buffer->count);
}

TEST(add_char_to_empty_buffer) {
    ASSERT_NOT_NULL(buffer);
    shared_buffer_produce(buffer, 'A');
    ASSERT_INT_EQ(1, buffer->count);
}

TEST(add_two_char_to_empty_buffer) {
    ASSERT_NOT_NULL(buffer);
    shared_buffer_produce(buffer, 'A');
    shared_buffer_produce(buffer, 'B');
    ASSERT_INT_EQ(2, buffer->count);
}

TEST(remove_char_from_nonempty_buffer) {
    ASSERT_NOT_NULL(buffer);
    char input = 'A';
    shared_buffer_produce(buffer, input);
    char output = shared_buffer_consume(buffer);
    ASSERT(input == output, "Consumed character not produced character!");
    ASSERT_INT_EQ(0, buffer->count);
}

TEST(remove_two_char_from_nonempty_buffer) {
    ASSERT_NOT_NULL(buffer);
    char inputs[2] = {
            'A',
            'B'
    };
    shared_buffer_produce(buffer, inputs[0]);
    shared_buffer_produce(buffer, inputs[1]);

    char outputs[2] = {
            '0',
            '0'
    };

    outputs[0] = shared_buffer_consume(buffer);
    outputs[1] = shared_buffer_consume(buffer);

    ASSERT(inputs[0] == outputs[0], "Consumed character not produced character!");
    ASSERT(inputs[1] == outputs[1], "Consumed character not produced character!");
    ASSERT_INT_EQ(0, buffer->count);
}

TEST_SUITE(sequential_shared_buffer) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);

    RUN_TEST(new_shared_buffer);
    RUN_TEST(add_char_to_empty_buffer);
    RUN_TEST(add_two_char_to_empty_buffer);
    RUN_TEST(remove_char_from_nonempty_buffer);
    RUN_TEST(remove_two_char_from_nonempty_buffer);
}

void produce_thread(void *args, unsigned long count) {
    // Hacky fix to fix unused param warning
    (void) count;
    ASSERT_NOT_NULL(args);

    shared_buffer_produce(buffer, *(char *) args);
    V(tsem);
}

void consume_thread(void *args, unsigned long count) {
    // Hacky fix to fix unused param warning
    (void) args;
    (void) count;

    ASSERT_NOT_NULL(args);
    char *value = args;
    *value = shared_buffer_consume(buffer);
    V(tsem);
}

TEST(double_concurrent_produce_not_full) {
    ASSERT_NOT_NULL(buffer);
    char input = 'A';
    thread_fork("thread1", NULL, produce_thread, &input, 0);
    thread_fork("thread2", NULL, produce_thread, &input, 0);

    wait(2);
    ASSERT_INT_EQ(2, buffer->count);
}

TEST(double_concurrent_consume_not_empty) {
    ASSERT_NOT_NULL(buffer);
    char input1 = 'A';
    char input2 = 'B';
    shared_buffer_produce(buffer, input1);
    shared_buffer_produce(buffer, input2);

    char output1 = '0';
    char output2 = '0';
    thread_fork("thread1", NULL, consume_thread, &input1, 0);
    thread_fork("thread2", NULL, consume_thread, &input2, 0);

    wait(2);
    ASSERT_INT_EQ(0, buffer->count);
    ASSERT(input1 = output1, "Consumed char does not match produced!");
    ASSERT(input2 = output2, "Consumed char does not match produced!");
}


TEST(consume_empty_buffer) {
    ASSERT_NOT_NULL(buffer);
    char consumed = '0';
    char produce = 'A';
    thread_fork("thread1", NULL, consume_thread, &consumed, 0);
    thread_fork("thread2", NULL, produce_thread, &produce, 0);

    wait(2);
    ASSERT_INT_EQ(0, buffer->count);
    ASSERT(consumed = produce, "Consumed char does not match produced!");
}

TEST(produce_full_buffer) {
    ASSERT_NOT_NULL(buffer);

    for (int i = 0; i < buffer->size; i++) {
        char c = (char) ('A' + i);
        shared_buffer_produce(buffer, c);
    }

    char input = 'X';
    char output = '0';
    thread_fork("thread1", NULL, produce_thread, &input, 0);
    thread_fork("thread2", NULL, consume_thread, &output, 0);
    thread_fork("thread3", NULL, consume_thread, &output, 0);


    wait(3);
    ASSERT_INT_EQ(9, buffer->count);
    ASSERT(input = output, "Consumed char does not match produced!");
}

TEST_SUITE(concurrent_shared_buffer) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);

    RUN_TEST(double_concurrent_produce_not_full);
    RUN_TEST(double_concurrent_consume_not_empty);
    RUN_TEST(consume_empty_buffer);
    RUN_TEST(produce_full_buffer);
}

// Test Entry point
int shared_buffer_test_run(int nargs, char **args) {
    (void) nargs;
    (void) args;
    RESET_COUNTERS();
    RUN_SUITE(sequential_shared_buffer);
    RUN_SUITE(concurrent_shared_buffer);
    TEST_REPORT();
    return 0;
}

