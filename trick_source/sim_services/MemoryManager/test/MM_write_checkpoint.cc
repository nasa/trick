
#include "gtest/gtest.h"
#define private public
#include "MM_test.hh"
#include "MM_write_checkpoint.hh"
#include <iostream>


/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */
class MM_write_checkpoint : public ::testing::Test {

        protected:
                Trick::MemoryManager *memmgr;
                MM_write_checkpoint() {
                        try {
                                memmgr = new Trick::MemoryManager;
                        } catch (std::logic_error e) {
                                memmgr = NULL;
                        }
                }
                ~MM_write_checkpoint() {
                        delete memmgr;
                }
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

// ================================================================================
TEST_F(MM_write_checkpoint, string_singleton) {

    std::stringstream ss;

    std::string *string_p = (std::string*)memmgr->declare_var("std::string string_singleton");
    *string_p = "string_singleton_test";

    memmgr->write_checkpoint( ss, "string_singleton");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "std::string string_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "string_singleton = \"string_singleton_test\";"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_singleton) {

    std::stringstream ss;

    double *dbl_p = (double*)memmgr->declare_var("double dbl_singleton");
    *dbl_p = 3.1415;

    memmgr->write_checkpoint( ss, "dbl_singleton");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double dbl_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_singleton = 3.1415;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}


// ================================================================================
TEST_F(MM_write_checkpoint, bool_singleton) {

    std::stringstream ss;

    bool *bool_p = (bool*)memmgr->declare_var("bool bool_singleton");
    *bool_p = true;

    //memmgr->write_checkpoint( std::cout, "bool_singleton");
    memmgr->write_checkpoint( ss, "bool_singleton");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "bool bool_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "bool_singleton = true;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_pointer) {

    std::stringstream ss;

    double **dbl_pp = (double**)memmgr->declare_var("double *dbl_pointer");
    double *dbl_p = (double*)memmgr->declare_var("double dbl_array[2]");

    *dbl_pp = dbl_p;
    dbl_p[0] = 1.0;
    dbl_p[1] = 2.0;

    memmgr->write_checkpoint( ss, "dbl_pointer");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double* dbl_pointer;"
        "double dbl_array[2];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_pointer = &dbl_array[0];"
        "dbl_array = {1, 2};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, bool_pointer) {

    std::stringstream ss;

    bool **bool_pp = (bool**)memmgr->declare_var("bool *bool_pointer");
    bool *bool_p = (bool*)memmgr->declare_var("bool bool_array[2]");

    *bool_pp = bool_p;
    bool_p[0] = true;
    bool_p[1] = false;

    memmgr->write_checkpoint( ss, "bool_pointer");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "bool* bool_pointer;"
        "bool bool_array[2];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "bool_pointer = &bool_array[0];"
        "bool_array = {true, false};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_array) {

    std::stringstream ss;

    double *dbl_p = (double*)memmgr->declare_var("double dbl_array[3]");

    dbl_p[0] = 1.1;
    dbl_p[1] = 2.2;
    dbl_p[2] = 3.3;

    memmgr->write_checkpoint( ss, "dbl_array");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double dbl_array[3];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_array = "
        "    {1.1, 2.2, 3.3};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_array_expanded) {

    std::stringstream ss;

    double *dbl_p = (double*)memmgr->declare_var("double dbl_array[3]");

    dbl_p[0] = 1.1;
    dbl_p[1] = 2.2;
    dbl_p[2] = 3.3;

    memmgr->set_expanded_arrays(true);
    memmgr->write_checkpoint( ss, "dbl_array");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double dbl_array[3];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_array[0] = 1.1;"
        "dbl_array[1] = 2.2;"
        "dbl_array[2] = 3.3;"

        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_array_pointers) {

    std::stringstream ss;

    double **dbl_pa = (double**)memmgr->declare_var("double *dbl_array_ptrs[3]");

    double *dbl_p1 = (double*)memmgr->declare_var("double dbl1");
    double *dbl_p2 = (double*)memmgr->declare_var("double dbl2");
    double *dbl_p3 = (double*)memmgr->declare_var("double dbl3");

    dbl_pa[0] = dbl_p1;
    dbl_pa[1] = dbl_p2;
    dbl_pa[2] = dbl_p3;

    memmgr->write_checkpoint( ss, "dbl_array_ptrs");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double* dbl_array_ptrs[3];"
        "double dbl1;"
        "double dbl2;"
        "double dbl3;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_array_ptrs = "
        "    {&dbl1, &dbl2, &dbl3};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_array_pointers_expanded) {

    std::stringstream ss;

    double **dbl_pa = (double**)memmgr->declare_var("double *dbl_array_ptrs[3]");

    double *dbl_p1 = (double*)memmgr->declare_var("double dbl1");
    double *dbl_p2 = (double*)memmgr->declare_var("double dbl2");
    double *dbl_p3 = (double*)memmgr->declare_var("double dbl3");

    dbl_pa[0] = dbl_p1;
    dbl_pa[1] = dbl_p2;
    dbl_pa[2] = dbl_p3;

    memmgr->set_expanded_arrays(true);
    memmgr->write_checkpoint( ss, "dbl_array_ptrs");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double* dbl_array_ptrs[3];"
        "double dbl1;"
        "double dbl2;"
        "double dbl3;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "dbl_array_ptrs[0] = &dbl1;"
        "dbl_array_ptrs[1] = &dbl2;"
        "dbl_array_ptrs[2] = &dbl3;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_2d_constrained_array) {

    std::stringstream ss;

    double (*A)[3][4] = (double(*)[3][4])memmgr->declare_var("double A[3][4]");

    (*A)[0][0] = 0.0;
    (*A)[0][1] = 1.0;
    (*A)[0][2] = 2.0;
    (*A)[0][3] = 3.0;
    (*A)[1][0] = 10.0;
    (*A)[1][1] = 11.0;
    (*A)[1][2] = 12.0;
    (*A)[1][3] = 13.0;
    (*A)[2][0] = 20.0;
    (*A)[2][1] = 21.0;
    (*A)[2][2] = 22.0;
    (*A)[2][3] = 23.0;

    memmgr->write_checkpoint( ss, "A");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double A[3][4];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "A = "
        "    {"
        "        {0, 1, 2, 3},"
        "        {10, 11, 12, 13},"
        "        {20, 21, 22, 23}"
        "    };"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, dbl_2d_constrained_array_expanded) {

    std::stringstream ss;

    double (*A)[3][4] = (double(*)[3][4])memmgr->declare_var("double A[3][4]");

    (*A)[0][0] = 0.0;
    (*A)[0][1] = 1.0;
    (*A)[0][2] = 2.0;
    (*A)[0][3] = 3.0;
    (*A)[1][0] = 10.0;
    (*A)[1][1] = 11.0;
    (*A)[1][2] = 12.0;
    (*A)[1][3] = 13.0;
    (*A)[2][0] = 20.0;
    (*A)[2][1] = 21.0;
    (*A)[2][2] = 22.0;
    (*A)[2][3] = 23.0;

    memmgr->set_expanded_arrays(true);
    memmgr->write_checkpoint( ss, "A");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "double A[3][4];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "A[0][1] = 1;"
        "A[0][2] = 2;"
        "A[0][3] = 3;"
        "A[1][0] = 10;"
        "A[1][1] = 11;"
        "A[1][2] = 12;"
        "A[1][3] = 13;"
        "A[2][0] = 20;"
        "A[2][1] = 21;"
        "A[2][2] = 22;"
        "A[2][3] = 23;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, enum_singleton) {

    std::stringstream ss;

//    memmgr->set_debug_level(2);

    MONTH *enum_p = (MONTH*)memmgr->declare_var("MONTH enum_singleton");
    *enum_p = FEBRUARY;

    memmgr->write_checkpoint( ss, "enum_singleton");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "MONTH enum_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "enum_singleton = FEBRUARY;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, enum_pointer) {

    std::stringstream ss;

    MONTH **enum_pp = (MONTH**)memmgr->declare_var("MONTH *enum_pointer");
    MONTH *enum_p = (MONTH*)memmgr->declare_var("MONTH enum_array[2]");

    *enum_pp = enum_p;
    enum_p[0] = MARCH; 
    enum_p[1] = APRIL;

    memmgr->write_checkpoint( ss, "enum_pointer");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "MONTH* enum_pointer;"
        "MONTH enum_array[2];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "enum_pointer = &enum_array[0];"
        "enum_array = "
        "    {MARCH, APRIL};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, enum_array) {

    std::stringstream ss;

    MONTH *enum_p = (MONTH*)memmgr->declare_var("MONTH enum_array[3]");

    enum_p[0] = JUNE;
    enum_p[1] = JULY;
    enum_p[2] = AUGUST;

    memmgr->write_checkpoint( ss, "enum_array");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "MONTH enum_array[3];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "enum_array = "
        "    {JUNE, JULY, AUGUST};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, enum_array_pointers) {

    std::stringstream ss;

    MONTH **enum_pa = (MONTH**)memmgr->declare_var("MONTH *enum_array_ptrs[3]");

    MONTH *enum_p1 = (MONTH*)memmgr->declare_var("MONTH enum1");
    MONTH *enum_p2 = (MONTH*)memmgr->declare_var("MONTH enum2");
    MONTH *enum_p3 = (MONTH*)memmgr->declare_var("MONTH enum3");

    enum_pa[0] = enum_p1;
    enum_pa[1] = enum_p2;
    enum_pa[2] = enum_p3;

    memmgr->write_checkpoint( ss, "enum_array_ptrs");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "MONTH* enum_array_ptrs[3];"
        "MONTH enum1;"
        "MONTH enum2;"
        "MONTH enum3;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "enum_array_ptrs = "
        "    {&enum1, &enum2, &enum3};"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, udt_singleton_no_deps) {

    std::stringstream ss;

    UDT1 *udt_p = (UDT1*)memmgr->declare_var("UDT1 udt_singleton");

    udt_p->x = 3.1415;
    udt_p->udt_p = NULL;
    udt_p->dbl_p = NULL;

    memmgr->write_checkpoint( ss, "udt_singleton");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "UDT1 udt_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt_singleton.x = 3.1415;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, udt_singleton_self_referential) {

    std::stringstream ss;

    UDT1 *udt_p = (UDT1*)memmgr->declare_var("UDT1 udt_singleton");

    udt_p->x = 3.1415;
    udt_p->udt_p = udt_p;
    udt_p->dbl_p = NULL;

    memmgr->write_checkpoint( ss, "udt_singleton");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "UDT1 udt_singleton;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt_singleton.x = 3.1415;"
        "udt_singleton.udt_p = &udt_singleton.x;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, udt_singleton_deps) {

    std::stringstream ss;

    UDT1 *udt1_p = (UDT1*)memmgr->declare_var("UDT1 udt1");
    UDT1 *udt2_p = (UDT1*)memmgr->declare_var("UDT1 udt2");
    double *dbl1_p = (double*)memmgr->declare_var("double dbl1");

    udt1_p->x = 3.1415;
    udt1_p->udt_p = udt2_p;
    udt1_p->dbl_p = dbl1_p;

    memmgr->write_checkpoint( ss, "udt1");

    int result = strcmp_IgnoringWhiteSpace(
        ""
        "// Variable Declarations."
        "UDT1 udt1;"
        "UDT1 udt2;"
        "double dbl1;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt1.x = 3.1415;"
        "udt1.udt_p = &udt2.x;"
        "udt1.dbl_p = &dbl1;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, udt_pointer) {

    std::stringstream ss;

    UDT1 **udt1_p = (UDT1**)memmgr->declare_var("UDT1 *udt_p");
    UDT1  *a1 = (UDT1 *)memmgr->declare_var("UDT1 bbb");

    a1->x = 3.1415;
    a1->udt_p = NULL;

    *udt1_p = a1;

    memmgr->write_checkpoint( ss, "udt_p");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "UDT1* udt_p;"
        "UDT1 bbb;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt_p = &bbb;"
        "bbb.x = 3.1415;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, udt_array_no_deps) {

    std::stringstream ss;

    UDT1 *udt_p = (UDT1*)memmgr->declare_var("UDT1 udt_array[3]");

    udt_p[0].x = 3.1415;
    udt_p[0].udt_p = NULL;
    udt_p[0].dbl_p = NULL;

    udt_p[1].x = 3.1415;
    udt_p[1].udt_p = NULL;
    udt_p[1].dbl_p = NULL;

    udt_p[2].x = 3.1415;
    udt_p[2].udt_p = NULL;
    udt_p[2].dbl_p = NULL;

    memmgr->write_checkpoint( ss, "udt_array");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "UDT1 udt_array[3];"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt_array[0].x = 3.1415;"
        "udt_array[1].x = 3.1415;"
        "udt_array[2].x = 3.1415;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, udt_array_deps) {

    std::stringstream ss;

    UDT1 *udt_p = (UDT1*)memmgr->declare_var("UDT1 udt_array[3]");
    UDT1 *udt1_p = (UDT1*)memmgr->declare_var("UDT1 udt1");
    UDT1 *udt2_p = (UDT1*)memmgr->declare_var("UDT1 udt2");
    UDT1 *udt3_p = (UDT1*)memmgr->declare_var("UDT1 udt3");
    double *dbl1_p = (double*)memmgr->declare_var("double dbl1");
    double *dbl2_p = (double*)memmgr->declare_var("double dbl2");
    double *dbl3_p = (double*)memmgr->declare_var("double dbl3");

    udt_p[0].x = 3.1415;
    udt_p[0].udt_p = udt1_p;
    udt_p[0].dbl_p = dbl1_p;

    udt_p[1].x = 3.1415;
    udt_p[1].udt_p = udt2_p;
    udt_p[1].dbl_p = dbl2_p;

    udt_p[2].x = 3.1415;
    udt_p[2].udt_p = udt3_p;
    udt_p[2].dbl_p = dbl3_p;

    memmgr->write_checkpoint( ss, "udt_array");

    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "UDT1 udt_array[3];"
        "UDT1 udt1;"
        "double dbl1;"
        "UDT1 udt2;"
        "double dbl2;"
        "UDT1 udt3;"
        "double dbl3;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt_array[0].x = 3.1415;"
        "udt_array[0].udt_p = &udt1.x;"
        "udt_array[0].dbl_p = &dbl1;"
        "udt_array[1].x = 3.1415;"
        "udt_array[1].udt_p = &udt2.x;"
        "udt_array[1].dbl_p = &dbl2;"
        "udt_array[2].x = 3.1415;"
        "udt_array[2].udt_p = &udt3.x;"
        "udt_array[2].dbl_p = &dbl3;"
        , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

// ================================================================================
TEST_F(MM_write_checkpoint, vector_of_named_vars) {

    std::stringstream ss;
    std::vector<const char*> name_list;

    UDT1  *a1 = (UDT1 *)memmgr->declare_var("UDT1 A1");
    UDT1  *a2 = (UDT1 *)memmgr->declare_var("UDT1 A2");
    UDT1  *a3 = (UDT1 *)memmgr->declare_var("UDT1 A3");
    UDT1  *a4 = (UDT1 *)memmgr->declare_var("UDT1 A4");

    a1->x = 3.1415;
    a1->udt_p = a3;
    double *a5 = (double*)memmgr->declare_var("double dbl_1");
    a1->dbl_p = a5;
    *a5 = 3.1415;

    a2->x = 3.1415;
    a2->udt_p = a4;
    double *a6 = (double*)memmgr->declare_var("double dbl_2");
    a2->dbl_p = a6;
    *a6 = 3.1415;

    name_list.push_back("A1");
    name_list.push_back("A2");

    memmgr->write_checkpoint( ss, name_list);

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT1 A1;"
    "UDT1 A3;"
    "double dbl_1;"
    "UDT1 A2;"
    "UDT1 A4;"
    "double dbl_2;"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "A1.x = 3.1415;"
    "A1.udt_p = &A3.x;"
    "A1.dbl_p = &dbl_1;"
    "dbl_1 = 3.1415;"
    "A2.x = 3.1415;"
    "A2.udt_p = &A4.x;"
    "A2.dbl_p = &dbl_2;"
    "dbl_2 = 3.1415;"
     , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, reference_to_unknown_member) {
    std::stringstream ss;

    UDT1 *a1 = (UDT1 *)memmgr->declare_var("UDT1 A1");
    UDT2 *a2 = (UDT2 *)memmgr->declare_var("UDT2 A2");

    a1->dbl_p = &(a2->starStarDbl);

    memmgr->write_checkpoint(ss,"A1");

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT1 A1;"
    "UDT2 A2;"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "A1.dbl_p = &A2 + 8;"
     , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, reference_to_unknown_member_in_array) {
    std::stringstream ss;

    UDT1 *a1 = (UDT1 *)memmgr->declare_var("UDT1 A1");
    UDT2 *a2 = (UDT2 *)memmgr->declare_var("UDT2 A2[2]");

    a1->dbl_p = &(a2[1].starStarDbl);

    memmgr->write_checkpoint(ss, "A1");

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT1 A1;"
    "UDT2 A2[2];"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "A1.dbl_p = &A2[1] + 8;"
     , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, 1d_array_of_structures) {
    std::stringstream ss;

    UDT3 (*a)[3] = (UDT3(*)[3])memmgr->declare_var("UDT3 A[3]");

    (*a)[0].a = 1 ;
    (*a)[0].b = 2 ;
    (*a)[1].a = 3 ;
    (*a)[1].b = 4 ;
    (*a)[2].a = 5 ;
    (*a)[2].b = 6 ;

    memmgr->write_checkpoint(ss, "A");

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT3 A[3];"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "A[0].a = 1;"
    "A[0].b = 2;"
    "A[1].a = 3;"
    "A[1].b = 4;"
    "A[2].a = 5;"
    "A[2].b = 6;"
    , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, 2d_array_of_structures) {
    std::stringstream ss;

    UDT3 (*a)[2][3] = (UDT3(*)[2][3])memmgr->declare_var("UDT3 A[2][3]");

    (*a)[0][0].a = 1 ;
    (*a)[0][0].b = 2 ;
    (*a)[0][1].a = 3 ;
    (*a)[0][1].b = 4 ;
    (*a)[0][2].a = 5 ;
    (*a)[0][2].b = 6 ;
    (*a)[1][0].a = 7 ;
    (*a)[1][0].b = 8 ;
    (*a)[1][1].a = 9 ;
    (*a)[1][1].b = 10 ;
    (*a)[1][2].a = 11 ;
    (*a)[1][2].b = 12 ;

    memmgr->write_checkpoint(ss, "A");

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT3 A[2][3];"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "A[0][0].a = 1;"
    "A[0][0].b = 2;"
    "A[0][1].a = 3;"
    "A[0][1].b = 4;"
    "A[0][2].a = 5;"
    "A[0][2].b = 6;"
    "A[1][0].a = 7;"
    "A[1][0].b = 8;"
    "A[1][1].a = 9;"
    "A[1][1].b = 10;"
    "A[1][2].a = 11;"
    "A[1][2].b = 12;"
    , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, io_test ) {

    std::stringstream ss;

    UDT5 *udt5_p = (UDT5*)memmgr->declare_var("UDT5 udt5");
    udt5_p->star_star    = 3.0;
    udt5_p->star_aye     = 5.0;
    udt5_p->star_eau     = 8.0;
    udt5_p->star_aye_eau = 13.0;

    memmgr->write_checkpoint( ss, "udt5");
    memmgr->write_checkpoint( std::cout, "udt5");

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT5 udt5;"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "/*OUTPUT-ONLY: udt5.star_eau = 8;*/"
    "udt5.star_aye_eau = 13;"
    , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, Compressed_2d_char_arrays ) {

    // This test is associated with Issue: https://github.com/nasa/trick/issues/221
    // It is intended to test the code in Trick::ClassicCheckPointAgent::write_rvalue()
    // which checks the final, fixed dimension of a char array to determine whether
    // it can be written as a quoted string.

    std::stringstream ss;

    UDT6 *udt6_p = (UDT6*)memmgr->declare_var("UDT6 udt6");

    for (int ii=0; ii<128; ii++) {
        udt6_p->A[ii][0] = 'A';
        udt6_p->A[ii][1] = ((ii / 16) % 16) + 65;
        udt6_p->A[ii][2] =      ((ii) % 16) + 65;
        udt6_p->A[ii][3] = 0;

        udt6_p->B[ii][0] = 'B';
        udt6_p->B[ii][1] = ((ii / 16) % 16) + 65;
        udt6_p->B[ii][2] =      ((ii) % 16) + 65;
        udt6_p->B[ii][3] = 0;
    }
    memmgr->set_expanded_arrays(false);
    memmgr->write_checkpoint( ss, "udt6");

    int result = strcmp_IgnoringWhiteSpace(
    "// Variable Declarations."
    "UDT6 udt6;"
    "// Clear all allocations to 0."
    "clear_all_vars();"
    "// Variable Assignments."
    "udt6.A = {"
    "\"AAA\",\"AAB\",\"AAC\",\"AAD\",\"AAE\",\"AAF\",\"AAG\",\"AAH\","
    "\"AAI\",\"AAJ\",\"AAK\",\"AAL\",\"AAM\",\"AAN\",\"AAO\",\"AAP\","
    "\"ABA\",\"ABB\",\"ABC\",\"ABD\",\"ABE\",\"ABF\",\"ABG\",\"ABH\","
    "\"ABI\",\"ABJ\",\"ABK\",\"ABL\",\"ABM\",\"ABN\",\"ABO\",\"ABP\","
    "\"ACA\",\"ACB\",\"ACC\",\"ACD\",\"ACE\",\"ACF\",\"ACG\",\"ACH\","
    "\"ACI\",\"ACJ\",\"ACK\",\"ACL\",\"ACM\",\"ACN\",\"ACO\",\"ACP\","
    "\"ADA\",\"ADB\",\"ADC\",\"ADD\",\"ADE\",\"ADF\",\"ADG\",\"ADH\","
    "\"ADI\",\"ADJ\",\"ADK\",\"ADL\",\"ADM\",\"ADN\",\"ADO\",\"ADP\","
    "\"AEA\",\"AEB\",\"AEC\",\"AED\",\"AEE\",\"AEF\",\"AEG\",\"AEH\","
    "\"AEI\",\"AEJ\",\"AEK\",\"AEL\",\"AEM\",\"AEN\",\"AEO\",\"AEP\","
    "\"AFA\",\"AFB\",\"AFC\",\"AFD\",\"AFE\",\"AFF\",\"AFG\",\"AFH\","
    "\"AFI\",\"AFJ\",\"AFK\",\"AFL\",\"AFM\",\"AFN\",\"AFO\",\"AFP\","
    "\"AGA\",\"AGB\",\"AGC\",\"AGD\",\"AGE\",\"AGF\",\"AGG\",\"AGH\","
    "\"AGI\",\"AGJ\",\"AGK\",\"AGL\",\"AGM\",\"AGN\",\"AGO\",\"AGP\","
    "\"AHA\",\"AHB\",\"AHC\",\"AHD\",\"AHE\",\"AHF\",\"AHG\",\"AHH\","
    "\"AHI\",\"AHJ\",\"AHK\",\"AHL\",\"AHM\",\"AHN\",\"AHO\",\"AHP\" };"
    "udt6.B = {"
    "\"BAA\",\"BAB\",\"BAC\",\"BAD\",\"BAE\",\"BAF\",\"BAG\",\"BAH\","
    "\"BAI\",\"BAJ\",\"BAK\",\"BAL\",\"BAM\",\"BAN\",\"BAO\",\"BAP\","
    "\"BBA\",\"BBB\",\"BBC\",\"BBD\",\"BBE\",\"BBF\",\"BBG\",\"BBH\","
    "\"BBI\",\"BBJ\",\"BBK\",\"BBL\",\"BBM\",\"BBN\",\"BBO\",\"BBP\","
    "\"BCA\",\"BCB\",\"BCC\",\"BCD\",\"BCE\",\"BCF\",\"BCG\",\"BCH\","
    "\"BCI\",\"BCJ\",\"BCK\",\"BCL\",\"BCM\",\"BCN\",\"BCO\",\"BCP\","
    "\"BDA\",\"BDB\",\"BDC\",\"BDD\",\"BDE\",\"BDF\",\"BDG\",\"BDH\","
    "\"BDI\",\"BDJ\",\"BDK\",\"BDL\",\"BDM\",\"BDN\",\"BDO\",\"BDP\","
    "\"BEA\",\"BEB\",\"BEC\",\"BED\",\"BEE\",\"BEF\",\"BEG\",\"BEH\","
    "\"BEI\",\"BEJ\",\"BEK\",\"BEL\",\"BEM\",\"BEN\",\"BEO\",\"BEP\","
    "\"BFA\",\"BFB\",\"BFC\",\"BFD\",\"BFE\",\"BFF\",\"BFG\",\"BFH\","
    "\"BFI\",\"BFJ\",\"BFK\",\"BFL\",\"BFM\",\"BFN\",\"BFO\",\"BFP\","
    "\"BGA\",\"BGB\",\"BGC\",\"BGD\",\"BGE\",\"BGF\",\"BGG\",\"BGH\","
    "\"BGI\",\"BGJ\",\"BGK\",\"BGL\",\"BGM\",\"BGN\",\"BGO\",\"BGP\","
    "\"BHA\",\"BHB\",\"BHC\",\"BHD\",\"BHE\",\"BHF\",\"BHG\",\"BHH\","
    "\"BHI\",\"BHJ\",\"BHK\",\"BHL\",\"BHM\",\"BHN\",\"BHO\",\"BHP\" };"
    , ss.str().c_str());

    EXPECT_EQ(result, 0);
}

TEST_F(MM_write_checkpoint, ptr_to_array_of_ptrs_to_objects ) {

    // This test is associated with Issue: https://github.com/nasa/trick/issues/587

    // A possible fix to issue 587, involves aggregating condition_t and action_t
    // objects using a resizable list of pointers rather than a resizable array
    // of those objects. This is a test explores how this approach woud be
    // check-pointed.

    std::stringstream ss;

    // Allocate object with a double ptr member, that is
    // a pointer to an array of pointers to some (3 in this case) objects.
    UDT7 *udt7_p = (UDT7*)memmgr->declare_var("UDT7 udt7");

    // Initialize the object, and its aggregation.
    udt7_p->A = 2.71828;
    UDT3** pp = (UDT3**)memmgr->declare_var("UDT3* udt3p[3]");
    udt7_p->udt3pp = pp;
    pp[0] = (UDT3*)memmgr->declare_var("UDT3 udt3A");
    pp[1] = (UDT3*)memmgr->declare_var("UDT3 udt3B");
    pp[2] = (UDT3*)memmgr->declare_var("UDT3 udt3C");
    pp[0]->a = 1;
    pp[0]->b = 2;
    pp[1]->a = 4;
    pp[1]->b = 5;
    pp[2]->a = 7;
    pp[2]->b = 8;

    memmgr->set_expanded_arrays(false);
    // Write the checkpoint.
    memmgr->write_checkpoint( ss, "udt7");

    // Verify it's what we expected.
    int result = strcmp_IgnoringWhiteSpace(
        "// Variable Declarations."
        "UDT7 udt7;"
        "UDT3* udt3p[3];"
        "UDT3 udt3A;"
        "UDT3 udt3B;"
        "UDT3 udt3C;"
        "// Clear all allocations to 0."
        "clear_all_vars();"
        "// Variable Assignments."
        "udt7.A = 2.71828;"
        "udt7.udt3pp = &udt3p[0].a;"
        "udt3p[0] = &udt3A;"
        "udt3p[1] = &udt3B;"
        "udt3p[2] = &udt3C;"
        "udt3A.a = 1;"
        "udt3A.b = 2;"
        "udt3B.a = 4;"
        "udt3B.b = 5;"
        "udt3C.a = 7;"
        "udt3C.b = 8;"
    , ss.str().c_str());

    EXPECT_EQ(result, 0);

    // Delete all of the objects that we created.
    memmgr->delete_var("udt3A");
    memmgr->delete_var("udt3B");
    memmgr->delete_var("udt3C");
    memmgr->delete_var("udt3p");
    memmgr->delete_var("udt7");
    udt7_p = (UDT7*)NULL;
    pp = (UDT3**)NULL;

    // Re-create the objects from a checkpoint.
    memmgr->read_checkpoint_from_string(ss.str().c_str());

    EXPECT_EQ(result, 0);

    // Get the address of the object we just restored.
    REF2* ref = memmgr->ref_attributes("udt7");
    udt7_p = (UDT7*)ref->address;

    // Verify that the values are what they should be.
    EXPECT_NEAR(udt7_p->A, 2.71828, 0.000001);
    EXPECT_EQ( udt7_p->udt3pp[0]->a , 1);
    EXPECT_EQ( udt7_p->udt3pp[0]->b , 2);
    EXPECT_EQ( udt7_p->udt3pp[1]->a , 4);
    EXPECT_EQ( udt7_p->udt3pp[1]->b , 5);
    EXPECT_EQ( udt7_p->udt3pp[2]->a , 7);
    EXPECT_EQ( udt7_p->udt3pp[2]->b , 8);
}

