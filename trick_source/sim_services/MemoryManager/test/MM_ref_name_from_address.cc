
#include "MM_ref_name_from_address.hh"
#include "MM_test.hh"
#include "gtest/gtest.h"

// MM_sizeof_type

/*
 Test Fixture.
 */
class MM_ref_name_from_address : public ::testing::Test
{
protected:
    Trick::MemoryManager * memmgr;

    MM_ref_name_from_address()
    {
        memmgr = new Trick::MemoryManager;
    }

    ~MM_ref_name_from_address()
    {
        delete memmgr;
    }

    void SetUp() {}

    void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_ref_name_from_address, test1)
{
    double * dbl_p = (double *)memmgr->declare_var("double dbl_singleton");
    std::string ref_name = memmgr->ref_name_from_address(dbl_p);
    EXPECT_EQ("&dbl_singleton", ref_name);
}

TEST_F(MM_ref_name_from_address, test2)
{
    UDT2 * udt2_p1 = (UDT2 *)memmgr->declare_var("UDT2 udt2_var");
    std::string ref_name = memmgr->ref_name_from_address(udt2_p1);
    EXPECT_EQ("&udt2_var.udt1_var.x", ref_name);
}

TEST_F(MM_ref_name_from_address, test3)
{
    UDT2 * udt2_p1 = (UDT2 *)memmgr->declare_var("UDT2 udt2_var");
    std::string ref_name = memmgr->ref_name_from_address(&udt2_p1->udt1_var.udt_p);
    EXPECT_EQ("&udt2_var.udt1_var.udt_p", ref_name);
}

TEST_F(MM_ref_name_from_address, test4)
{
    UDT2 * udt2_p2 = (UDT2 *)memmgr->declare_var("UDT2 udt2_var2[3]");
    std::string ref_name = memmgr->ref_name_from_address(&(udt2_p2[1].udt1_var.udt_p));
    EXPECT_EQ("&udt2_var2[1].udt1_var.udt_p", ref_name);
}

#if __GNUC__ > 4
TEST_F(MM_ref_name_from_address, test5)
{
    int i;
    double d;
    UDT3 udt3_st = {i, false, d};
    UDT3 * udt3_p1 = &udt3_st;

    memmgr->declare_extern_var(udt3_p1, "UDT3 udt3_st");
    std::string ref_name = memmgr->ref_name_from_address(&(udt3_p1->bl));
    EXPECT_EQ("&udt3_st.bl", ref_name);
}

TEST_F(MM_ref_name_from_address, test6)
{
    int i;
    double d;
    bool b;
    UDT2 udt2_st;
    UDT3 udt3_st = {i, false, d};
    UDT4 udt4_st = {udt3_st, &b, udt2_st, 2.0};
    UDT4 * udt4_p1 = &udt4_st;

    memmgr->declare_extern_var(udt4_p1, "UDT4 udt4_st");
    std::string ref_name = memmgr->ref_name_from_address(udt4_p1);
    EXPECT_EQ("&udt4_st.udt3_ref", ref_name);
}

TEST_F(MM_ref_name_from_address, test7)
{
    int i;
    double d;
    bool b;
    UDT2 udt2_st;
    UDT3 udt3_st = {i, false, d};
    UDT4 udt4_st = {udt3_st, &b, udt2_st, 2.0};
    UDT4 * udt4_p1 = &udt4_st;

    memmgr->declare_extern_var(udt4_p1, "UDT4 udt4_st");
    std::string ref_name = memmgr->ref_name_from_address(&(udt4_p1->bl_p));
    EXPECT_EQ("&udt4_st.bl_p", ref_name);
}

TEST_F(MM_ref_name_from_address, test8)
{
    int i;
    double d;
    bool b;
    UDT2 udt2_st;
    UDT3 udt3_st = {i, false, d};
    UDT4 udt4_st = {udt3_st, &b, udt2_st, 2.0};
    UDT4 * udt4_p1 = &udt4_st;

    memmgr->declare_extern_var(udt4_p1, "UDT4 udt4_st");
    std::string ref_name = memmgr->ref_name_from_address(&(udt4_p1->dbl));
    EXPECT_EQ("&udt4_st.dbl", ref_name);
}
#endif

TEST_F(MM_ref_name_from_address, test9)
{
    UDT5 udt5_st;

    UDT5 * udt5_p1 = (UDT5 *)memmgr->declare_var("UDT5 udt5_var");
    std::string ref_name = memmgr->ref_name_from_address(&(udt5_p1->udt2_2d_arr[2][1].x));
    EXPECT_EQ("&udt5_var.udt2_2d_arr[2][1].x", ref_name);
}
