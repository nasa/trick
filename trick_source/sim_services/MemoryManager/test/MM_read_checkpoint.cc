
#include <gtest/gtest.h>
#include "trick/MemoryManager.hh"
#include "MM_user_defined_types.hh"
#include "MM_test.hh"
#include <iostream>
#include <iomanip>


/*
 Test Fixture.
 */
class MM_read_checkpoint : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_read_checkpoint() { memmgr = new Trick::MemoryManager; }
		~MM_read_checkpoint() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

void *make_array(size_t dsize, int n, int *dims, int cur) {
   int i;
   char** dary;
   if (cur == n-1) {
      return( calloc( dsize, dims[cur]));
   } else {
      dary = (char**)calloc( sizeof(char*),dims[cur]);
      for (i=0; i<dims[cur]; i++) {
         dary[i] = (char*)make_array( dsize,n,dims,cur+1);
      }
      return( dary);
   }
}

// ================================================================================
TEST_F(MM_read_checkpoint, string) {

    int result = 1;
    std::string *string_ptr;
    (void) memmgr->declare_extern_var(&string_ptr, "std::string* string_ptr");

    memmgr->read_checkpoint_from_string(
        "std::string trick_anon_local_0;"
        "trick_anon_local_0 = \"hello there!\";"
        "string_ptr = &trick_anon_local_0;"
    );

    result = string_ptr->compare("hello there!");
    EXPECT_EQ(0, result);
}

// ================================================================================
TEST_F(MM_read_checkpoint, array_of_double) {

        double* dbl_ptr;

        (void) memmgr->declare_extern_var(&dbl_ptr, "double* dbl_ptr");

        memmgr->read_checkpoint_from_string(
            "double trick_anon_local_0[10];"
            "trick_anon_local_0 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};"
            "dbl_ptr = &trick_anon_local_0;"
        );

        EXPECT_EQ( dbl_ptr[0], 1.0);
        EXPECT_EQ( dbl_ptr[1], 2.0);
        EXPECT_EQ( dbl_ptr[2], 3.0);
        EXPECT_EQ( dbl_ptr[3], 4.0);
        EXPECT_EQ( dbl_ptr[4], 5.0);
        EXPECT_EQ( dbl_ptr[5], 6.0);
        EXPECT_EQ( dbl_ptr[6], 7.0);
        EXPECT_EQ( dbl_ptr[7], 8.0);
        EXPECT_EQ( dbl_ptr[8], 9.0);
        EXPECT_EQ( dbl_ptr[9], 10.0);

}

// ================================================================================
TEST_F(MM_read_checkpoint, multi_array_of_double) {

        double* dbl_ptr;

        (void) memmgr->declare_extern_var(&dbl_ptr, "double* dbl_ptr");

        memmgr->read_checkpoint_from_string(
            "double trick_anon_local_0[2][2][2][2];"
            "trick_anon_local_0 = "
            "{"
            "    {"
            "        {"
            "            {1.0,2.0},"
            "            {3.0,4.0}"
            "        },"
            "        {"
            "            {5.0,6.0},"
            "            {7.0,8.0}"
            "        }"
            "    },"
            "    {"
            "        {"
            "            {9.0,10.0},"
            "            {11.0,12.0}"
            "        },"
            "        {"
            "            {13.0,14.0},"
            "            {15.0,16.0}"
            "        }"
            "    }"
            "};"
            "dbl_ptr = &trick_anon_local_0;"
        );

        EXPECT_EQ( dbl_ptr[0], 1.0);
        EXPECT_EQ( dbl_ptr[1], 2.0);
        EXPECT_EQ( dbl_ptr[2], 3.0);
        EXPECT_EQ( dbl_ptr[3], 4.0);

        EXPECT_EQ( dbl_ptr[4], 5.0);
        EXPECT_EQ( dbl_ptr[5], 6.0);
        EXPECT_EQ( dbl_ptr[6], 7.0);
        EXPECT_EQ( dbl_ptr[7], 8.0);

        EXPECT_EQ( dbl_ptr[8], 9.0);
        EXPECT_EQ( dbl_ptr[9], 10.0);
        EXPECT_EQ( dbl_ptr[10], 11.0);
        EXPECT_EQ( dbl_ptr[11], 12.0);

        EXPECT_EQ( dbl_ptr[12], 13.0);
        EXPECT_EQ( dbl_ptr[13], 14.0);
        EXPECT_EQ( dbl_ptr[14], 15.0);
        EXPECT_EQ( dbl_ptr[15], 16.0);
}

