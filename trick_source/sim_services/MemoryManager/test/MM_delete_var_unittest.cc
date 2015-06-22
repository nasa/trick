
#include <gtest/gtest.h>
#include "trick/MemoryManager.hh"
#include "MM_test.hh"
#include <iostream>

/*
 Test Fixture.
 */
class MM_delete_var_unittest : public ::testing::Test {
    protected:
    Trick::MemoryManager *memmgr;
    MM_delete_var_unittest() { memmgr = new Trick::MemoryManager; }
    ~MM_delete_var_unittest() { delete memmgr;   }
    void SetUp() {}
    void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_delete_var_unittest, var_exists) {

    int exists;

    // Check whether a variable named "foo" exists.
    // It shouldn't exist because we haven't created it yet.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Create a variable named "foo".
    (void) memmgr->declare_var("double foo");

    // Check whether a variable named "foo" exists.
    // It should exist because we just created it.
    exists = memmgr->var_exists("foo");

    EXPECT_EQ(1, exists);

}

TEST_F(MM_delete_var_unittest, byAddress_destroy) {

    int exists;

    // Check whether a variable named "foo" exists. It shouldn't exist because we haven't created it yet.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Create a variable named "foo".
    double *dbl_p = (double*)memmgr->declare_var("double foo");

    // Show that the MemoryManager "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(1, exists);

    // Call delete_var with the address of the variable named "foo" and with the destroy flag set.
    memmgr->delete_var(dbl_p, 1);

    // Show that the MemoryManager no longer "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);
}

#if 0
TEST_F(MM_delete_var_unittest, byAddress_nodestroy) {

    int exists;

    // Check whether a variable named "foo" exists. It shouldn't exist because we haven't created it yet.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Create a variable named "foo".
    double *dbl_p = (double*)memmgr->declare_var("double foo");
    *dbl_p = 3.14;

    // Show that the MemoryManager "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(1, exists);

    // Call delete_var with the address of the variable named "foo" and with the destroy flag not set.
    memmgr->delete_var(dbl_p, 0);

    // Show that the MemoryManager no longer "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Show that the object still exists.
    EXPECT_NEAR(3.14, *dbl_p, 0.000001);
}
#endif

TEST_F(MM_delete_var_unittest, byName) {

    int exists;

    // Check whether a variable named "foo" exists. It shouldn't exist because we haven't created it yet.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Create a variable named "foo".
    (void) memmgr->declare_var("double foo");

    // Show that the MemoryManager "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(1, exists);

    // Call delete_var with the name of the variable.
    memmgr->delete_var("foo");

    // Show that the MemoryManager no longer "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);
}


TEST_F(MM_delete_var_unittest, externByAddress) {

    int exists;
    double dbl;

    // Check whether a variable named "foo" exists. It shouldn't exist because we haven't created it yet.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Create a variable named "foo".
    double *dbl_p = (double*)memmgr->declare_extern_var(&dbl, "double foo");

    // Show that the MemoryManager "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(1, exists);

    // Call delete_var with the address of the variable named "foo" and with the destroy flag set.
    memmgr->delete_extern_var(dbl_p);

    // Show that the MemoryManager no longer "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);
}


TEST_F(MM_delete_var_unittest, externByName) {

    int exists;
    double dbl;

    // Check whether a variable named "foo" exists. It shouldn't exist because we haven't created it yet.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);

    // Create a variable named "foo".
    (void) memmgr->declare_extern_var(&dbl, "double foo");

    // Show that the MemoryManager "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(1, exists);

    // Call delete_var with the address of the variable named "foo" and with the destroy flag set.
    memmgr->delete_extern_var("foo");

    // Show that the MemoryManager no longer "knows" about it.
    exists = memmgr->var_exists("foo");
    EXPECT_EQ(0, exists);
}


