
#include <gtest/gtest.h>
#define private public
#include "MM_test.hh"
#include "MM_write_checkpoint.hh"
#include <iostream>


/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */
class MM_write_checkpoint_hexfloat : public ::testing::Test {

        protected:
                Trick::MemoryManager *memmgr;
                MM_write_checkpoint_hexfloat() {
                        try {
                                memmgr = new Trick::MemoryManager;
                        } catch (std::logic_error e) {
                                memmgr = NULL;
                        }
                }
                ~MM_write_checkpoint_hexfloat() {
                        delete memmgr;
                }
                void SetUp() {}
                void TearDown() {}
};

#include <ctype.h>
#include <string>
int strcmp_IgnoringWhiteSpace(const char* s1, const char* s2) {
    int i1 = 0;
    int i2 = 0;

    while (1) {
        while ( !isgraph( s1[i1] ) && s1[i1] != '\0') { i1++; }
        while ( !isgraph( s2[i2] ) && s2[i2] != '\0') { i2++; }
        if ( s1[i1] == '\0' && s2[i2] == '\0') { return 0; }
        if ( s1[i1] != s2[i2]) {
            if (s1[i1] < s2[i2]) {
                return -1;
            } else {
                return 1;
            }
        }
        i1++; i2++;
    }
}

// ================================================================================
TEST_F(MM_write_checkpoint_hexfloat, dbl_singleton) {

    // 1. Create the stream into which the checkpoint will be written.
    std::stringstream ss;

    // 2. Allocate and initialize a singleton double.
    double *dbl_p = (double*)memmgr->declare_var("double dbl_singleton");
    *dbl_p = 3.1415;

    // 3. Tell the MemoryManager that when checkpointing, floating point values
    // are to be represented as hexidecimal values (to preserve precision).
    memmgr->set_hexfloat_checkpoint(1);

    // 4. Write a checkpoint to the stream.
    memmgr->write_checkpoint( ss, "dbl_singleton");

    // 5. Verify that the stream contains the checkpoint that we are expecting.
    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double dbl_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_singleton = 0g400921cac083126f;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}


// ================================================================================
TEST_F(MM_write_checkpoint_hexfloat, dbl_array) {

    // 1. Create the stream into which the checkpoint will be written.
    std::stringstream ss;

    // 2. Allocate and initialize an array of doubles.
    double *dbl_p = (double*)memmgr->declare_var("double dbl_array[3]");
    dbl_p[0] = 1.1;
    dbl_p[1] = 2.2;
    dbl_p[2] = 3.3;

    // 3. Tell the MemoryManager that when checkpointing, floating point values
    // are to be represented as hexidecimal values (to preserve precision).
    memmgr->set_hexfloat_checkpoint(1);

    // 4. Write a checkpoint to the stream.
    memmgr->write_checkpoint( ss, "dbl_array");

    // 5. Verify that the stream contains the checkpoint that we are expecting.
    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double dbl_array[3];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_array = "
        "    {0g3ff199999999999a, 0g400199999999999a, 0g400a666666666666};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}
