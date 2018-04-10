#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <types.h>

typedef struct Linked_List_Node Linked_List_Node;

struct Linked_List_Node {
    Linked_List_Node *prev;
    Linked_List_Node *next;
    int key;
    void *data;
};

typedef struct Linked_List Linked_List;

struct Linked_List {
    Linked_List_Node *first;
    Linked_List_Node *last;
    int length;
};

Linked_List *linked_list_create(void);

Linked_List_Node *linked_list_create_node(int key, void *data);

/*
 * Inserts the item at the front of the list.  If the item is not the
 * only item in the list, set its key to min - 1, where min is the
 * previous minimum key in the list.  If this is the first item, set
 * its key to 0.
 */
void linked_list_prepend(Linked_List *list, void *data);

/*
 * Prints the list.  Adds the int 'which' to the front of the output,
 * to aid in debugging.
 */
void linked_list_printlist(Linked_List *list, int which);

/*******************************
 * These are to be implemented *
 *******************************/

/*
 * Inserts into the linked-list so that the items are in order by
 * their keys.
 */
void linked_list_insert(Linked_List *list, int key, void *data);

/*
 * Removes the head node from the list.  key is set to the key of the
 * removed node.  The removed item is returned.
 *
 */
void *linked_list_remove_head(Linked_List *list, int *key);

//
// TEST CODE
//
void yield_if_should(int location);

void interleave_1_thread(void *args, unsigned long count);

#define CONCURRENT_TEST_NUM 4
#define CONCURRENT_LOCATION_NUM 10

static int yield_array[CONCURRENT_TEST_NUM][CONCURRENT_LOCATION_NUM] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static unsigned long test_num = 0;

#endif
