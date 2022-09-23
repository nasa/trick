
#include "gtest/gtest.h"
#define private public
#include "MM_user_defined_types.hh"
#include "MM_test.hh"
#include <iostream>

/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */

class MM_clear_var : public ::testing::Test {

    protected:
        Trick::MemoryManager *memmgr;
        MM_clear_var() {
        try {
                memmgr = new Trick::MemoryManager;
            } catch (std::logic_error e) {
                memmgr = NULL;
            }
        }
        ~MM_clear_var() {
            delete memmgr;
        }
    void SetUp() {}
    void TearDown() {}
};

#define SMIDGE 0.000001

// =============================================================

TEST_F(MM_clear_var, test_char) {
    char* a_p = (char*)memmgr->declare_var("char a");
    *a_p = 'A';
    EXPECT_EQ(*a_p, 'A');

    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, '\0');
}

TEST_F(MM_clear_var, test_char_named) {
    char* a_p = (char*)memmgr->declare_var("char a");
    *a_p = 'A';
    EXPECT_EQ(*a_p, 'A');

    memmgr->clear_var("a");
    EXPECT_EQ(*a_p, '\0');
}

TEST_F(MM_clear_var, test_string) {

    int result = 1;
    std::string * string_p = (std::string *)memmgr->declare_var("std::string a");

    *string_p = "hello";
    result = string_p->compare("hello");
    EXPECT_EQ(0, result);

    memmgr->clear_var((void*)string_p );
    result = string_p->compare("");
    EXPECT_EQ(0, result);
}


TEST_F(MM_clear_var, test_bool) {
    bool* a_p = (bool*)memmgr->declare_var("bool a");
    *a_p = true;
    EXPECT_EQ(*a_p, true);

    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, false);
}

TEST_F(MM_clear_var, test_wchar_t) {
    wchar_t* a_p = (wchar_t*)memmgr->declare_var("wchar_t a");
    *a_p = L'A';
    EXPECT_EQ(*a_p, L'A');
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, L'\0');
}

TEST_F(MM_clear_var, test_short) {
    short* a_p = (short*)memmgr->declare_var("short a");
    *a_p = 17000;
    EXPECT_EQ(*a_p, 17000);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_int) {
    int* a_p = (int*)memmgr->declare_var("int a");
    *a_p = 12345678;
    EXPECT_EQ(*a_p, 12345678);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_enum) {
    int* a_p = (int*)memmgr->declare_var("FRUIT a");
    *a_p = NECTARINE;
    EXPECT_EQ(*a_p, NECTARINE);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_long) {
    long* a_p = (long*)memmgr->declare_var("long a");
    *a_p = 87654321;
    EXPECT_EQ(*a_p, 87654321);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_float) {
    float* a_p = (float*)memmgr->declare_var("float a");
    *a_p = 2.71828;
    EXPECT_NEAR(*a_p, 2.71828, SMIDGE);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_double) {
    float* a_p = (float*)memmgr->declare_var("double a");
    *a_p = 3.14159;
    EXPECT_NEAR(*a_p, 3.14159, SMIDGE);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_long_long) {
    float* a_p = (float*)memmgr->declare_var("long long a");
    *a_p = 12345678;
    EXPECT_EQ(*a_p, 12345678);
    memmgr->clear_var((void*)a_p );
    EXPECT_EQ(*a_p, 0);
}

TEST_F(MM_clear_var, test_bitfield) {
    FLAGS* flags_p = (FLAGS*)memmgr->declare_var("FLAGS flags");

    flags_p->a = 6;
    flags_p->b = 21;
    flags_p->c = 85;
    flags_p->d = 170;
    flags_p->e = 341;

    EXPECT_EQ(flags_p->a, 6);
    EXPECT_EQ(flags_p->b, 21);
    EXPECT_EQ(flags_p->c, 85);
    EXPECT_EQ(flags_p->d, 170);
    EXPECT_EQ(flags_p->e, 341);

    memmgr->clear_var((void*)flags_p );

    EXPECT_EQ(flags_p->a, 0);
    EXPECT_EQ(flags_p->b, 0);
    EXPECT_EQ(flags_p->c, 0);
    EXPECT_EQ(flags_p->d, 0);
    EXPECT_EQ(flags_p->e, 0);
}

