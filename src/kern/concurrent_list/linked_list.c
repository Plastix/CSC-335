#include <linked_list.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>

Linked_List *linked_list_create(void) {
    Linked_List *ptr = kmalloc(sizeof(Linked_List));
    ptr->length = 0;
    ptr->first = NULL;
    ptr->last = NULL;

    ptr->lock = lock_create("Linked List Lock");
    if (ptr->lock == NULL) {
        kfree(ptr);
        return NULL;
    }

    return ptr;
}

Linked_List_Node *linked_list_create_node(int key, void *data) {
    Linked_List_Node *newnode = kmalloc(sizeof(Linked_List_Node));
    newnode->prev = NULL;
    newnode->next = NULL;
    newnode->key = key;

    newnode->data = data;

    return newnode;
}

void linked_list_prepend(Linked_List *list, void *data) {
    Linked_List_Node *newnode;

    lock_acquire(list->lock);
    Linked_List_Node *f = list->first;
    yield_on_test(4);

    if (list->first == NULL) {
        newnode = linked_list_create_node(0, data);
        list->first = newnode;
        list->last = newnode;
    } else {
        newnode = linked_list_create_node(f->key - 1, data);

        newnode->next = list->first;
        f->prev = newnode;
        list->first = newnode;
    }

    list->length++;

    lock_release(list->lock);
}

void linked_list_printlist(Linked_List *list, int which) {
    lock_acquire(list->lock);

    Linked_List_Node *runner = list->first;

    kprintf("%d: ", which);

    while (runner != NULL) {
        kprintf("%d[%c] ", runner->key, *((int *) runner->data));
        runner = runner->next;
    }

    kprintf("\n");

    lock_release(list->lock);
}

void linked_list_insert(Linked_List *list, int key, void *data) {
    // Don't insert into NULL lists
    if (list == NULL) {
        return;
    }

    lock_acquire(list->lock);

    Linked_List_Node *new = linked_list_create_node(key, data);
    Linked_List_Node *runner = list->first;
    if (runner == NULL) {
        yield_on_test(1);
        list->first = new;
        list->last = new;
    } else {

        while (runner != NULL && runner->key < key) {
            runner = runner->next;
        }

        // Insert new node before runner
        if (runner != NULL) {
            Linked_List_Node *prev = runner->prev;
            new->next = runner;
            yield_on_test(2);
            new->prev = prev;
            runner->prev = new;

            if (prev != NULL) {
                prev->next = new;
            }
            // Insert new node at tail of list
        } else {
            Linked_List_Node *last = list->last;
            new->next = NULL;
            new->prev = last;
            last->next = new;
        }

        if (new->prev == NULL) {
            list->first = new;
        }

        if (new->next == NULL) {
            list->last = new;
        }
    }

    list->length++;

    lock_release(list->lock);
}

void *linked_list_remove_head(Linked_List *list, int *key) {
    // Don't remove from NULL lists
    if (list == NULL) {
        return NULL;
    }

    lock_acquire(list->lock);

    Linked_List_Node *removed = list->first;
    if (removed == NULL) {
        lock_release(list->lock);
        return NULL;
    }

    Linked_List_Node *new_head = removed->next;
    yield_on_test(3);
    void *data = removed->data;
    *key = removed->key;

    list->first = new_head;

    if (new_head != NULL) {
        new_head->prev = NULL;
    }

    if (list->length <= 2) {
        list->last = new_head;
    }

    list->length--;

    lock_release(list->lock);

    kfree(removed);

    return data;
}

// Init Global Test var
long test_num = 0;

void yield_on_test(int test) {
    if (test_num == test) {
        thread_yield();
    }
}

/**
Questions:
1. What is the type of data in the Linked_List_Node struct?  Explain.

 The struct contains a pointer to the next node in the list as well as the previous node in the list. The struct also
 contains an integer value which is the key of the current node. Each struct also contains a pointer to some piece of
 data. This pointer is of type void* and can be thought of as a "generic pointer". It can be converted to another pointer
 type without an explicit cast. In order to dereference the pointer and get the data it must be cast to a complete
 data type first.

2. Explain the line typedef struct Linked_List Linked_List in linked_list.h.

  Typedefs are used to create type aliases. They are commonly used to reduce the amount of typing needed. This typedef
  aliases "Linked_List" to "struct Linked_List". Therefore, whenever you need a "struct Linked_List" you can just type
  "Linked_List." Structs can be thought of as lightweight objects so this allows a more Java-like syntax such as
  "Linked_List *list."

3. What does the function kmalloc used in linked_list.c do?  How do you do the analogous thing in Java?

 kmalloc is used to allocate memory of a specified size. This is OS161's version of "malloc" in the C standard library.
 In Java, this kind of memory allocation is handled for you whenever you instantiate a new object using the "new"
 keyword (e.g. "StringBuilder builder = new StringBuilder();). Additionally, in Java, memory dealocation is handled
 automatically by the garbage collector.

4. Boot the kernel and use the menu to figure out how to run the linked list test found in linked_list_test.c. Run it.
 Look at the source code for the kernel menu.  Trace the code between the point where the user enters the appropriate
 command in the menu and the point at which the test starts to run.  Give a brief description of this process.

  The file menu.c at src/kern/main/menu.c manages the user menu when the OS boots. Inside of menu.c, there is a
  hardcoded list called "cmdtable" which lists menu items and their respective function entry points.
  menu.c contains a function called "menu" which is a REPL. It prints out the list of menu options to the console and
  then reads in user input using "kgets." This 64 byte string is then passed on the function "menu_execute."
  "menu_execute" does some string checking and parsing and calls into "cmd_dispatch." This is the function which loops
  over the static "cmdtable" and checks to see if the string command matches any defined. If so, then the respective
  command function is called.

5. What does splhigh() do? Why is it called at the beginning of the two linked list test threads?  Try commenting it
 out, then rebuild and install the kernel, and then run the test again.  What happens?  You'll explore this further
 in the next part.

 "spl" stands for "set priority level" which allows the programmer to change how interrupts are handled by the kernel.
 splhigh() disables all interrupts on the current processor. If this is removed then all the test printout is messed up
 (interleaved) and the kernel panics and crashes. This is because the kernel is using timer-based interrupts to switch
 between the threads to perform multiprogramming. Without these interrupts, the first thread gains control of the
 processor and the second thread only starts running once the first thread finishes.
 */
