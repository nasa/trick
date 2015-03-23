
#include <gtest/gtest.h>
#include "MM_test.hh"
#include "MM_user_defined_types.hh"

/*
 Test Fixture.
 */
class MM_declare_var_2 : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_declare_var_2() { memmgr = new Trick::MemoryManager; }
		~MM_declare_var_2() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/*
 This template function validates an ALLOC_INFO record created as a result
 of creating a variable using declare_var.
 */
template <typename T>
void validate_alloc_info_local(Trick::MemoryManager *memmgr,
                T *test_var,
                TRICK_TYPE trick_type,
                const char *type_name,
                const char *var_name,
                int num,
                int num_index,
                int *extent ) {

        ALLOC_INFO* alloc_info = NULL;
        alloc_info = memmgr->get_alloc_info_of(test_var);

        validate_alloc_info(alloc_info, test_var, trick_type, type_name, var_name, num, num_index, extent);

        EXPECT_EQ( alloc_info->stcl, TRICK_LOCAL) <<
                "Storage class (stcl) for a var created with var_declare should always be TRICK_LOCAL.";
}


/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_declare_var_2, Character) {
	char *test_var = (char *)memmgr->declare_var("char", 10);
        int extents[8] = {10,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_CHARACTER, NULL, NULL, 10, 1, extents);
}

TEST_F(MM_declare_var_2, UnsignedCharacter) {
	unsigned char *test_var = (unsigned char *)memmgr->declare_var("unsigned char", 9);
        int extents[8] = {9,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_CHARACTER, NULL, NULL, 9, 1, extents);
}

// One is not meant to create std:strings using declare_var.

TEST_F(MM_declare_var_2, Short) {
        short *test_var = (short *)memmgr->declare_var("short", 8);
        int extents[8] = {8,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_SHORT, NULL, NULL, 8, 1, extents);
}

TEST_F(MM_declare_var_2, UnsignedShort) {
        unsigned short *test_var = (unsigned short *)memmgr->declare_var("unsigned short", 7);
        int extents[8] = {7,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_SHORT, NULL, NULL, 7, 1, extents);
}

TEST_F(MM_declare_var_2, Integer) {
        int *test_var = (int *)memmgr->declare_var("int", 6);
        int extents[8] = {6,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_INTEGER, NULL, NULL, 6, 1, extents);
}

TEST_F(MM_declare_var_2, UnsignedInteger) {
        unsigned int *test_var = (unsigned int *)memmgr->declare_var("unsigned int", 5);
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_INTEGER, NULL, NULL, 5, 1, extents);
}

TEST_F(MM_declare_var_2, Long) {
        long *test_var = (long *)memmgr->declare_var("long",4);
        int extents[8] = {4,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_LONG, NULL, NULL, 4, 1, extents);
}

TEST_F(MM_declare_var_2, UnsignedLong) {
        unsigned long *test_var = (unsigned long *)memmgr->declare_var("unsigned long", 3);
        int extents[8] = {3,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_LONG, NULL, NULL, 3, 1, extents);
}

TEST_F(MM_declare_var_2, Float) {
        float *test_var = (float *)memmgr->declare_var("float", 5);
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_FLOAT, NULL, NULL, 5, 1, extents);
}

TEST_F(MM_declare_var_2, Double) {
        double *test_var = (double *)memmgr->declare_var("double", 7);
        int extents[8] = {7,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 7, 1, extents);
}

// Bitfields are tested elsewhere.

TEST_F(MM_declare_var_2, LongLong) {
        long long *test_var = (long long *)memmgr->declare_var("long long", 9);
        int extents[8] = {9,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_LONG_LONG, NULL, NULL, 9, 1, extents);
}

TEST_F(MM_declare_var_2, UnsignedLongLong) {
        unsigned long long *test_var = (unsigned long long *)memmgr->declare_var("unsigned long long", 11);
        int extents[8] = {11,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_UNSIGNED_LONG_LONG, NULL, NULL, 11, 1, extents);
}

// Hmmm. What to do with the silly TRICK_FILE_POINTER type.

TEST_F(MM_declare_var_2, Boolean) {
        bool *test_var = (bool *)memmgr->declare_var("bool", 13);
        int extents[8] = {13,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_BOOLEAN, NULL, NULL, 13, 1, extents);
}

TEST_F(MM_declare_var_2, WideCharacter) {
        wchar_t *test_var = (wchar_t *)memmgr->declare_var("wchar_t", 15);
        int extents[8] = {15,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_WCHAR, NULL, NULL, 15, 1, extents);
}


// Intrinsic Type Arrays

TEST_F(MM_declare_var_2, AnonOneDimDoubleArray) {
        double *test_var = (double *)memmgr->declare_var("double[5]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 5, 1, extents);
}

TEST_F(MM_declare_var_2, NamedOneDimDoubleArray) {
        double *test_var = (double *)memmgr->declare_var("double myarray[5]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, "myarray", 5, 1, extents);
}

TEST_F(MM_declare_var_2, AnonOneDimDoublePtrArray) {
        double **test_var = (double **)memmgr->declare_var("double*[5]");
        int extents[8] = {5,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 5, 2, extents);
}

TEST_F(MM_declare_var_2, AnonTwoDimDoubleArray) {
        double *test_var = (double *)memmgr->declare_var("double[3][4]");
        int extents[8] = {3,4,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 12, 2, extents);
}

TEST_F(MM_declare_var_2, AnonThreeDimDoubleArray) {
          double *test_var = (double *)memmgr->declare_var("double[2][3][4]");
          int extents[8] = {2,3,4,0,0,0,0,0};
          validate_alloc_info_local(memmgr, test_var, TRICK_DOUBLE, NULL, NULL, 24, 3, extents);
}

// User-defined types

TEST_F(MM_declare_var_2, UserDefinedSingleton) {
        UDT1 *test_var = (UDT1 *)memmgr->declare_var("UDT1");
        validate_alloc_info_local(memmgr, test_var, TRICK_STRUCTURED, "UDT1", NULL, 1, 0, NULL);
}

TEST_F(MM_declare_var_2, UserDefinedOneDimArray) {
        UDT1 *test_var = (UDT1 *)memmgr->declare_var("UDT1[3]");
        int extents[8] = {3,0,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_STRUCTURED, "UDT1", NULL, 3, 1, extents);
}

TEST_F(MM_declare_var_2, UserDefinedTwoDimArray) {
        UDT1 *test_var = (UDT1 *)memmgr->declare_var("UDT1[3][4]");
        int extents[8] = {3,4,0,0,0,0,0,0};
        validate_alloc_info_local(memmgr, test_var, TRICK_STRUCTURED, "UDT1", NULL, 12, 2, extents);
}

