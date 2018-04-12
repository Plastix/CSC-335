#include <linked_list.h>
#include <lib.h>
#include <test.h>
#include <testlib.h>
#include <thread.h>
#include <spl.h>

// Function signatures
int *allocate_int(int);

void clear_ints(int *nums[], int);

void print_list_test(Linked_List *list);

void insert_key_thread(void *, unsigned long);

void remove_head_thread(void *, unsigned long);

void wait(void);

//
// Helper Functions
//
int *allocate_int(int value) {
    int *pointer = kmalloc(sizeof(int));
    *pointer = value;
    return pointer;
}

void clear_ints(int *nums[], int cont) {
    for (int i = 0; i < cont; i++) {
        kfree(nums[i]);
    }
}

void print_list_test(Linked_List *list) {
    kprintf("list: ");
    linked_list_printlist(list, 0);
    int size = 0;
    if (list != NULL) {
        size = list->length;
    }
    kprintf("list size: %d\n", size);
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
void insert_key_thread(void *args, unsigned long count) {
    // Hacky fix to fix unused param warning
    count = count;

    // Disable interrupts for this current thread
    splhigh();

    linked_list_insert(list, 0, args);
}

void remove_head_thread(void *args, unsigned long count) {
    // Hacky fix to fix unused param warning
    count = count;
    args = args;

    // Disable interrupts for this current thread
    splhigh();

    int key = 0;
    linked_list_remove_head(list, &key);
}

void wait(){
    for (int i = 0; i < 100; ++i) {
        thread_yield();
    }
}

TEST(interleaving_1) {
    test_num = 1; // Set global test num

    int *nums[2] = {
            allocate_int(65), // 'A'
            allocate_int(66) // 'B'
    };

    kprintf("\n");
    print_list_test(list);

    thread_fork("thread1", NULL, insert_key_thread, nums[0], 1);
    thread_fork("thread2", NULL, insert_key_thread, nums[1], 1);
    kprintf("Added '%c' to list via Thread 1.\n", *nums[0]);
    kprintf("Added '%c' to list via Thread 2\n", *nums[1]);

    wait();
    print_list_test(list);
}

TEST(interleaving_2) {
    test_num = 2; // Set global test num

    int *nums[2] = {
            allocate_int(65), // 'A'
            allocate_int(66), // 'B'
    };

    linked_list_prepend(list, nums[0]); // key = 0
    kprintf("\n");
    print_list_test(list);

    thread_fork("thread1", NULL, insert_key_thread, nums[1], 0);
    thread_fork("thread2", NULL, remove_head_thread, NULL, 1);
    kprintf("Added '%c' to list via Thread 1...\n", *nums[1]);
    kprintf("Removed head via Thread 2...\n");

    wait();
    print_list_test(list);
}

int linked_list_test_run(int nargs, char **args) {
    int testnum = 0;
    if (nargs == 2) {
        testnum = args[1][0] - '0'; // XXX - Hack - only works for testnum 0 -- 9
    }

    RESET_COUNTERS();
    SUITE_CONFIGURE(&test_setup, &test_teardown);
    switch (testnum) {
        case 0:
            RUN_SUITE(linked_list_tests);
            break;
        case 1:
            RUN_TEST(interleaving_1);
            break;
        case 2:
            RUN_TEST(interleaving_2);
            break;
        default:
            kprintf("Unknown test number %d!", testnum);
    }

    TEST_REPORT();
    return 0;
}
