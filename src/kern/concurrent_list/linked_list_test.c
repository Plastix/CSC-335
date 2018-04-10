#include <linked_list.h>
#include <lib.h>
#include <test.h>
#include <testlib.h>
#include <thread.h>

//
// Helper Functions
//
int *allocate_int(int);

int *allocate_int(int value) {
    int *pointer = kmalloc(sizeof(int));
    *pointer = value;
    return pointer;
}

void clear_ints(int *nums[], int);

void clear_ints(int *nums[], int cont) {
    for (int i = 0; i < cont; i++) {
        kfree(nums[i]);
    }
}

//
// Test Util Functions
//
static Linked_List *list;

void test_setup(void) {
    list = linked_list_create();
}

void test_teardown(void) {
    kfree(list);
}

//
// Tests
//
TEST(empty_list) {
    ASSERT_INT_EQ(0, list->length);
    ASSERT_NULL(list->first);
    ASSERT_NULL(list->last);
}

TEST(node_creation) {
    Linked_List_Node *node = linked_list_create_node(10, NULL);

    ASSERT_INT_EQ(10, node->key);
    ASSERT_NULL(node->next);
    ASSERT_NULL(node->prev);
    ASSERT_NULL(node->data);

    kfree(node);
}

TEST(append_test1) {
    int *num = allocate_int(10);
    linked_list_prepend(list, num);

    ASSERT_INT_EQ(1, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_INT_EQ(*num, *(int *) first->data);
    ASSERT(first == last, "First and last nodes are not the same!");

    kfree(num);
}

TEST(append_test2) {
    const int INT_NUM = 3;
    int *nums[INT_NUM];
    for (int i = 0; i < INT_NUM; i++) {
        int *num = allocate_int(i);
        nums[i] = num;
        linked_list_prepend(list, num);
    }
    ASSERT_INT_EQ(3, list->length);

    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_INT_EQ(2, *(int *) first->data);
    ASSERT_INT_EQ(0, *(int *) last->data);

    for (int i = 0; i < INT_NUM; i++) {
        kfree(nums[i]);
    }
}

TEST(insert_null_list) {
    int *data = allocate_int(5);
    linked_list_insert(NULL, 0, data);
    kfree(data);
}

TEST(insert_empty_list) {
    int *data = allocate_int(5);
    linked_list_insert(list, 0, data);

    ASSERT_INT_EQ(1, list->length);
    Linked_List_Node *first = list->first;
    ASSERT_NOT_NULL(first);
    ASSERT_NULL(first->prev);
    ASSERT_NULL(first->next);
    ASSERT_INT_EQ(*data, *(int *) first->data);
    ASSERT_INT_EQ(0, first->key);

    Linked_List_Node *last = list->last;
    ASSERT(first == last, "First and last nodes are not the same!");

    kfree(data);
}

TEST(insert_front_singleton) {
    int *one = allocate_int(5);
    linked_list_prepend(list, one); // key = 0

    int *two = allocate_int(10);
    linked_list_insert(list, -1, two);

    ASSERT_INT_EQ(2, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_NULL(first->prev);
    ASSERT_NULL(last->next);
    ASSERT(first->next == last, "First does not point to next!");
    ASSERT(last->prev == first, "Last does not point to first!");

    ASSERT(first != last, "First and last nodes are the same!");
    ASSERT_INT_EQ(*two, *(int *) first->data);
    ASSERT_INT_EQ(*one, *(int *) last->data);
    ASSERT_INT_EQ(-1, first->key);

    kfree(one);
    kfree(two);
}

TEST(insert_back_singleton) {
    int *one = allocate_int(5);
    linked_list_prepend(list, one); // key = 0

    int *two = allocate_int(10);
    linked_list_insert(list, 1, two);

    ASSERT_INT_EQ(2, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;

    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_NULL(first->prev);
    ASSERT_NULL(last->next);
    ASSERT(first->next == last, "First does not point to next!");
    ASSERT(last->prev == first, "Last does not point to first!");

    ASSERT(first != last, "First and last nodes are the same!");
    ASSERT_INT_EQ(*two, *(int *) last->data);
    ASSERT_INT_EQ(*one, *(int *) first->data);
    ASSERT_INT_EQ(1, last->key);

    kfree(one);
    kfree(two);
}

TEST(insert_front_nonempty) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // key = 0
    linked_list_prepend(list, nums[1]); // key = -1

    linked_list_insert(list, -2, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_NULL(first->prev);
    ASSERT_INT_EQ(*nums[2], *(int *) first->data);
    ASSERT_INT_EQ(-2, first->key);

    Linked_List_Node *second = first->next;
    Linked_List_Node *third = second->next;

    ASSERT_NOT_NULL(second);
    ASSERT_NOT_NULL(third);
    ASSERT(second->prev == first, "Second does not point to first!");
    ASSERT(third->prev == second, "Third does not point to second!");
    ASSERT_NULL(third->next);

    clear_ints(nums, 3);
}

TEST(insert_front_nonempty_samekey) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // key = 0
    linked_list_prepend(list, nums[1]); // key = -1

    linked_list_insert(list, -1, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_NULL(first->prev);
    ASSERT_INT_EQ(*nums[2], *(int *) first->data);
    ASSERT_INT_EQ(-1, first->key);

    Linked_List_Node *second = first->next;
    Linked_List_Node *third = second->next;

    ASSERT_NOT_NULL(second);
    ASSERT_NOT_NULL(third);
    ASSERT(second->prev == first, "Second does not point to first!");
    ASSERT(third->prev == second, "Third does not point to second!");
    ASSERT_NULL(third->next);

    clear_ints(nums, 3);
}


TEST(insert_back_nonempty) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // key = 0
    linked_list_prepend(list, nums[1]); // key = -1

    linked_list_insert(list, 1, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *last = list->last;
    Linked_List_Node *first = list->first;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_NULL(last->next);
    ASSERT_INT_EQ(*nums[2], *(int *) last->data);
    ASSERT_INT_EQ(1, last->key);

    Linked_List_Node *second = first->next;
    Linked_List_Node *third = second->next;

    ASSERT_NOT_NULL(second);
    ASSERT_NOT_NULL(third);
    ASSERT(second->prev == first, "Second does not point to first!");
    ASSERT(third->prev == second, "Third does not point to second!");

    clear_ints(nums, 3);
}

TEST(insert_middle) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // key = 0
    linked_list_prepend(list, nums[1]); // key = -1

    linked_list_insert(list, 0, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *second = first->next;
    Linked_List_Node *third = second->next;

    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(second);
    ASSERT_NOT_NULL(third);
    ASSERT_INT_EQ(*nums[2], *(int *) second->data);
    ASSERT_INT_EQ(0, second->key);

    ASSERT(second->prev == first, "Second does not point to first!");
    ASSERT(third->prev == second, "Third does not point to second!");


    clear_ints(nums, 3);
}

TEST(remove_null_list) {
    int *key = allocate_int(5);
    void *removed = linked_list_remove_head(NULL, key);
    ASSERT_NULL(removed);
    ASSERT_INT_EQ(5, *key);
    kfree(key);
}

TEST(remove_empty_list) {
    int *key = allocate_int(5);
    void *removed = linked_list_remove_head(list, key);
    ASSERT_NULL(removed);
    ASSERT_INT_EQ(5, *key);
    kfree(key);
}

TEST(remove_singleton) {
    int *data = allocate_int(10);
    linked_list_prepend(list, data); // key = 0

    int *key = allocate_int(5);
    int *removed = linked_list_remove_head(list, key);
    ASSERT_NOT_NULL(removed);
    ASSERT_INT_EQ(0, *key);
    ASSERT_INT_EQ(10, *removed);
    ASSERT_NULL(list->first);
    ASSERT_NULL(list->last);
    ASSERT_INT_EQ(0, list->length);

    kfree(data);
    kfree(key);
}


TEST(remove_nonempty) {
    int *nums[2] = {
            allocate_int(10),
            allocate_int(20),
    };

    linked_list_prepend(list, nums[0]); // key = 0
    linked_list_prepend(list, nums[1]); // key = -1

    int *key = allocate_int(5);
    int *removed = linked_list_remove_head(list, key);
    ASSERT_NOT_NULL(removed);
    ASSERT_INT_EQ(-1, *key);
    ASSERT_INT_EQ(20, *removed);

    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_INT_EQ(1, list->length);
    ASSERT(first == last, "First and last nodes are not the same!");
    ASSERT_INT_EQ(10, *(int *) first->data);

    clear_ints(nums, 2);
}

TEST(remove_nonempty2) {
    int *nums[3] = {
            allocate_int(10),
            allocate_int(20),
            allocate_int(30)
    };

    linked_list_prepend(list, nums[0]); // key = 0
    linked_list_prepend(list, nums[1]); // key = -1
    linked_list_prepend(list, nums[2]); // key = -2

    int *key = allocate_int(5);
    int *removed = linked_list_remove_head(list, key);
    ASSERT_NOT_NULL(removed);
    ASSERT_INT_EQ(-2, *key);
    ASSERT_INT_EQ(30, *removed);

    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT_NOT_NULL(first);
    ASSERT_NOT_NULL(last);
    ASSERT_INT_EQ(2, list->length);
    ASSERT(first != last, "First and last nodes are the same!");
    ASSERT_INT_EQ(20, *(int *) first->data);
    ASSERT_INT_EQ(10, *(int *) last->data);

    ASSERT_NULL(first->prev);
    ASSERT_NULL(last->next);
    ASSERT(first->next == last, "First does not point to next!");
    ASSERT(last->prev == first, "Last does not point to first!");

    clear_ints(nums, 3);
}

TEST_SUITE(linked_list_tests) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);

    // list creation tests
    RUN_TEST(empty_list);

    // node creation tests
    RUN_TEST(node_creation);

    // append tests
    RUN_TEST(append_test1);
    RUN_TEST(append_test2);

    // insert tests
    RUN_TEST(insert_null_list);
    RUN_TEST(insert_empty_list);
    RUN_TEST(insert_front_singleton);
    RUN_TEST(insert_back_singleton);
    RUN_TEST(insert_front_nonempty);
    RUN_TEST(insert_back_nonempty);
    RUN_TEST(insert_front_nonempty_samekey);
    RUN_TEST(insert_middle);

    // remove tests
    RUN_TEST(remove_null_list);
    RUN_TEST(remove_empty_list);
    RUN_TEST(remove_singleton);
    RUN_TEST(remove_nonempty);
    RUN_TEST(remove_nonempty2);
}


//
// Concurrent Tests
// These are meant to show the linked list breaking with used from multiple threads.
//
void interleave_1_thread(void *args, unsigned long count) {
    // Hacky fix to fix unused param warning
    count = count;

    linked_list_insert(list, 0, args);
}

TEST(interleaving_1) {
    test_num = 0;

    // Setup yield positions
    yield_array[test_num][0] = 1;

    int *nums[2] = {
            allocate_int(10),
            allocate_int(20)
    };

    thread_fork("thread1", NULL, interleave_1_thread, nums[0], 0);
    thread_fork("thread2", NULL, interleave_1_thread, nums[1], 0);

    // TODO Wait for threads
//    clear_ints(nums, 2);
}


TEST_SUITE(concurrent_tests) {
    SUITE_CONFIGURE(&test_setup, &test_teardown);

    RUN_TEST(interleaving_1);
}

int linked_list_test_run(int nargs, char **args) {
    int testnum = 0;
    if (nargs == 2) {
        testnum = args[1][0] - '0'; // XXX - Hack - only works for testnum 0 -- 9
    }

    kprintf("%d", testnum);
    RESET_COUNTERS();
    RUN_SUITE(linked_list_tests);
    RUN_SUITE(concurrent_tests);
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
