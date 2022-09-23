
#include <gtest/gtest.h>
#include "trick/MemoryManager.hh"
#include "MM_user_defined_types.hh"
#include "MM_test.hh"
#include <iostream>


/*
 Test Fixture.
 */
class MM_strdup : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_strdup() { memmgr = new Trick::MemoryManager; }
		~MM_strdup() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_strdup, theTest) {

        const char* testString = "Fox in Sox";
	char* test_var = memmgr->mm_strdup( testString);

        ALLOC_INFO* alloc_info = memmgr->get_alloc_info_of(test_var);

        int extents[8] = {11,0,0,0,0,0,0,0};
        validate_alloc_info(alloc_info, test_var, TRICK_CHARACTER, NULL, NULL, 11, 1, extents);

        EXPECT_EQ( alloc_info->stcl, TRICK_LOCAL);

        // The contents of the original and the duplicated string should be equal.
        EXPECT_STREQ(testString, (char*)alloc_info->start);
}
