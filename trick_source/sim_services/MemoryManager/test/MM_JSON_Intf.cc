#include <gtest/gtest.h>
#include "MM_test.hh"
#include "MM_user_defined_types.hh"

/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */
class MM_JSON_intf : public ::testing::Test {

	protected:
		Trick::MemoryManager *memmgr;

		MM_JSON_intf() {
			try {
				memmgr = new Trick::MemoryManager;
			} catch (std::logic_error e) {
				memmgr = NULL;
			}
		}

		~MM_JSON_intf() {
			delete memmgr;
		}

		void SetUp() {}
		void TearDown() {}
};

/*
     The tests.
 */
TEST_F(MM_JSON_intf, Write_Named_Local_C_Alloc_Info ) {
    std::stringstream ss;
    std::string s;

    double *d_p = (double*)memmgr->declare_var("double foo[2]");
    ALLOC_INFO* alloc_info = memmgr->get_alloc_info_of(d_p);
    memmgr->write_JSON_alloc_info(ss, alloc_info);
    s = ss.str();

    std::size_t pos;
    bool found = true;

    pos = s.find("name");
    if (pos == std::string::npos) found = false;
    pos = s.find("foo");
    if (pos == std::string::npos) found = false;
    pos = s.find("start");
    if (pos == std::string::npos) found = false;
    pos = s.find("end");
    if (pos == std::string::npos) found = false;
    pos = s.find("num");
    if (pos == std::string::npos) found = false;
    pos = s.find("size");
    if (pos == std::string::npos) found = false;
    pos = s.find("type");
    if (pos == std::string::npos) found = false;
    pos = s.find("stcl");
    if (pos == std::string::npos) found = false;
    pos = s.find("TRICK_LOCAL");
    if (pos == std::string::npos) found = false;
    pos = s.find("language");
    if (pos == std::string::npos) found = false;
    pos = s.find("Language_C");
    if (pos == std::string::npos) found = false;
    pos = s.find("index");
    if (pos == std::string::npos) found = false;
    EXPECT_EQ(found, true);
}

 TEST_F(MM_JSON_intf, Write_Anon_Extern_CPP_Alloc_Info ) {
    std::stringstream ss;
    std::string s;

    UDT1 test_var;
    UDT1 *test_var_p = (UDT1 *)memmgr->declare_extern_var(&test_var, "UDT1");
    ALLOC_INFO* alloc_info = memmgr->get_alloc_info_of(test_var_p);
    //memmgr->write_JSON_alloc_info(std::cout, alloc_info);
    memmgr->write_JSON_alloc_info(ss, alloc_info);
    s = ss.str();

    std::size_t pos;
    bool found = true;

    pos = s.find("name");
    if (pos == std::string::npos) found = false;
    pos = s.find("start");
    if (pos == std::string::npos) found = false;
    pos = s.find("end");
    if (pos == std::string::npos) found = false;
    pos = s.find("num");
    if (pos == std::string::npos) found = false;
    pos = s.find("size");
    if (pos == std::string::npos) found = false;
    pos = s.find("type");
    if (pos == std::string::npos) found = false;
    pos = s.find("stcl");
    if (pos == std::string::npos) found = false;
    pos = s.find("TRICK_EXTERN");
    if (pos == std::string::npos) found = false;
    pos = s.find("language");
    if (pos == std::string::npos) found = false;
    pos = s.find("Language_CPP");
    if (pos == std::string::npos) found = false;
    pos = s.find("index");
    if (pos == std::string::npos) found = false;

    EXPECT_EQ(found, true);
}

TEST_F(MM_JSON_intf, Write_Alloc_Info_List ) {
    std::stringstream ss;
    std::string s;
    void *ptr;
    ptr = memmgr->declare_var("double apple");
    ptr = memmgr->declare_var("double banana");
    ptr = memmgr->declare_var("double cherry");
    ptr = memmgr->declare_var("double date");
    ptr = memmgr->declare_var("double elderberry");

    memmgr->write_JSON_alloc_list(std::cout, 0, 2);
    memmgr->write_JSON_alloc_list(ss, 0, 2);
    s = ss.str();

    std::size_t pos;
    bool found = true;

    pos = s.find("alloc_total");
    if (pos >= std::string::npos) found = false;
    pos = s.find("chunk_size");
    if (pos >= std::string::npos) found = false;
    pos = s.find("chunk_start");
    if (pos >= std::string::npos) found = false;
    pos = s.find("alloc_list");
    if (pos == std::string::npos) found = false;

    EXPECT_EQ(found, true);

}
