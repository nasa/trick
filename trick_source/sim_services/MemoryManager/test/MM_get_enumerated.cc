
#include "gtest/gtest.h"
#include "MM_test.hh"
#include "MM_get_enumerated.hh"
#include "trick/vval.h"

// MM_sizeof_type


/*
 Test Fixture.
 */
class MM_get_enumerated : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_get_enumerated() { memmgr = new Trick::MemoryManager; }
		~MM_get_enumerated() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_get_enumerated, test1) {

  V_DATA v_data;
  int value;

  /* Create a varible instance of the MONTH enum type so that the enum values will
     be added to the enumeration map, and therefore be visable. */
  MONTH *enum_p = (MONTH*)memmgr->declare_var("MONTH enum_singleton");
  *enum_p = FEBRUARY;

  /* Verify that get_enumerated retrieves the values associated with each of the
     enumeration labels */
  value = -1;
  memmgr->get_enumerated("JANUARY", &v_data);
  value = vval_int( &v_data);
  EXPECT_EQ(value, 1);

  value = -1;
  memmgr->get_enumerated("MARCH", &v_data);
  value = vval_int( &v_data);
  EXPECT_EQ(value, 3);

  value = -1;
  memmgr->get_enumerated("MAY", &v_data);
  value = vval_int( &v_data);
  EXPECT_EQ(value, 5);

  value = -1;
  memmgr->get_enumerated("DECEMBER", &v_data);
  value = vval_int( &v_data);
  EXPECT_EQ(value, 12);

}
