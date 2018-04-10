/*
 * Copyright (c) 2012 David Siñuela Pastor, siu.4coders@gmail.com
 * Modified and Ported to OS 161 by Aidan Pieper
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <lib.h>

/*  Maximum length of last message */
#define MINUNIT_MESSAGE_LEN (sizeof(char) * 1024)
#define STRING_SEPARATOR "########################################"

#define NULL ((void *)0)

/*  Misc. counters */
static int minunit_run = 0;
static int minunit_assert = 0;
static int minunit_fail = 0;
static int minunit_status = 0;

/*  Last message */
static char minunit_last_message[MINUNIT_MESSAGE_LEN];

/*  Test setup and teardown function pointers */
static void (*minunit_setup)(void) = NULL;

static void (*minunit_teardown)(void) = NULL;

/*  Definitions */
#define TEST(method_name) static void method_name(void)
#define TEST_SUITE(suite_name) static void suite_name(void)

#define _SAFE_BLOCK(block) do {\
    block\
} while(0)

/*  Run test suite and unset setup and teardown functions */
#define RUN_SUITE(suite_name) _SAFE_BLOCK(\
    kprintf("\n\n%s\nTest Suite: %s\n%s\n", STRING_SEPARATOR, #suite_name, STRING_SEPARATOR);\
    suite_name();\
    minunit_setup = NULL;\
    minunit_teardown = NULL;\
)

#define RESET_COUNTERS() _SAFE_BLOCK(\
    minunit_run = 0;\
    minunit_assert = 0;\
    minunit_fail = 0;\
    minunit_status = 0;\
)

/*  Configure setup and teardown functions */
#define SUITE_CONFIGURE(setup_fun, teardown_fun) _SAFE_BLOCK(\
    minunit_setup = setup_fun;\
    minunit_teardown = teardown_fun;\
)

/*  Test runner */
#define RUN_TEST(test) _SAFE_BLOCK(\
    if (minunit_setup) (*minunit_setup)();\
    minunit_status = 0;\
    kprintf("\nRunning test: %s ", #test);\
    test();\
    minunit_run++;\
    if (minunit_status) {\
        minunit_fail++;\
        kprintf("\n%s\n", minunit_last_message);\
    }\
    if (minunit_teardown) (*minunit_teardown)();\
)

/*  Report */
#define TEST_REPORT() _SAFE_BLOCK(\
    kprintf("\n\n%d tests, %d assertions, %d failures\n", minunit_run, minunit_assert, minunit_fail);\
)

/*  Assertions */
#define FAIL(message) _SAFE_BLOCK(\
    minunit_assert++;\
    snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "FAIL: %s\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
    minunit_status = 1;\
    return;\
)

#define ASSERT(test, message) _SAFE_BLOCK(\
    minunit_assert++;\
    if (!(test)) {\
        snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "FAIL: %s\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
        minunit_status = 1;\
        return;\
    } else {\
        kprintf(".");\
    }\
)

#define ASSERT_INT_EQ(expected, result) _SAFE_BLOCK(\
    int minunit_tmp_e = expected;\
    int minunit_tmp_r = result;\
    minunit_assert++;\
    if (minunit_tmp_e != minunit_tmp_r) {\
        snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "FAIL: %s\n\t%s:%d: %d expected but was %d", __func__, __FILE__, __LINE__, minunit_tmp_e, minunit_tmp_r);\
        minunit_status = 1;\
        return;\
    } else {\
        kprintf(".");\
    }\
)

#define ASSERT_STR_EQ(expected, result) _SAFE_BLOCK(\
    const char* minunit_tmp_e = expected;\
    const char* minunit_tmp_r = result;\
    minunit_assert++;\
    if (!minunit_tmp_e) {\
        minunit_tmp_e = "<null pointer>";\
    }\
    if (!minunit_tmp_r) {\
        minunit_tmp_r = "<null pointer>";\
    }\
    if(strcmp(minunit_tmp_e, minunit_tmp_r) == 0) {\
        snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "FAIL: %s\n\t%s:%d: '%s' expected but was '%s'", __func__, __FILE__, __LINE__, minunit_tmp_e, minunit_tmp_r);\
        minunit_status = 1;\
        return;\
    } else {\
        kprintf(".");\
    }\
)

#define ASSERT_NULL(ptr) ASSERT((ptr) == NULL, "Pointer is not null!")
#define ASSERT_NOT_NULL(ptr) ASSERT((ptr) != NULL, "Pointer is null!")