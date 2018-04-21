#include <types.h>
#include <test.h>
#include <lib.h>
#include <shared_buffer.h>
#include <synch.h>
#include <testlib.h>

#define BUFFER_SIZE 10

// Function signatures
// Test teardown / setup definitions
//static void wait(int);

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
    kfree(buffer);
    sem_destroy(tsem);
}


//static void wait(int thread_num) {
//    for (int i = 0; i < thread_num; i++) {
//        P(tsem);
//    }
//}

TEST(new_shared_buffer) {
    ASSERT_NOT_NULL(buffer);
    ASSERT_INT_EQ(10, buffer->size);
    ASSERT_INT_EQ(0, buffer->count);
}

TEST_SUITE(sequential_shared_buffer) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);

    RUN_TEST(new_shared_buffer);
}

TEST_SUITE(concurrent_shared_buffer) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);
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