TEST_F(MM_clear_var, test_composite_type) {

   // Allocatate a UDT1 object and call it udt1.
    UDT1 *udt1_p = (UDT1*)memmgr->declare_var("UDT1 udt1");
    udt1_p->x = 1.0;
    udt1_p->y = 2.0;
    udt1_p->z = 3.0;

   // Allocatate a UDT2 object and call it udt2.
    UDT2 *udt2_p = (UDT2*)memmgr->declare_var("UDT2 udt2");
    udt2_p->A = 4.0;
    udt2_p->B = 5.0;
    udt2_p->C = 6.0;
    udt2_p->ss = "hello";
    udt2_p->udt1.x = 7.0;
    udt2_p->udt1.y = 8.0;
    udt2_p->udt1.z = 9.0;
    udt2_p->udt1_p = udt1_p;

    // Clear the udt2 allocation.
    memmgr->clear_var( (void*)udt2_p );

    // Expect udt2 allocation to be cleared.
    EXPECT_NEAR(udt2_p->A, 0.0, SMIDGE) ;
    EXPECT_NEAR(udt2_p->B, 0.0, SMIDGE) ;
    EXPECT_NEAR(udt2_p->C, 0.0, SMIDGE) ;

    EXPECT_EQ(udt2_p->ss, (char*)NULL) ;

    EXPECT_NEAR(udt2_p->udt1.x, 0.0, SMIDGE) ;
    EXPECT_NEAR(udt2_p->udt1.y, 0.0, SMIDGE) ;
    EXPECT_NEAR(udt2_p->udt1.z, 0.0, SMIDGE) ;

    EXPECT_EQ(udt2_p->udt1_p, (UDT1*)NULL) ;

    // Expect udt1 allocation to be uneffected.
    EXPECT_NEAR(udt1_p->x, 1.0, SMIDGE) ;
    EXPECT_NEAR(udt1_p->y, 2.0, SMIDGE) ;
    EXPECT_NEAR(udt1_p->z, 3.0, SMIDGE) ;
}

TEST_F(MM_clear_var, 1d_array_of_structures) {

    UDT1 (*a)[3] = (UDT1(*)[3])memmgr->declare_var("UDT1 A[3]");

    (*a)[0].x = 1.0 ;
    (*a)[0].y = 2.0 ;
    (*a)[0].z = 3.0 ;
    (*a)[1].x = 4.0 ;
    (*a)[1].y = 5.0 ;
    (*a)[1].z = 6.0 ;
    (*a)[2].x = 7.0 ;
    (*a)[2].y = 8.0 ;
    (*a)[2].z = 9.0 ;

    // Clear the allocation.
    memmgr->clear_var( (void*)a );

    EXPECT_NEAR((*a)[0].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[1].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[2].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[2].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[2].z, 0.0, SMIDGE) ;
}

TEST_F(MM_clear_var, 2d_array_of_structures) {

    UDT1 (*a)[2][3] = (UDT1(*)[2][3])memmgr->declare_var("UDT1 A[2][3]");

    (*a)[0][0].x = 1 ;
    (*a)[0][0].y = 2 ;
    (*a)[0][0].z = 3 ;

    (*a)[0][1].x = 4 ;
    (*a)[0][1].y = 5 ;
    (*a)[0][1].z = 6 ;

    (*a)[0][2].x = 7 ;
    (*a)[0][2].y = 8 ;
    (*a)[0][2].z = 9 ;

    (*a)[1][0].x = 10;
    (*a)[1][0].y = 11;
    (*a)[1][0].z = 12;

    (*a)[1][1].x = 13;
    (*a)[1][1].y = 14 ;
    (*a)[1][1].z = 15 ;

    (*a)[1][2].x = 16 ;
    (*a)[1][2].y = 17 ;
    (*a)[1][2].z = 18 ;

    // Clear the allocation.
    memmgr->clear_var( (void*)a );

    EXPECT_NEAR((*a)[0][0].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0][0].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0][0].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[0][1].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0][1].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0][1].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[0][2].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0][2].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[0][2].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[1][0].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1][0].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1][0].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[1][1].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1][1].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1][1].z, 0.0, SMIDGE) ;

    EXPECT_NEAR((*a)[1][2].x, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1][2].y, 0.0, SMIDGE) ;
    EXPECT_NEAR((*a)[1][2].z, 0.0, SMIDGE) ;

}

TEST_F(MM_clear_var, all_vars) {

    int result = 1;

    // Create variables.
    char* a_p             = (char*)memmgr->declare_var("char a");
    std::string* string_p = (std::string *)memmgr->declare_var("std::string b");
    bool* c_p             = (bool*)memmgr->declare_var("bool c");
    wchar_t* d_p          = (wchar_t*)memmgr->declare_var("wchar_t d");

    // Initialize them.
    *a_p = 'A';
    EXPECT_EQ(*a_p, 'A');

    *string_p = "hello";
    result = string_p->compare("hello");
    EXPECT_EQ(0, result);

    *c_p = true;
    EXPECT_EQ(*c_p, true);

    *d_p = L'A';
    EXPECT_EQ(*d_p, L'A');

    // Try to clear them.
    memmgr->clear_all_vars();

    // Confirm that we cleared them.
    EXPECT_EQ(*a_p, '\0');

    result = string_p->compare("");
    EXPECT_EQ(0, result);

    EXPECT_EQ(*c_p, false);

    EXPECT_EQ(*d_p, L'\0');
}

