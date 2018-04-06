#include <linked_list.h>
#include <lib.h>
#include <thread.h>
#include <spl.h>
#include <test.h>
#include <testlib.h>

static Linked_List *list;

void test_setup(void) {
    list = linked_list_create();
}

void test_teardown(void) {
    kfree(list);
}

TEST(empty_list) {
    ASSERT_INT_EQ(0, list->length);
}

TEST(append_test1) {
    int *c = kmalloc(sizeof(int));
    *c = 10;
    linked_list_prepend(list, c);

    ASSERT_INT_EQ(1, list->length);
    ASSERT_INT_EQ(*c, *(int *) list->first->data);
}

TEST(append_test2) {
    for(int i = 0; i < 3; i ++) {
        int *num = kmalloc(sizeof(int));
        *num = i;
        linked_list_prepend(list, num);

        if(i == 1){
            ASSERT_INT_EQ(2, list->length);
            ASSERT_INT_EQ(1, *(int *) list->first->data);
        }else if(i == 2){
            ASSERT_INT_EQ(3, list->length);
            ASSERT_INT_EQ(2, *(int *) list->first->data);
        }
    }
}

TEST_SUITE(linked_list_tests) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);

    RUN_TEST(empty_list);
    RUN_TEST(append_test1);
    RUN_TEST(append_test2);
}

int linked_list_test_run(int nargs, char **args) {
    int testnum = 0;
    if (nargs == 2) {
        testnum = args[1][0] - '0'; // XXX - Hack - only works for testnum 0 -- 9
    }
    kprintf("Running Test: %d\n", testnum);

    RUN_SUITE(linked_list_tests);
    TEST_REPORT();
    return 0;
}

//static void linked_list_test_adder(void *list, unsigned long which) {
//    splhigh();
//
//    int i;
//    int *c;
//
//    Linked_List *linked_list = list;
//    int length = linked_list->length;
//
//    for (i = 0; i < 10; i++) {
//        c = kmalloc(sizeof(int));
//        *c = 'A' + i;
//        linked_list_prepend(list, c);
//
//        linked_list_printlist(list, which);
//    }
//
//    KASSERT(linked_list->length == length + 10);
//}

//int linked_list_test_run(int nargs, char **args) {
//    int testnum = 0;
//
//    if (nargs == 2) {
//        testnum = args[1][0] - '0'; // XXX - Hack - only works for testnum 0 -- 9
//    }
//
//    kprintf("testnum: %d\n", testnum);
//
//    Linked_List *list = linked_list_create();
//
//    thread_fork("adder 1",
//                NULL,
//                linked_list_test_adder,
//                list,
//                1);
//
//    thread_fork("adder 2",
//                NULL,
//                linked_list_test_adder,
//                list,
//                2);
//
//    // XXX - Bug - We're returning from this function without waiting
//    // for these two threads to finish.  The execution of these
//    // threads may interleave with the kernel's main menu thread and
//    // cause interleaving of console output.  We going to accept this
//    // problem for the moment until we learn how to fix in Project 2.
//    // An enterprising student might investigate why this is not a
//    // problem with other tests suites the kernel uses.
//
//    return 0;
//}
