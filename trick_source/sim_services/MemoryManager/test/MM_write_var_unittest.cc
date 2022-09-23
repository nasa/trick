
#include <gtest/gtest.h>
#include "trick/MemoryManager.hh"
#include "MM_user_defined_types.hh"
#include "MM_test.hh"
#include <iostream>


/*
 Test Fixture.
 */
class MM_write_var : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_write_var() { memmgr = new Trick::MemoryManager; }
		~MM_write_var() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

#include <ctype.h>
#include <string>
int strcmp_IgnoringWhiteSpace(const char* s1, const char* s2) {
    int i1 = 0;
    int i2 = 0;

    while (1) {
        while ( !isgraph( s1[i1] ) && s1[i1] != '\0') { i1++; }
        while ( !isgraph( s2[i2] ) && s2[i2] != '\0') { i2++; }
        if ( s1[i1] == '\0' && s2[i2] == '\0') { return 0; }
        if ( s1[i1] != s2[i2]) {
            if (s1[i1] < s2[i2]) {
                return -1;
            } else {
                return 1;
            }
        }
        i1++; i2++;
    }
}

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_write_var, CharVarByAddr) {
    std::stringstream ss;
    char *test_var = (char *)memmgr->declare_var("char var_char");
    *test_var = 'a';
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_char = 'a';\n", ss.str());
}

TEST_F(MM_write_var, CharVarByName) {
    std::stringstream ss;
    char *test_var = (char *)memmgr->declare_var("char var_char");
    *test_var = 'a';
    memmgr->write_var(ss, "var_char");
    EXPECT_EQ("var_char = 'a';\n", ss.str());
}

TEST_F(MM_write_var, UnsignedCharVarByAddr) {
    std::stringstream ss;
    unsigned char *test_var = (unsigned char *)memmgr->declare_var("unsigned char var_uchar");
    *test_var = 65;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_uchar = 65;\n", ss.str());
}

TEST_F(MM_write_var, ShortVarByAddr) {
    std::stringstream ss;
    short *test_var = (short *)memmgr->declare_var("short var_short");
    *test_var = -35;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_short = -35;\n", ss.str());
}

TEST_F(MM_write_var, UnsignedShortVarByAddr) {
    std::stringstream ss;
    unsigned short *test_var = (unsigned short *)memmgr->declare_var("unsigned short var_ushort");
    *test_var = 35;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_ushort = 35;\n", ss.str());
}

TEST_F(MM_write_var, IntegerVarByAddr) {
    std::stringstream ss;
    int *test_var = (int *)memmgr->declare_var("int var_int");
    *test_var = -45;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_int = -45;\n", ss.str());
}

TEST_F(MM_write_var, UnsignedIntegerVarByAddr) {
    std::stringstream ss;
    int *test_var = (int *)memmgr->declare_var("unsigned int var_uint");
    *test_var = 35;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_uint = 35;\n", ss.str());
}

TEST_F(MM_write_var, LongVarByAddr) {
    std::stringstream ss;
    long *test_var = (long *)memmgr->declare_var("long var_long");
    *test_var = -65;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_long = -65;\n", ss.str());
}

TEST_F(MM_write_var, UnsignedLongVarByAddr) {
    std::stringstream ss;
    long *test_var = (long *)memmgr->declare_var("long var_ulong");
    *test_var = 65;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_ulong = 65;\n", ss.str());
}

TEST_F(MM_write_var, FloatVarByAddr) {
    std::stringstream ss;
    float *test_var = (float *)memmgr->declare_var("float var_float");
    *test_var = 3.141592653589793;
    memmgr->write_var(ss, (void*)test_var);
    // Expect a precision of 8.
    EXPECT_EQ("var_float = 3.1415927;\n", ss.str());
}

TEST_F(MM_write_var, DoubleVarByAddr) {
    std::stringstream ss;
    double *test_var = (double *)memmgr->declare_var("double var_double");
    *test_var = 3.141592653589793;
    memmgr->write_var(ss, (void*)test_var);
    // Expect a precision of 16.
    EXPECT_EQ("var_double = 3.141592653589793;\n", ss.str());
}

