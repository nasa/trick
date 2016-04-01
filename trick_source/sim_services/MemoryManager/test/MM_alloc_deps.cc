
#include <gtest/gtest.h>
#define private public
//#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "MM_test.hh"
#include "MM_alloc_deps.hh"
#include <iostream>


/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */
class MM_alloc_deps : public ::testing::Test {

        protected:
                Trick::MemoryManager *memmgr;
                MM_alloc_deps() {
                        try {
                                memmgr = new Trick::MemoryManager;
                        } catch (std::logic_error e) {
                                memmgr = NULL;
                        }
                }
                ~MM_alloc_deps() {
                        delete memmgr;
                }
                void SetUp() {}
                void TearDown() {}
};

bool dependencies_contain(std::vector<ALLOC_INFO*>& dependencies, const char* name) {

        int n_depends = dependencies.size();
        for (int ii=0 ; ii < n_depends ; ii++ ) {
           // std::cout << "###" << dependencies[ii]->name << std::endl;
            if ( strcmp (dependencies[ii]->name, name) == 0 ){
                return (true);
            }
        }
        return false;
}

// ================================================================================
TEST_F(MM_alloc_deps, dbl_singleton) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    double *dbl_p = (double*)memmgr->declare_var("double dbl_singleton");
    *dbl_p = 3.1415;

    memmgr->get_alloc_deps_in_allocation("dbl_singleton");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 1);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl_singleton"), true);
}


// ================================================================================
TEST_F(MM_alloc_deps, dbl_pointer) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    double **dbl_pp = (double**)memmgr->declare_var("double *dbl_pointer");
    double *dbl_p = (double*)memmgr->declare_var("double dbl_array[2]");

    *dbl_pp = dbl_p;

    memmgr->get_alloc_deps_in_allocation("dbl_pointer");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 2);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl_pointer"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl_array"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, dbl_array) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    double *dbl_p = (double*)memmgr->declare_var("double dbl_array[3]");

    dbl_p[0] = 1.1;
    dbl_p[1] = 2.2;
    dbl_p[2] = 3.3;

    memmgr->get_alloc_deps_in_allocation("dbl_array");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 1);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl_array"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, dbl_array_pointers) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(2);

    double **dbl_pa = (double**)memmgr->declare_var("double *dbl_array_ptrs[3]");

    double *dbl_p1 = (double*)memmgr->declare_var("double dbl1");
    double *dbl_p2 = (double*)memmgr->declare_var("double dbl2");
    double *dbl_p3 = (double*)memmgr->declare_var("double dbl3");

    dbl_pa[0] = dbl_p1;
    dbl_pa[1] = dbl_p2;
    dbl_pa[2] = dbl_p3;

    memmgr->get_alloc_deps_in_allocation("dbl_array_ptrs");

    // There should be 4 dependencies.
    EXPECT_EQ( dependencies.size(), 4);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl_array_ptrs"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl1"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl2"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl3"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, udt_singleton_no_deps) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    UDT1 *udt_p = (UDT1*)memmgr->declare_var("UDT1 udt_singleton");

    udt_p->x = 3.1415;
    udt_p->udt_p = NULL;
    udt_p->dbl_p = NULL;

    memmgr->get_alloc_deps_in_allocation("udt_singleton");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 1);
    EXPECT_EQ( dependencies_contain(dependencies, "udt_singleton"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, udt_singleton_self_referential) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    UDT1 *udt_p = (UDT1*)memmgr->declare_var("UDT1 udt_singleton");

    udt_p->x = 3.1415;
    udt_p->udt_p = udt_p;
    udt_p->dbl_p = NULL;

    memmgr->get_alloc_deps_in_allocation("udt_singleton");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 1);
    EXPECT_EQ( dependencies_contain(dependencies, "udt_singleton"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, udt_singleton_deps) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    UDT1 *udt1_p = (UDT1*)memmgr->declare_var("UDT1 udt1");
    UDT1 *udt2_p = (UDT1*)memmgr->declare_var("UDT1 udt2");
    double *dbl1_p = (double*)memmgr->declare_var("double dbl1");

    udt1_p->x = 3.1415;
    udt1_p->udt_p = udt2_p;
    udt1_p->dbl_p = dbl1_p;

    memmgr->get_alloc_deps_in_allocation("udt1");

    // There should be 3 dependencies.
    EXPECT_EQ( dependencies.size(), 3);
    EXPECT_EQ( dependencies_contain(dependencies, "udt1"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "udt1"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl1"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, udt_pointer) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

    UDT1 **udt1_p = (UDT1**)memmgr->declare_var("UDT1 *udt_p");
    UDT1  *a1 = (UDT1 *)memmgr->declare_var("UDT1 bbb");

    a1->x = 3.1415;
    a1->udt_p = NULL;

    *udt1_p = a1;

    memmgr->get_alloc_deps_in_allocation("udt_p");

    // There should be 2 dependencies.
    EXPECT_EQ( dependencies.size(), 2);

    EXPECT_EQ( dependencies_contain(dependencies, "udt_p"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "bbb"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, udt_array_no_deps) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

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

    memmgr->get_alloc_deps_in_allocation("udt_array");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 1);
    EXPECT_EQ( dependencies_contain(dependencies, "udt_array"), true);
}

// ================================================================================
TEST_F(MM_alloc_deps, udt_array_deps) {
    std::vector<ALLOC_INFO*> & dependencies = memmgr->dependencies ;

    //memmgr->set_debug_level(1);

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

    memmgr->get_alloc_deps_in_allocation("udt_array");

    // There should be 1 dependency.
    EXPECT_EQ( dependencies.size(), 7);
    EXPECT_EQ( dependencies_contain(dependencies, "udt_array"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "udt1"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "udt2"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "udt3"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl1"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl2"), true);
    EXPECT_EQ( dependencies_contain(dependencies, "dbl3"), true);
}
