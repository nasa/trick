
#include <gtest/gtest.h>
#include "MM_test.hh"
#include "MM_user_defined_types.hh"

// MM_sizeof_type


/*
 Test Fixture.
 */
class MM_sizeof_type : public ::testing::Test {
	protected:
	Trick::MemoryManager *memmgr;
		MM_sizeof_type() { memmgr = new Trick::MemoryManager; }
		~MM_sizeof_type() { delete memmgr;   }
		void SetUp() {}
		void TearDown() {}
};

/* ================================================================================
                                      Test Cases
   ================================================================================
*/

TEST_F(MM_sizeof_type, Character) {
	size_t size = memmgr->sizeof_type("char");
        EXPECT_EQ(sizeof(char), size);
}

TEST_F(MM_sizeof_type, UnsignedCharacter) {
	size_t size = memmgr->sizeof_type("unsigned char");
        EXPECT_EQ(sizeof(unsigned char), size);
}

// One is not meant to create std:strings using declare_var.

TEST_F(MM_sizeof_type, Short) {
	size_t size = memmgr->sizeof_type("short");
        EXPECT_EQ(sizeof(short), size);
}

TEST_F(MM_sizeof_type, UnsignedShort) {
	size_t size = memmgr->sizeof_type("unsigned short");
        EXPECT_EQ(sizeof(unsigned short), size);
}

TEST_F(MM_sizeof_type, Integer) {
	size_t size = memmgr->sizeof_type("int");
        EXPECT_EQ(sizeof(int), size);
}

TEST_F(MM_sizeof_type, UnsignedInteger) {
	size_t size = memmgr->sizeof_type("unsigned int");
        EXPECT_EQ(sizeof(unsigned int), size);
}

TEST_F(MM_sizeof_type, Long) {
	size_t size = memmgr->sizeof_type("long");
        EXPECT_EQ(sizeof(long), size);
}

TEST_F(MM_sizeof_type, UnsignedLong) {
	size_t size = memmgr->sizeof_type("unsigned long");
        EXPECT_EQ(sizeof(unsigned long), size);
}

TEST_F(MM_sizeof_type, Float) {
	size_t size = memmgr->sizeof_type("float");
        EXPECT_EQ(sizeof(float), size);
}

TEST_F(MM_sizeof_type, Double) {
	size_t size = memmgr->sizeof_type("double");
        EXPECT_EQ(sizeof(double), size);
}

// Bitfields are tested elsewhere.

TEST_F(MM_sizeof_type, LongLong) {
	size_t size = memmgr->sizeof_type("long long");
        EXPECT_EQ(sizeof(long long), size);
}

TEST_F(MM_sizeof_type, UnsignedLongLong) {
	size_t size = memmgr->sizeof_type("unsigned long long");
        EXPECT_EQ(sizeof(long long), size);
}

// Hmmm. What to do with the silly TRICK_FILE_POINTER type.

TEST_F(MM_sizeof_type, Boolean) {
	size_t size = memmgr->sizeof_type("bool");
        EXPECT_EQ(sizeof(bool), size);
}

TEST_F(MM_sizeof_type, WideCharacter) {
	size_t size = memmgr->sizeof_type("wchar_t");
        EXPECT_EQ(sizeof(wchar_t), size);
}

// Intrinsic Type Arrays

TEST_F(MM_sizeof_type, OneDimDoubleArray) {
	size_t size = memmgr->sizeof_type("double[5]");
        EXPECT_EQ(sizeof(double)*5, size);
}

TEST_F(MM_sizeof_type, OneDimDoublePtrArray) {
	size_t size = memmgr->sizeof_type("double*[5]");
        EXPECT_EQ(sizeof(double*)*5, size);
}

TEST_F(MM_sizeof_type, TwoDimDoubleArray) {
	size_t size = memmgr->sizeof_type("double[3][4]");
        EXPECT_EQ(sizeof(double)*12, size);
}

TEST_F(MM_sizeof_type, ThreeDimDoubleArray) {
	size_t size = memmgr->sizeof_type("double[2][3][4]");
        EXPECT_EQ(sizeof(double)*24, size);
}

// User-defined types

TEST_F(MM_sizeof_type, UserDefinedSingleton) {
	size_t size = memmgr->sizeof_type("UDT1");
        EXPECT_EQ(sizeof(UDT1), size);
}

TEST_F(MM_sizeof_type, UserDefinedOneDimArray) {
	size_t size = memmgr->sizeof_type("UDT1[3]");
        EXPECT_EQ(sizeof(UDT1)*3, size);
}

TEST_F(MM_sizeof_type, UserDefinedTwoDimArray) {
	size_t size = memmgr->sizeof_type("UDT1[3][4]");
        EXPECT_EQ(sizeof(UDT1)*12, size);
}

