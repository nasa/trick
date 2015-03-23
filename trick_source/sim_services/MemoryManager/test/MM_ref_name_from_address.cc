
#include "gtest/gtest.h"
#include "MM_test.hh"
#include "MM_ref_name_from_address.hh"

// MM_sizeof_type


/*
 Test Fixture.
 */
class MM_ref_name_from_address : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_ref_name_from_address() { memmgr = new Trick::MemoryManager; }
		~MM_ref_name_from_address() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_ref_name_from_address, test1) {

  double* dbl_p = (double*)memmgr->declare_var("double dbl_singleton");
  std::string ref_name = memmgr->ref_name_from_address(dbl_p);
  EXPECT_EQ("&dbl_singleton", ref_name);

}

TEST_F(MM_ref_name_from_address, test2) {

  UDT2* udt2_p1 = (UDT2*)memmgr->declare_var("UDT2 udt2_var");
  std::string ref_name = memmgr->ref_name_from_address(udt2_p1);
  EXPECT_EQ("&udt2_var.udt1_var.x", ref_name);

}

TEST_F(MM_ref_name_from_address, test3) {

  UDT2* udt2_p1 = (UDT2*)memmgr->declare_var("UDT2 udt2_var");
  std::string ref_name = memmgr->ref_name_from_address(&udt2_p1->udt1_var.udt_p);
  EXPECT_EQ("&udt2_var.udt1_var.udt_p", ref_name);

}

TEST_F(MM_ref_name_from_address, test4) {

  UDT2* udt2_p2 = (UDT2*)memmgr->declare_var("UDT2 udt2_var2[3]");
  std::string ref_name = memmgr->ref_name_from_address(&(udt2_p2[1].udt1_var.udt_p));
  EXPECT_EQ("&udt2_var2[1].udt1_var.udt_p", ref_name);

}