TEST_F(MM_write_var, LongLongVarByAddr) {
    std::stringstream ss;
    long long *test_var = (long long *)memmgr->declare_var("long long var_long_long ");
    *test_var = -65;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_long_long = -65;\n", ss.str());
}

TEST_F(MM_write_var, UnsignedLongLongVarByAddr) {
    std::stringstream ss;
    unsigned long long *test_var = (unsigned long long *)memmgr->declare_var("unsigned long long var_ulong_long ");
    *test_var = 65;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_ulong_long = 65;\n", ss.str());
}

TEST_F(MM_write_var, BooleanVarByAddr) {
    std::stringstream ss;
    bool* test_var = (bool*)memmgr->declare_var("bool var_bool");
    *test_var = true;
    memmgr->write_var(ss, (void*)test_var);
    EXPECT_EQ("var_bool = true;\n", ss.str());
}

TEST_F(MM_write_var, UserDefinedType1VarByAddr) {
    std::stringstream ss;
    UDT1* test_var = (UDT1*)memmgr->declare_var("UDT1 var_udt1");
    test_var->x = 1.23456789;
    test_var->y = 2.345678901;
    test_var->z = 3.456789012;
    memmgr->write_var(ss, (void*)test_var);

    int result = strcmp_IgnoringWhiteSpace(
        "var_udt1.x = 1.23456789;"
        "var_udt1.y = 2.345678901;"
        "var_udt1.z = 3.456789012;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_var, ArrayOfCharStringsVarByAddr) {
    std::stringstream ss;
    const char** test_var = (const char**)memmgr->declare_var("char* var_strings[3]");
    test_var[0] = "Huey";
    test_var[1] = "Dewey";
    test_var[2] = "Louie";
    memmgr->write_var(ss, (void*)test_var);

    int result = strcmp_IgnoringWhiteSpace(
        "var_strings = {\"Huey\",\"Dewey\",\"Louie\"};"
        , ss.str().c_str());
    EXPECT_EQ(result, 0);
}

//TEST_F(MM_write_var, WideCharacter) {
//    std::stringstream ss;
//    wchar_t* test_var = (wchar_t*)memmgr->declare_var("wchar_t var_wchar");
//    *test_var = true;
//    memmgr->write_var(ss, (void*)test_var);
//    EXPECT_EQ("var_wchar = true;\n", ss.str());
//}

TEST_F(MM_write_var, DoubleOneDimArrayVarByAddr) {
    std::stringstream ss;
    double *test_var = (double *)memmgr->declare_var("double var_double_array[5]");

    test_var[0] = 1.000000000000001;
    test_var[1] = 2.000000000000002;
    test_var[2] = 3.000000000000003;
    test_var[3] = 4.000000000000004;
    test_var[4] = 5.000000000000005;

    memmgr->write_var(ss, (void*)test_var);
    int result = strcmp_IgnoringWhiteSpace(
        "var_double_array = {1.000000000000001, 2.000000000000002, 3.000000000000003,"
        " 4.000000000000004, 5.000000000000005};",
        ss.str().c_str());
    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_var, DoubleTwoDimArrayVarByAddr) {
    std::stringstream ss;
    double (*test_var)[2][3] = (double (*)[2][3])memmgr->declare_var("double var_double_2d_array[2][3]");

    (*test_var)[0][0] = 1.000000000000001;
    (*test_var)[0][1] = 2.000000000000001;
    (*test_var)[0][2] = 3.000000000000001;
    (*test_var)[1][0] = 4.000000000000001;
    (*test_var)[1][1] = 5.000000000000001;
    (*test_var)[1][2] = 6.000000000000001;

    memmgr->write_var(ss, (void*)test_var);
    int result = strcmp_IgnoringWhiteSpace(
        "var_double_2d_array = {{1.000000000000001, 2.000000000000001, 3.000000000000001},"
                             " {4.000000000000001, 5.000000000000001, 6.000000000000001}};",
        ss.str().c_str());
    EXPECT_EQ(result, 0);
}