// ================================================================================
TEST_F(MM_read_checkpoint, multi_array_of_double2) {

        double (*dbl_ptr)[2][2][2][2];

        (void) memmgr->declare_extern_var(&dbl_ptr, "double* dbl_ptr");

        memmgr->read_checkpoint_from_string(
            "double trick_anon_local_0[2][2][2][2];"
            "trick_anon_local_0 = "
            "{"
            "    {"
            "        {"
            "            {1.0,2.0},"
            "            {3.0,4.0}"
            "        },"
            "        {"
            "            {5.0,6.0},"
            "            {7.0,8.0}"
            "        }"
            "    },"
            "    {"
            "        {"
            "            {9.0,10.0},"
            "            {11.0,12.0}"
            "        },"
            "        {"
            "            {13.0,14.0},"
            "            {15.0,16.0}"
            "        }"
            "    }"
            "};"
            "dbl_ptr = &trick_anon_local_0;"
        );

        EXPECT_EQ( (*dbl_ptr)[0][0][0][0], 1.0);
        EXPECT_EQ( (*dbl_ptr)[0][0][0][1], 2.0);
        EXPECT_EQ( (*dbl_ptr)[0][0][1][0], 3.0);
        EXPECT_EQ( (*dbl_ptr)[0][0][1][1], 4.0);

        EXPECT_EQ( (*dbl_ptr)[0][1][0][0], 5.0);
        EXPECT_EQ( (*dbl_ptr)[0][1][0][1], 6.0);
        EXPECT_EQ( (*dbl_ptr)[0][1][1][0], 7.0);
        EXPECT_EQ( (*dbl_ptr)[0][1][1][1], 8.0);

        EXPECT_EQ( (*dbl_ptr)[1][0][0][0], 9.0);
        EXPECT_EQ( (*dbl_ptr)[1][0][0][1], 10.0);
        EXPECT_EQ( (*dbl_ptr)[1][0][1][0], 11.0);
        EXPECT_EQ( (*dbl_ptr)[1][0][1][1], 12.0);

        EXPECT_EQ( (*dbl_ptr)[1][1][0][0], 13.0);
        EXPECT_EQ( (*dbl_ptr)[1][1][0][1], 14.0);
        EXPECT_EQ( (*dbl_ptr)[1][1][1][0], 15.0);
        EXPECT_EQ( (*dbl_ptr)[1][1][1][1], 16.0);
}

// ================================================================================
TEST_F(MM_read_checkpoint, hexfloat) {

        double *dbl_ptr;

        (void) memmgr->declare_extern_var(&dbl_ptr, "double* dbl_ptr");

        memmgr->read_checkpoint_from_string(
            "double dbl_singleton;"
            "dbl_singleton = 0g400921cac083126f;"
            "dbl_ptr = &dbl_singleton;"
        );

        EXPECT_EQ( (*dbl_ptr), 3.1415);
}

// ================================================================================
TEST_F(MM_read_checkpoint, enum_test) {

        FRUIT *fruit_ptr;

        (void) memmgr->declare_extern_var(&fruit_ptr, "FRUIT* fruit_ptr");

        memmgr->read_checkpoint_from_string(
            "FRUIT fruit_singleton;"
            "fruit_singleton = MANGO;"
            "fruit_ptr = &fruit_singleton;"
        );

        EXPECT_EQ( (*fruit_ptr), MANGO);
}
// ================================================================================
TEST_F(MM_read_checkpoint, bool_test) {

        bool T_flag;
        bool F_flag;

        (void) memmgr->declare_extern_var(&T_flag, "bool T_flag");
        (void) memmgr->declare_extern_var(&F_flag, "bool F_flag");

        memmgr->read_checkpoint_from_string(
            "T_flag = true;"
            "F_flag = false;"
            /* This is a C-style comment */
        );
        EXPECT_EQ( T_flag, true);
        EXPECT_EQ( F_flag, false);
}



TEST_F(MM_read_checkpoint, 2D_char_array) {

    UDT7 *udt7_p = (UDT7*)memmgr->declare_var("UDT7 udt7");

    memmgr->set_debug_level(1);
    memmgr->read_checkpoint_from_string(
    "udt7.A = {"
    "\"A00\",\"A01\",\"A02\",\"A03\",\"A04\",\"A05\",\"A06\",\"A07\","
    "\"A08\",\"A09\",\"A10\",\"A11\",\"A12\",\"A13\",\"A14\",\"A15\","
    "\"A16\",\"A17\",\"A18\",\"A19\",\"A20\",\"A21\",\"A22\",\"A23\","
    "\"A24\",\"A25\",\"A26\",\"A27\",\"A28\",\"A29\",\"A30\",\"A31\"};"
    );

    for (int ii=0; ii<32; ii++) {
        std::stringstream ss;
        ss << "A" ;
        ss << std::setfill('0') << std::setw(2) ;
        ss << ii ;
        std::cout << ss.str() << std::endl;
        int result = ss.str().compare( &udt7_p->A[ii][0] );

        EXPECT_EQ(result, 0);
    }




}
