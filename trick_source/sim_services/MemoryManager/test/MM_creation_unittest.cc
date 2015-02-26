/*
 * $Id: MM_creation_unittest.cc 1741 2011-07-07 19:25:51Z wwoodard $
 */

#include <gtest/gtest.h>
#include "sim_services/MemoryManager/include/MemoryManager.hh"


TEST(MM_creation, NonNull) {

	Trick::MemoryManager *memmgr = NULL;

	try {
	   memmgr = new Trick::MemoryManager;
	} catch (std::logic_error e) {
           memmgr = NULL;
	}

	ASSERT_TRUE((void*)memmgr != NULL);

	delete memmgr;

}


TEST(MM_creation, Singleton){

	Trick::MemoryManager * memmgr;
	Trick::MemoryManager * disallowed_memmgr;

	try {
		memmgr = new Trick::MemoryManager;
	} catch (std::logic_error e) {
		memmgr = NULL;
	}

	EXPECT_TRUE((void*)memmgr != NULL);

	try {
		disallowed_memmgr = new Trick::MemoryManager();
	} catch (std::logic_error e) {
                disallowed_memmgr = NULL;
	}

	EXPECT_TRUE((void*)disallowed_memmgr == NULL);
}

