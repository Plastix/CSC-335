#include <linked_list.h>
#include <lib.h>
#include <test.h>
#include <testlib.h>

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
    CHECK(!list->first);
    CHECK(!list->length);
}

TEST(node_creation) {
    Linked_List_Node *node = linked_list_create_node(10, NULL);

    ASSERT_INT_EQ(10, node->key);
    CHECK(!node->next);
    CHECK(!node->prev);
    CHECK(!node->data);

    kfree(node);
}

TEST(append_test1) {
    int *num = allocate_int(10);
    linked_list_prepend(list, num);

    ASSERT_INT_EQ(1, list->length);
    Linked_List_Node *first = list->first;
    ASSERT_INT_EQ(*num, *(int *) first->data);

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
    ASSERT(first != NULL, "First node is null!");
    ASSERT_INT_EQ(*data, *(int *) first->data);
    ASSERT_INT_EQ(0, first->key);

    Linked_List_Node *last = list->last;
    ASSERT(first == last, "First and last nodes different");

    kfree(data);
}

TEST(insert_front_singleton) {
    int *one = allocate_int(5);
    linked_list_prepend(list, one);

    int *two = allocate_int(10);
    linked_list_insert(list, -1, two);

    ASSERT_INT_EQ(2, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
    ASSERT(first != NULL, "First node is null!");
    ASSERT(last != NULL, "Last node is null!");
    ASSERT(first != last, "First and last nodes are the same!");
    ASSERT_INT_EQ(*two, *(int *) first->data);
    ASSERT_INT_EQ(*one, *(int *) last->data);
    ASSERT_INT_EQ(-1, first->key);

    kfree(one);
    kfree(two);
}

TEST(insert_back_singleton) {
    int *one = allocate_int(5);
    linked_list_prepend(list, one);

    int *two = allocate_int(10);
    linked_list_insert(list, 1, two);

    ASSERT_INT_EQ(2, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *last = list->last;
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

    linked_list_prepend(list, nums[0]); // 0
    linked_list_prepend(list, nums[1]); // -1

    linked_list_insert(list, -2, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *first = list->first;
    ASSERT_INT_EQ(*nums[2], *(int *) first->data);

    clear_ints(nums, 3);
}

TEST(insert_front_nonempty_samekey) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // 0
    linked_list_prepend(list, nums[1]); // -1

    linked_list_insert(list, -1, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *first = list->first;
    ASSERT_INT_EQ(*nums[2], *(int *) first->data);

    clear_ints(nums, 3);
}


TEST(insert_back_nonempty) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // 0
    linked_list_prepend(list, nums[1]); // -1

    linked_list_insert(list, 1, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *last = list->last;
    ASSERT_INT_EQ(*nums[2], *(int *) last->data);

    clear_ints(nums, 3);
}

TEST(insert_middle) {
    int *nums[3] = {
            allocate_int(1),
            allocate_int(2),
            allocate_int(3),
    };

    linked_list_prepend(list, nums[0]); // 0
    linked_list_prepend(list, nums[1]); // -1

    linked_list_insert(list, 0, nums[2]);

    ASSERT_INT_EQ(3, list->length);
    Linked_List_Node *first = list->first;
    Linked_List_Node *second = first->next;
    ASSERT_INT_EQ(*nums[2], *(int *) second->data);

    clear_ints(nums, 3);
}

TEST(remove_null_list) {
    int *key = allocate_int(5);
    Linked_List_Node *removed = linked_list_remove_head(NULL, key);
    ASSERT(removed == NULL, "Removed node is non-null!");
    ASSERT_INT_EQ(5, *key);
    kfree(key);
}

TEST(remove_empty_list) {
    int *key = allocate_int(5);
    Linked_List_Node *removed = linked_list_remove_head(list, key);
    ASSERT(removed == NULL, "Removed node is non-null!");
    ASSERT_INT_EQ(5, *key);
    kfree(key);
}

TEST(remove_singleton) {
    int *data = allocate_int(10);
    linked_list_prepend(list, data); // 0

    int *key = allocate_int(5);
    Linked_List_Node *removed = linked_list_remove_head(list, key);
    ASSERT(removed != NULL, "Removed node is null!");
    ASSERT_INT_EQ(0, *key);
    ASSERT_INT_EQ(10, *(int *) removed->data);
    ASSERT_INT_EQ(0, removed->key);
    CHECK(!list->first);
    ASSERT_INT_EQ(0, list->length);

    kfree(data);
    kfree(key);
}


TEST(remove_nonempty) {
    int *nums[2] = {
            allocate_int(10),
            allocate_int(20),
    };

    linked_list_prepend(list, nums[0]); // 0
    linked_list_prepend(list, nums[1]); // -1

    int *key = allocate_int(5);
    Linked_List_Node *removed = linked_list_remove_head(list, key);
    ASSERT(removed != NULL, "Removed node is null!");
    ASSERT_INT_EQ(-1, *key);
    ASSERT_INT_EQ(20, *(int *) removed->data);
    ASSERT_INT_EQ(-1, removed->key);
    CHECK(list->first);
    ASSERT_INT_EQ(1, list->length);

    clear_ints(nums, 2);
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
}


int linked_list_test_run(int nargs, char **args) {
    int testnum = 0;
    if (nargs == 2) {
        testnum = args[1][0] - '0'; // XXX - Hack - only works for testnum 0 -- 9
    }

    kprintf("Running Test Suite: %d\n", testnum);
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
