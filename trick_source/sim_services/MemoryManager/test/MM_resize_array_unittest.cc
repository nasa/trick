
#include <gtest/gtest.h>
#include "trick/MemoryManager.hh"
#include "MM_user_defined_types.hh"
#include "MM_test.hh"
#include <iostream>


/*
 Test Fixture.
 */
class MM_resize_array : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_resize_array() { memmgr = new Trick::MemoryManager; }
		~MM_resize_array() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_resize_array, Three_args_with_address_Case1) {

        // Create a one dim array with 5 elements.
	double *original_array = (double *)memmgr->declare_var("double[5]");

        // Resize the array to have 8 elements.
        int cdims[1] = {8};
	double *resized_array = (double *)memmgr->resize_array(original_array, 1, cdims);

        ASSERT_TRUE(resized_array != NULL);
        validate_alloc_info(memmgr, resized_array, TRICK_DOUBLE, NULL, NULL, 8, 1, cdims);
}

TEST_F(MM_resize_array, Three_args_with_address_Case2) {

        // Create a 2x3 array.
	double *original_array = (double *)memmgr->declare_var("double[2][3]");

        // Resize the array to 5x7.
        int cdims[2] = {5,7};
	double *resized_array = (double *)memmgr->resize_array(original_array, 2, cdims);

        // Verify the alloc_info record for this  .
        ASSERT_TRUE(resized_array != NULL);
        validate_alloc_info(memmgr, resized_array, TRICK_DOUBLE, NULL, NULL, 35, 2, cdims);
}

TEST_F(MM_resize_array, Three_args_with_address_Case3) {

        // Create a 2x3 array.
	int (*original_array)[2][3] = (int(*)[2][3])memmgr->declare_var("int[2][3]");

        // Assign some values to it's elements.
        (*original_array)[0][0] = 1;
        (*original_array)[0][1] = 2;
        (*original_array)[0][2] = 3;
        (*original_array)[1][0] = 4;
        (*original_array)[1][1] = 5;
        (*original_array)[1][2] = 6;

        // Resize the array to 5x7.
        int cdims[2] = {5,7};
	int (*resized_array)[5][7] = (int(*)[5][7])memmgr->resize_array(original_array, 2, cdims);

        // Verify that the elements common to both arrays have the same values.
        EXPECT_EQ( (*resized_array)[0][0], 1);
        EXPECT_EQ( (*resized_array)[0][1], 2);
        EXPECT_EQ( (*resized_array)[0][2], 3);
        EXPECT_EQ( (*resized_array)[1][0], 4);
        EXPECT_EQ( (*resized_array)[1][1], 5);
        EXPECT_EQ( (*resized_array)[1][2], 6);
}

TEST_F(MM_resize_array, Three_args_with_name_Case1) {

        // Create a one dim array with 5 elements.
	(void) memmgr->declare_var("double Array_A[5]");

        // Resize the array to have 8 elements.
        int cdims[1] = {8};
	double *resized_array = (double *)memmgr->resize_array("Array_A", 1, cdims);

        ASSERT_TRUE(resized_array != NULL);
        validate_alloc_info(memmgr, resized_array, TRICK_DOUBLE, NULL, "Array_A", 8, 1, cdims);
}

TEST_F(MM_resize_array, Two_args_with_address_Case1) {

        // Create a one dim array with 5 elements.
	double *original_array = (double *)memmgr->declare_var("double[5]");

        // Resize the array to have 8 elements.
        int cdims[1] = {8};
	double *resized_array = (double *)memmgr->resize_array(original_array, 8);

        ASSERT_TRUE(resized_array != NULL);
        validate_alloc_info(memmgr, resized_array, TRICK_DOUBLE, NULL, NULL, 8, 1, cdims);
}

TEST_F(MM_resize_array, Two_args_with_name_Case1) {

        // Create a one dim array with 5 elements.
	(void) memmgr->declare_var("double Array_B[5]");

        // Resize the array to have 8 elements.
        int cdims[1] = {8};
	double *resized_array = (double *)memmgr->resize_array("Array_B", 8);

        ASSERT_TRUE(resized_array != NULL);
        validate_alloc_info(memmgr, resized_array, TRICK_DOUBLE, NULL, "Array_B", 8, 1, cdims);
}
