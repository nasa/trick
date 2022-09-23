
#include <gtest/gtest.h>
#include "MM_test.hh"
#include "MM_user_defined_types.hh"


/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */
class MM_ref_attributes : public ::testing::Test {

	protected:
		Trick::MemoryManager *memmgr;

		MM_ref_attributes() {
			try {
				memmgr = new Trick::MemoryManager;
			} catch (std::logic_error e) {
				memmgr = NULL;
			}
		}

		~MM_ref_attributes() {
			delete memmgr;
		}

		void SetUp() {}
		void TearDown() {}
};

/*
   The tests.
 */
TEST_F(MM_ref_attributes, NormalCases) {
        REF2 *ref;
        UDT1  udt1;
        UDT2  udt2;
        UDT3  udt3;

        udt3.udt1_p = &udt1;
        udt3.udt2_p = &udt2;
        udt2.udt1_p = &udt1;

        UDT3* udt3_p = (UDT3*)memmgr->declare_extern_var(&udt3, "UDT3 udt3");
        ASSERT_TRUE(udt3_p != NULL);

        std::cout << "Case of: \"udt3\"" << std::endl;
        ref = memmgr->ref_attributes("udt3");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.X\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.X");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.X, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.Y\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.Y");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.Y, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.Z\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.Z");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.Z, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.I\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.I");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.I, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.M2\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.M2");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.M2, ref->address);

        ASSERT_TRUE(ref->attr != NULL);
        ASSERT_TRUE(ref->attr->units != NULL);
        std::cout << "Units of udt3.M2 = " << ref->attr->units << std::endl;

        free( ref);

        std::cout << "Case of: \"udt3.M2[2][3]\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.M2[2][3]");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.M2[2][3], ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.M3[1][1][3]\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.M3[1][1][3]");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.M3[1][1][3], ref->address);

        ASSERT_TRUE(ref->attr != NULL);
        ASSERT_TRUE(ref->attr->units != NULL);
        std::cout << "Units of udt3.M3 = " << ref->attr->units << std::endl;
        free( ref);

        std::cout << "Case of: \"udt3.C[5]\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.C[5]");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.C[5], ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.N.A\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.N.A");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.N.A, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.N.udt1.x\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.N.udt1.x");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.N.udt1.x, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.NA[1].udt1.x\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.NA[1].udt1.x");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.NA[1].udt1.x, ref->address);
        free( ref);

        // Some pointer dereferences
        std::cout << "Case of: \"udt3.udt1_p->x\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.udt1_p->x");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.udt1_p->x, ref->address);
        free( ref);

        std::cout << "Case of: \"udt3.udt2_p->udt1_p->y\"" << std::endl;
        ref = memmgr->ref_attributes("udt3.udt2_p->udt1_p->y");
        ASSERT_TRUE(ref != NULL);
        EXPECT_EQ( &udt3.udt2_p->udt1_p->y, ref->address);
        free( ref);

}

TEST_F(MM_ref_attributes, FailureCases) {
        REF2 *ref;
        UDT1  udt1;
        UDT2  udt2;
        UDT3  udt3;

        udt3.udt1_p = &udt1;
        udt3.udt2_p = &udt2;
        udt2.udt1_p = &udt1;

        UDT3* udt3_p = (UDT3*)memmgr->declare_extern_var(&udt3, "UDT3 udt3");
        ASSERT_TRUE(udt3_p != NULL);

        // Failure cases.
        ref = memmgr->ref_attributes("udt3.idontexist");
        ASSERT_TRUE(ref == NULL);

        std::cout << ISO_6429_White_Background
                  << ISO_6429_Blue_Foreground
                  << ISO_6429_Underline
                  << "NOTE: An error message is expected in this test."
                  << ISO_6429_Restore_Default
                  << std::endl;

        ref = memmgr->ref_attributes("udt3.M2[4][7]");
        ASSERT_TRUE(ref == NULL);

}
