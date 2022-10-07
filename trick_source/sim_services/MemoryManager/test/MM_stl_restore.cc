#include "gtest/gtest.h"
#define private public

#include "trick/MemoryManager.hh"
#include "trick/memorymanager_c_intf.h"
#include "MM_stl_testbed.hh"
#include "MM_test.hh"

#include "trick/checkpoint_sequence_stl.hh"
#include "trick/checkpoint_map.hh"
#include "trick/checkpoint_stack.hh"
#include "trick/checkpoint_queue.hh"
#include "trick/checkpoint_pair.hh"

/*
 This tests the allocation of user-defined types using MemoryManager::declare_var().
 */
class MM_stl_restore : public ::testing::Test {

    protected:
        Trick::MemoryManager *memmgr;

        MM_stl_restore() {
            try {
                memmgr = new Trick::MemoryManager;
            } catch (std::logic_error e) {
                memmgr = NULL;
            }
        }
        ~MM_stl_restore() {
            delete memmgr;
        }
        void SetUp() {}
        void TearDown() {}
};



TEST_F(MM_stl_restore, i_vec ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_vec")->attr;


    std::vector<int> test_data = get_test_data<int>(20);
    // Register the expected temporary variables with the memory manager
    int * temp_data = (int *) memmgr->declare_var("int my_alloc_i_vec[20]");
    for (int i = 0; i < 20; i++) {
        temp_data[i] = test_data[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_vec, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_vec, test_data);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_vec"), 0);
}

TEST_F(MM_stl_restore, i_s_vec ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_s_vec")->attr;


    std::vector<int> test_data = get_test_data<int>(50);
    std::vector<int> lengths = {20, 4, 1, 1, 14, 10};

    // Register the expected temporary variables with the memory manager
    std::string my_alloc_i_s_vec[6];
    int start_index = 0;
    memmgr->declare_extern_var(&my_alloc_i_s_vec, "std::string my_alloc_i_s_vec[6]");
    for (int i = 0; i < lengths.size(); i++) {
        // std::string var_name = 
        my_alloc_i_s_vec[i] = "my_alloc_i_s_vec_" + std::to_string(i);

        std::string temp_var_name = "int my_alloc_i_s_vec_" + std::to_string(i) + "[" + std::to_string(lengths[i]) + "]";
        int * temp_arr = (int *)memmgr->declare_var(temp_var_name.c_str());
        for (int j = 0; j < lengths[i]; j++) {
            temp_arr[j] = test_data[start_index + j];
        }

        start_index += lengths[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_s_vec, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->i_s_vec.size(), lengths.size());
    start_index = 0;
    for (int i = 0; i < lengths.size(); i++) {
        ASSERT_EQ(testbed->i_s_vec[i].size(), lengths[i]); 
        EXPECT_EQ(testbed->i_s_vec[i], std::vector<int>(test_data.begin()+start_index, test_data.begin()+start_index+lengths[i]));
        start_index += lengths[i];
    }

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_vec"), 0);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_vec_" + std::to_string(i)), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_vec_" + std::to_string(i)), 0);
    }
}

TEST_F(MM_stl_restore, s_vec ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<int> test_data = get_test_data<int>(40);

    // Register the expected temporary variables with the memory manager
    std::string my_alloc_s_vec[20];
    memmgr->declare_extern_var(&my_alloc_s_vec, "std::string my_alloc_s_vec[20]");
    for (int i = 0; i < test_data.size()/2; i++) {
        my_alloc_s_vec[i] = "my_alloc_s_vec_" + std::to_string(i);

        std::string first_var_name = "int my_alloc_s_vec_" + std::to_string(i) + "_first";
        std::string second_var_name = "int my_alloc_s_vec_" + std::to_string(i) + "_second";

        int * temp_first = (int *)memmgr->declare_var(first_var_name.c_str());
        *temp_first = test_data[i*2];
        int * temp_second = (int *)memmgr->declare_var(second_var_name.c_str());
        *temp_second = test_data[i*2+1];
    }
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.s_vec")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->s_vec, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->s_vec.size(), test_data.size()/2);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(testbed->s_vec[i].first, test_data[i*2]);
        EXPECT_EQ(testbed->s_vec[i].second, test_data[i*2+1]);
    }

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_vec"), 0);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_vec_" + std::to_string(i) + "_first"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_vec_" + std::to_string(i) + "_second"), 0);
    }
}

TEST_F(MM_stl_restore, string_vec ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<std::string> test_data = get_test_data<std::string>(20);

    // Register the expected temporary variables with the memory manager
    std::string * temp_data = (std::string *) memmgr->declare_var("std::string my_alloc_string_vec[20]");
    for (int i = 0; i < 20; i++) {
        temp_data[i] = std::string(test_data[i]);
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.string_vec")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->string_vec, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->string_vec, test_data);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_string_vec"), 0);
}

TEST_F(MM_stl_restore, i_i_pair ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    double test_first  = random<double>();
    float  test_second = random<float> ();

    // Register the expected temporary variables with the memory manager
    double * first_data = (double *) memmgr->declare_var("double my_alloc_i_i_pair_first");
    float * second_data = (float *) memmgr->declare_var("float my_alloc_i_i_pair_second");

    *first_data = test_first;
    *second_data = test_second;

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_i_pair")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->i_i_pair, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_i_pair.first, test_first);
    EXPECT_EQ(testbed->i_i_pair.second, test_second);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_i_pair_first"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_i_pair_second"), 0);
}

TEST_F(MM_stl_restore, i_s_pair ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    double test_first  = random<double>();
    std::vector<bool> test_second = get_test_data<bool>(20);

    // Register the expected temporary variables with the memory manager
    double * first_data = (double *) memmgr->declare_var("double my_alloc_i_s_pair_first");
    std::string * second_link = (std::string *) memmgr->declare_var("std::string my_alloc_i_s_pair_second"); 
    bool * second_data = (bool *) memmgr->declare_var("bool my_alloc_i_s_pair_second_inner[20]");

    *first_data = test_first;
    *second_link = std::string("inner");
    for (int i = 0; i < test_second.size(); i++) {
        second_data[i] = test_second[i];
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_s_pair")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->i_s_pair, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_s_pair.first, test_first);
    EXPECT_EQ(testbed->i_s_pair.second.size(), test_second.size());
    for (int i = 0; i < test_second.size(); i++) {
        EXPECT_EQ(testbed->i_s_pair.second.front(), test_second[i]);
        testbed->i_s_pair.second.pop();
    }

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_pair_first"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_pair_second_inner"), 0);
}

TEST_F(MM_stl_restore, s_i_pair ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<bool> test_first = get_test_data<bool>(20);
    double test_second  = random<double>();

    // Register the expected temporary variables with the memory manager
    bool * first_data = (bool *) memmgr->declare_var("bool my_alloc_s_i_pair_first_inner[20]");
    std::string * first_link = (std::string *) memmgr->declare_var("std::string my_alloc_s_i_pair_first");
    *first_link = "inner";
    double * second_data = (double *) memmgr->declare_var("double my_alloc_s_i_pair_second");

    for (int i = 0; i < test_first.size(); i++) {
        first_data[i] = test_first[i];
    }
    *second_data = test_second;


    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.s_i_pair")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->s_i_pair, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_i_pair.second, test_second);
    ASSERT_EQ(testbed->s_i_pair.first.size(), test_first.size());
    for (int i = 0; i < test_first.size(); i++) {
        EXPECT_EQ(testbed->s_i_pair.first.front(), test_first[i]);
        testbed->s_i_pair.first.pop();
    }

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_i_pair_first_inner"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_i_pair_second"), 0);
}

TEST_F(MM_stl_restore, s_s_pair ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<int> test_first = get_test_data<int>(20);
    std::vector<float> test_second = get_test_data<float>(20);

    // Register the expected temporary variables with the memory manager
    int * first_data = (int *) memmgr->declare_var("int my_alloc_s_s_pair_first_inner[20]");
    float * second_data = (float *) memmgr->declare_var("float my_alloc_s_s_pair_second_inner[20]");

    std::string * first_link  = (std::string *) memmgr->declare_var("std::string my_alloc_s_s_pair_first");
    std::string * second_link = (std::string *) memmgr->declare_var("std::string my_alloc_s_s_pair_second");
    *first_link  = std::string("inner");
    *second_link  = std::string("inner");



    for (int i = 0; i < test_first.size(); i++) {
        first_data[i] = test_first[i];
    }
    for (int i = 0; i < test_second.size(); i++) {
        second_data[i] = test_second[i];
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.s_s_pair")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->s_s_pair, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_s_pair.first.size(), test_first.size());
    EXPECT_EQ(testbed->s_s_pair.second.size(), test_second.size());
    for (int i = 0; i < test_first.size(); i++) {
        EXPECT_EQ(testbed->s_s_pair.first[i], test_first[i]);
        EXPECT_EQ(testbed->s_s_pair.second.top(), test_second[i]);
        testbed->s_s_pair.second.pop();
    }

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_s_pair_first_inner"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_s_pair_second_inner"), 0);
}

TEST_F(MM_stl_restore, pair_pair ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    int test_first_first  = random<int>();
    int test_first_second  = random<int>();
    int test_second = random<int>();

    // Register the expected temporary variables with the memory manager
    int * first_first_data = (int *) memmgr->declare_var("int my_alloc_pair_pair_first_inner_first");
    int * first_second_data = (int *) memmgr->declare_var("int my_alloc_pair_pair_first_inner_second");
    int * second_data = (int *) memmgr->declare_var("int my_alloc_pair_pair_second");

    std::string * first_link =  (std::string *) memmgr->declare_var("std::string my_alloc_pair_pair_first[1]");
    *first_link = std::string ("inner");

    *first_first_data = test_first_first;
    *first_second_data = test_first_second;
    *second_data = test_second;

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.pair_pair")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->pair_pair, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->pair_pair.first.first, test_first_first);
    EXPECT_EQ(testbed->pair_pair.first.second, test_first_second);
    EXPECT_EQ(testbed->pair_pair.second, test_second);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_pair_pair_first"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_pair_pair_first_inner_first"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_pair_pair_first_inner_second"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_pair_pair_second"), 0);
}

TEST_F(MM_stl_restore, i_i_map ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<int> test_keys = get_test_data<int>(20);
    std::vector<double> test_data = get_test_data<double>(20);
    std::map<int, double> test_map;

    int * key_data = (int *) memmgr->declare_var("int my_alloc_i_i_map_keys[20]");
    double * val_data = (double *) memmgr->declare_var("double my_alloc_i_i_map_data[20]");
    for (int i = 0; i < test_keys.size(); i++) {
        key_data[i] = test_keys[i];
        val_data[i] = test_data[i];

        // Create a map of expected data along side so that we don't have to deal with ordering stuff
        test_map[test_keys[i]] = test_data[i];
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_i_map")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->i_i_map, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_i_map, test_map);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_i_map_keys"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_i_map_data"), 0);
}

TEST_F(MM_stl_restore, i_s_map ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<int> test_keys = get_test_data<int>(10);
    std::vector<std::string> test_data = get_test_data<std::string>(60);
    std::map<int, std::stack<std::string>> test_map;

    int * key_data = (int *) memmgr->declare_var("int my_alloc_i_s_map_keys[10]");
    std::string * val_data = (std::string *) memmgr->declare_var("std::string my_alloc_i_s_map_data[10]");
    for (int i = 0; i < test_keys.size(); i++) {
        key_data[i] = test_keys[i];
        val_data[i] = std::string("my_alloc_i_s_map_data_" + std::to_string(i));

        std::string var_name = "std::string my_alloc_i_s_map_data_" + std::to_string(i) + "[" + std::to_string(6) + "]";
        std::string * val_temp = (std::string *) memmgr->declare_var(var_name.c_str());

        std::vector<std::string> test_strings_easy = {"a", "b", "c", "d", "e", "f"};
        for (int j = 0; j < 6; j++) {
            // val_temp[j] = std::string(test_data[(i*6) + j]);
            val_temp[j] = std::string(test_strings_easy[j]);


            // Create a map of expected data along side so that we don't have to deal with ordering stuff
            // Reverse order of the data since it's a stack
            test_map[test_keys[i]].push(std::string(test_strings_easy[5-j]));
        }
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_s_map")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->i_s_map, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_s_map, test_map);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_map_keys"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_s_map_data"), 0);
    for (int i = 0; i < testbed->i_s_map.size(); i++) {
        std::string var_name_test = "my_alloc_i_s_map_data_" + std::to_string(i);
        EXPECT_EQ(memmgr->var_exists(var_name_test.c_str()), 0);
    }
}

TEST_F(MM_stl_restore, s_i_map ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    std::vector<int> test_keys = get_test_data<int>(30);
    std::vector<std::string> test_data = get_test_data<std::string>(10);
    std::map<std::set<int>, std::string> test_map;

    std::string * key_data = (std::string *) memmgr->declare_var("std::string my_alloc_s_i_map_keys[10]");
    std::string * val_data = (std::string *) memmgr->declare_var("std::string my_alloc_s_i_map_data[10]");
    for (int i = 0; i < test_data.size(); i++) {
        key_data[i] = std::string("my_alloc_s_i_map_keys_" + std::to_string(i));

        std::string var_name = "int my_alloc_s_i_map_keys_" + std::to_string(i) + "[" + std::to_string(3) + "]";
        int * val_temp = (int *) memmgr->declare_var(var_name.c_str());

        std::set<int> temp_set;
        for (int j = 0; j < 3; j++) {
            val_temp[j] = test_keys[(i*3) + j];

            // Create a map of expected data along side so that we don't have to deal with ordering stuff
            temp_set.insert(test_keys[(i*3) + j]);
        }

        val_data[i] = test_data[i];

        // insert into the test map
        test_map[temp_set] = test_data[i];
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.s_i_map")->attr;

    // ACT
    (attr->restore_stl)((void *) &testbed->s_i_map, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_i_map, test_map);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_i_map_keys"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_i_map_data"), 0);
    for (int i = 0; i < testbed->i_s_map.size(); i++) {
        std::string var_name_test = "my_alloc_s_i_map_keys_" + std::to_string(i);
        EXPECT_EQ(memmgr->var_exists(var_name_test.c_str()), 0);
    }
}


TEST_F(MM_stl_restore, s_s_map ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  

    int map_size = 10;
    std::vector<int> test_keys = get_test_data<int>(20);
    std::vector<int> test_data = get_test_data<int>(40);
    std::map<std::pair<int,int>, std::vector<int>> test_map;

    // Make the top level key and data structures
    // These will just have strings that point to where the actual data is
    std::string * key_data = (std::string *) memmgr->declare_var("std::string my_alloc_s_s_map_keys[10]");
    std::string * val_data = (std::string *) memmgr->declare_var("std::string my_alloc_s_s_map_data[10]");
    for (int i = 0; i < map_size; i++) {
        // This is the string that lives in the my_alloc_s_s_map_keys structure
        key_data[i] = std::string("my_alloc_s_s_map_keys_" + std::to_string(i));

        // These first and second variables hold the actual data for index i
        std::string first_name = "int my_alloc_s_s_map_keys_" + std::to_string(i) + "_first";
        int * val_first = (int *) memmgr->declare_var(first_name.c_str());
        *val_first = test_keys[i*2];

        std::string second_name = "int my_alloc_s_s_map_keys_" + std::to_string(i) + "_second";
        int * val_second = (int *) memmgr->declare_var(second_name.c_str());
        *val_second = test_keys[i*2+1];

        // This is the string that lives in the my_alloc_s_s_map_data structure
        val_data[i] = std::string("my_alloc_s_s_map_data_" + std::to_string(i));

        // This is the array that holds the actual data for index i
        std::string data_name = "int my_alloc_s_s_map_data_" + std::to_string(i) + "[" + std::to_string(4) + "]";
        int * data_values = (int *) memmgr->declare_var(data_name.c_str());

        for (int j = 0; j < 4; j++) {
            data_values[j] = test_data[(i*4) + j];

            // Create a map of expected data along side so that we don't have to deal with ordering stuff
            test_map[std::pair<int,int>(test_keys[i*2], test_keys[i*2+1])].push_back(test_data[(i*4) + j]);
        }
    }

    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.s_s_map")->attr;



    // ACT
    (attr->restore_stl)((void *) &testbed->s_s_map, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_s_map, test_map);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_s_map_keys"), 0);
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_s_map_data"), 0);
    for (int i = 0; i < map_size; i++) {
        std::string var_name_test = "my_alloc_s_s_map_keys_" + std::to_string(i) + "_first";
        EXPECT_EQ(memmgr->var_exists(var_name_test.c_str()), 0);
        var_name_test = "my_alloc_s_s_map_keys_" + std::to_string(i) + "_second";
        EXPECT_EQ(memmgr->var_exists(var_name_test.c_str()), 0);
        var_name_test = "my_alloc_s_s_map_data_" + std::to_string(i);
        EXPECT_EQ(memmgr->var_exists(var_name_test.c_str()), 0);
    }
}


TEST_F(MM_stl_restore, i_queue ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_queue")->attr;


    std::vector<int> test_data = get_test_data<int>(20);
    std::queue<int> expected_data;
    // Register the expected temporary variables with the memory manager
    int * temp_data = (int *) memmgr->declare_var("int my_alloc_i_queue[20]");
    for (int i = 0; i < 20; i++) {
        temp_data[i] = test_data[i];
        expected_data.push(test_data[i]);
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_queue, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_queue, expected_data);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_queue"), 0);
}


TEST_F(MM_stl_restore, s_queue ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES * attr =  memmgr->ref_attributes("my_alloc.s_queue")->attr;

    std::vector<int> test_first= get_test_data<int>(20);
    std::vector<double> test_second = get_test_data<double>(20);

    std::queue<std::pair<int,double>> expected;

    std::string * data_links  = (std::string *) memmgr->declare_var("std::string my_alloc_s_queue[20]");

    for (int i = 0; i < test_first.size(); i++) {
        data_links[i] = "my_alloc_s_queue_" + std::to_string(i);

        std::string first_var_name = "int my_alloc_s_queue_" + std::to_string(i) + "_first";
        std::string second_var_name = "double my_alloc_s_queue_" + std::to_string(i) + "_second";

        int * temp_first = (int *)memmgr->declare_var(first_var_name.c_str());
        *temp_first = test_first[i];
        double * temp_second = (double *)memmgr->declare_var(second_var_name.c_str());
        *temp_second = test_second[i];

        // Build a copy of the expected data
        expected.push(std::pair<int,double>(test_first[i], test_second[i]));
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->s_queue, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_queue, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_queue"), 0);
    for (int i = 0; i < test_first.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_queue_" + std::to_string(i) + "_first"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_queue_" + std::to_string(i) + "_second"), 0);
    }
}

TEST_F(MM_stl_restore, nested_list_queue ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.nested_list_queue")->attr;

    std::vector<float> test_data = get_test_data<float>(50);
    // Make sure we can handle lists of different sizes
    std::vector<int> lengths = {20, 4, 1, 1, 14, 10};

    // Register the expected temporary variables with the memory manager
    std::queue<std::list<float>> expected;
    
    int start_index = 0;
    std::string * nested_list_links = (std::string *) memmgr->declare_var( "std::string my_alloc_nested_list_queue[6]");
    for (int i = 0; i < lengths.size(); i++) {
        nested_list_links[i] = "my_alloc_nested_list_queue_" + std::to_string(i);

        std::string temp_var_name = "float my_alloc_nested_list_queue_" + std::to_string(i) + "[" + std::to_string(lengths[i]) + "]";
        std::list<float> temp_expected;
        float * temp_arr = (float *)memmgr->declare_var(temp_var_name.c_str());
        for (int j = 0; j < lengths[i]; j++) {
            temp_arr[j] = test_data[start_index + j];
            temp_expected.push_back(test_data[start_index + j]);
        }

        expected.push(temp_expected);

        start_index += lengths[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->nested_list_queue, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->nested_list_queue, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_nested_list_queue"), 0);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_list_queue_" + std::to_string(i)), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_list_queue_" + std::to_string(i)), 0);
    }
}

TEST_F(MM_stl_restore, i_stack ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_stack")->attr;


    std::vector<long long> test_data = get_test_data<long long>(20);
    std::stack<long long> expected_data;
    // Register the expected temporary variables with the memory manager
    long long * temp_data = (long long *) memmgr->declare_var("long long my_alloc_i_stack[20]");
    for (int i = 0; i < test_data.size(); i++) {
        temp_data[i] = test_data[i];

        // push backwards bc stack
        expected_data.push(test_data[test_data.size()-1-i]);
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_stack, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_stack, expected_data);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_stack"), 0);
}

TEST_F(MM_stl_restore, s_stack ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES * attr =  memmgr->ref_attributes("my_alloc.s_stack")->attr;

    std::vector<short> test_first= get_test_data<short>(20);
    std::vector<double> test_second = get_test_data<double>(20);

    std::stack<std::pair<short,double>> expected;

    std::string * data_links  = (std::string *) memmgr->declare_var("std::string my_alloc_s_stack[20]");

    for (int i = 0; i < test_first.size(); i++) {
        data_links[i] = "my_alloc_s_stack_" + std::to_string(i);

        std::string first_var_name = "short my_alloc_s_stack_" + std::to_string(i) + "_first";
        std::string second_var_name = "double my_alloc_s_stack_" + std::to_string(i) + "_second";

        short * temp_first = (short *)memmgr->declare_var(first_var_name.c_str());
        *temp_first = test_first[i];
        double * temp_second = (double *)memmgr->declare_var(second_var_name.c_str());
        *temp_second = test_second[i];

        // Build a copy of the expected data
        // Backwards because stack
        expected.push(std::pair<short,double>(test_first[test_first.size()-1-i], test_second[test_first.size()-1-i]));
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->s_stack, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_stack, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_stack"), 0);
    for (int i = 0; i < test_first.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_stack_" + std::to_string(i) + "_first"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_stack_" + std::to_string(i) + "_second"), 0);
    }
}

TEST_F(MM_stl_restore, nested_list_stack ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.nested_list_stack")->attr;

    std::vector<float> test_data = get_test_data<float>(50);
    // Make sure we can handle lists of different sizes
    std::vector<int> lengths = {20, 4, 1, 1, 14, 10};

    // Just build the stack backwards and flip it
    std::stack<std::list<float>> expected;
    std::stack<std::list<float>> reverse_expected;

    // Register the expected temporary variables with the memory manager
    int start_index = 0;
    std::string * nested_list_links = (std::string *) memmgr->declare_var( "std::string my_alloc_nested_list_stack[6]");
    for (int i = 0; i < lengths.size(); i++) {
        nested_list_links[i] = "my_alloc_nested_list_stack_" + std::to_string(i);

        std::string temp_var_name = "float my_alloc_nested_list_stack_" + std::to_string(i) + "[" + std::to_string(lengths[i]) + "]";
        std::list<float> temp_expected;
        float * temp_arr = (float *)memmgr->declare_var(temp_var_name.c_str());
        for (int j = 0; j < lengths[i]; j++) {
            temp_arr[j] = test_data[start_index + j];
            temp_expected.push_back(test_data[start_index + j]);
        }

        reverse_expected.push(temp_expected);

        start_index += lengths[i];
    }

    // flip the stack so we have expected in the right order
    while (!reverse_expected.empty()) {
        expected.push(reverse_expected.top());
        reverse_expected.pop();
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->nested_list_stack, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->nested_list_stack, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_nested_list_stack"), 0);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_list_stack_" + std::to_string(i)), 0);
    }
}

TEST_F(MM_stl_restore, i_set ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_set")->attr;


    std::vector<char> test_data = get_test_data<char>(100);
    std::set<char> expected;

    // Make the expected structure first
    for (char val : test_data) {
        expected.insert(val);
    }

    // Register the expected temporary variables with the memory manager
    std::string var_name = "char my_alloc_i_set[" + std::to_string(expected.size()) + "]";
    char * temp_data = (char *) memmgr->declare_var(var_name.c_str());
    int index = 0;
    for (const char val : expected) {
        temp_data[index++] = val;
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_set, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_set, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_set"), 0);
}


TEST_F(MM_stl_restore, s_set ) { 
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES * attr =  memmgr->ref_attributes("my_alloc.s_set")->attr;

    std::vector<int> test_first= get_test_data<int>(20);
    std::vector<int> test_second = get_test_data<int>(20);

    std::set<std::pair<int,int>> expected;

    std::string * data_links  = (std::string *) memmgr->declare_var("std::string my_alloc_s_set[20]");

    for (int i = 0; i < test_first.size(); i++) {
        data_links[i] = "my_alloc_s_set_" + std::to_string(i);

        std::string first_var_name = "int my_alloc_s_set_" + std::to_string(i) + "_first";
        std::string second_var_name = "int my_alloc_s_set_" + std::to_string(i) + "_second";

        int * temp_first = (int *)memmgr->declare_var(first_var_name.c_str());
        *temp_first = test_first[i];
        int * temp_second = (int *)memmgr->declare_var(second_var_name.c_str());
        *temp_second = test_second[i];

        // Build a copy of the expected data
        expected.insert(std::pair<int,double>(test_first[i], test_second[i]));
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->s_set, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_set, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_set"), 0);
    for (int i = 0; i < test_first.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_set_" + std::to_string(i) + "_first"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_set_" + std::to_string(i) + "_second"), 0);
    }
}

TEST_F(MM_stl_restore, vector_set ) { 
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.vector_set")->attr;

    std::vector<int> test_data = get_test_data<int>(50);
    // Make sure we can handle lists of different sizes
    std::vector<int> lengths = {20, 4, 1, 1, 14, 10};

    // Build a copy of the expected data
    std::set<std::vector<int>> expected;

    // Register the expected temporary variables with the memory manager
    int start_index = 0;
    std::string * nested_list_links = (std::string *) memmgr->declare_var( "std::string my_alloc_vector_set[6]");
    for (int i = 0; i < lengths.size(); i++) {
        nested_list_links[i] = "my_alloc_vector_set_" + std::to_string(i);

        std::string temp_var_name = "int my_alloc_vector_set_" + std::to_string(i) + "[" + std::to_string(lengths[i]) + "]";
        std::vector<int> temp_expected;
        int * temp_arr = (int *)memmgr->declare_var(temp_var_name.c_str());
        for (int j = 0; j < lengths[i]; j++) {
            temp_arr[j] = test_data[start_index + j];
            temp_expected.push_back(test_data[start_index + j]);
        }

        expected.insert(temp_expected);

        start_index += lengths[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->vector_set, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->vector_set, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_vector_set"), 0);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_vector_set_" + std::to_string(i)), 0);
    }
}

TEST_F(MM_stl_restore, nested_map_set ) { 
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.nested_map_set")->attr;

    std::vector<short> test_keys = get_test_data<short>(60);
    std::vector<double> test_vals = get_test_data<double>(60);

    std::set<std::map<short,double>> expected;

    std::string * nested_map_links = (std::string *) memmgr->declare_var( "std::string my_alloc_nested_map_set[6]");
    for (int i = 0; i < 6; i++) {
        nested_map_links[i] = "my_alloc_nested_map_set_" + std::to_string(i);

        std::string keys_var_name = "short my_alloc_nested_map_set_" + std::to_string(i) + "_keys[10]";
        std::string vals_var_name = "double my_alloc_nested_map_set_" + std::to_string(i) + "_data[10]";

        short * map_keys_temp = (short *) memmgr->declare_var( keys_var_name.c_str() );
        double * map_vals_temp = (double *) memmgr->declare_var( vals_var_name.c_str() );

        std::map<short,double> temp_map;
        for (int j = 0; j < 10; j++) {
            map_keys_temp[j] = test_keys[(i*10) + j];
            map_vals_temp[j] = test_vals[(i*10) + j];

            temp_map[test_keys[(i*10) + j]] = test_vals[(i*10) + j];
        }

        expected.emplace(temp_map);
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->nested_map_set, "my_alloc", attr->name) ;

    // ASSERT
    ASSERT_EQ(testbed->nested_map_set, expected);


    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_nested_map_set"), 0);
    for (int i = 0; i < expected.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_map_set_" + std::to_string(i) + "_data"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_map_set_" + std::to_string(i) + "_vals"), 0);
    }
}

// Multiset

TEST_F(MM_stl_restore, i_multiset ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_multiset")->attr;


    std::vector<int> test_data = get_test_data<int>(100);
    std::multiset<int> expected;

    // Make the expected structure first
    for (int val : test_data) {
        expected.insert(val);
        expected.insert(val);
    }

    // Register the expected temporary variables with the memory manager
    std::string var_name = "int my_alloc_i_multiset[" + std::to_string(expected.size()) + "]";
    int * temp_data = (int *) memmgr->declare_var(var_name.c_str());
    int index = 0;
    for (const int val : expected) {
        temp_data[index++] = val;
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_multiset, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_multiset, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_multiset"), 0);
}


TEST_F(MM_stl_restore, s_multiset ) { 
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES * attr =  memmgr->ref_attributes("my_alloc.s_multiset")->attr;

    std::vector<int> test_first= get_test_data<int>(20);
    std::vector<int> test_second = get_test_data<int>(20);

    std::multiset<std::pair<int,int>> expected;

    std::string * data_links  = (std::string *) memmgr->declare_var("std::string my_alloc_s_multiset[40]");

    for (int i = 0; i < test_first.size(); i++) {
        // just insert the same thing twice
        for (int j = 0; j < 2; j++) {
            data_links[i*2+j] = "my_alloc_s_multiset_" + std::to_string(i*2+j);

            std::string first_var_name = "int my_alloc_s_multiset_" + std::to_string(i*2+j) + "_first";
            std::string second_var_name = "int my_alloc_s_multiset_" + std::to_string(i*2+j) + "_second";

            int * temp_first = (int *)memmgr->declare_var(first_var_name.c_str());
            *temp_first = test_first[i];
            int * temp_second = (int *)memmgr->declare_var(second_var_name.c_str());
            *temp_second = test_second[i];

            // Build a copy of the expected data
            expected.insert(std::pair<int,double>(test_first[i], test_second[i]));
        }
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->s_multiset, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->s_multiset, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_s_multiset"), 0);
    for (int i = 0; i < expected.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_multiset_" + std::to_string(i) + "_first"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_s_multiset_" + std::to_string(i) + "_second"), 0);
    }
}

TEST_F(MM_stl_restore, vector_multiset ) { 
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.vector_multiset")->attr;

    std::vector<int> test_data = get_test_data<int>(50);
    // Make sure we can handle lists of different sizes
    std::vector<int> lengths = {20, 4, 1, 1, 14, 10};

    // Build a copy of the expected data
    std::multiset<std::vector<int>> expected;

    // Register the expected temporary variables with the memory manager
    int start_index = 0;
    std::string * nested_list_links = (std::string *) memmgr->declare_var( "std::string my_alloc_vector_multiset[12]");
    for (int i = 0; i < lengths.size(); i++) {
        for (int j = 0; j < 2; j++) {
            nested_list_links[i*2+j] = "my_alloc_vector_multiset_" + std::to_string(i*2+j);

            std::string temp_var_name = "int my_alloc_vector_multiset_" + std::to_string(i*2+j) + "[" + std::to_string(lengths[i]) + "]";
            std::vector<int> temp_expected;
            int * temp_arr = (int *)memmgr->declare_var(temp_var_name.c_str());
            for (int k = 0; k < lengths[i]; k++) {
                temp_arr[k] = test_data[start_index + k];
                temp_expected.push_back(test_data[start_index + k]);
            }

            expected.insert(temp_expected);

        }
        start_index += lengths[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->vector_multiset, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->vector_multiset, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_vector_multiset"), 0);
    for (int i = 0; i < expected.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_vector_multiset_" + std::to_string(i)), 0);
    }
}

TEST_F(MM_stl_restore, nested_map_multiset ) { 
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.nested_map_multiset")->attr;

    std::vector<short> test_keys = get_test_data<short>(60);
    std::vector<double> test_vals = get_test_data<double>(60);

    std::multiset<std::map<short,double>> expected;

    std::string * nested_map_links = (std::string *) memmgr->declare_var( "std::string my_alloc_nested_map_multiset[12]");
    for (int i = 0; i < 6; i++) {
        for (int k = 0; k < 2; k++) {
            nested_map_links[i] = "my_alloc_nested_map_multiset_" + std::to_string(i*2+k);

            std::string keys_var_name = "short my_alloc_nested_map_multiset_" + std::to_string(i*2+k) + "_keys[10]";
            std::string vals_var_name = "double my_alloc_nested_map_multiset_" + std::to_string(i*2+k) + "_data[10]";

            short * map_keys_temp = (short *) memmgr->declare_var( keys_var_name.c_str() );
            double * map_vals_temp = (double *) memmgr->declare_var( vals_var_name.c_str() );

            std::map<short,double> temp_map;
            for (int j = 0; j < 10; j++) {
                map_keys_temp[j] = test_keys[(i*10) + j];
                map_vals_temp[j] = test_vals[(i*10) + j];

                temp_map[test_keys[(i*10) + j]] = test_vals[(i*10) + j];
            }

            expected.emplace(temp_map);
        }
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->nested_map_multiset, "my_alloc", attr->name) ;

    // ASSERT
    ASSERT_EQ(testbed->nested_map_multiset, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_nested_map_multiset"), 0);
    for (int i = 0; i < expected.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_map_multiset_" + std::to_string(i) + "_data"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_nested_map_multiset_" + std::to_string(i) + "_vals"), 0);
    }
}

TEST_F(MM_stl_restore, i_array ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.i_array")->attr;

    std::vector<char> test_data = get_test_data<char>(10);
    std::array<char, 10> expected;

    // Register the expected temporary variables with the memory manager
    char * temp_data = (char *) memmgr->declare_var("char my_alloc_i_array[10]");
    for (int i = 0; i < test_data.size(); i++) {
        temp_data[i] = test_data[i];
        expected[i] = test_data[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->i_array, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->i_array, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_i_array"), 0);
}

TEST_F(MM_stl_restore, pair_array ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES * attr =  memmgr->ref_attributes("my_alloc.pair_array")->attr;

    std::vector<int> test_first= get_test_data<int>(10);
    std::vector<int> test_second = get_test_data<int>(10);

    std::array<std::pair<int,int>, 10> expected;

    std::string * data_links  = (std::string *) memmgr->declare_var("std::string my_alloc_pair_array[10]");

    for (int i = 0; i < test_first.size(); i++) {
        data_links[i] = "my_alloc_pair_array_" + std::to_string(i);

        std::string first_var_name = "int my_alloc_pair_array_" + std::to_string(i) + "_first";
        std::string second_var_name = "int my_alloc_pair_array_" + std::to_string(i) + "_second";

        int * temp_first = (int *)memmgr->declare_var(first_var_name.c_str());
        *temp_first = test_first[i];
        int * temp_second = (int *)memmgr->declare_var(second_var_name.c_str());
        *temp_second = test_second[i];

        // Build a copy of the expected data
        expected[i] = std::pair<int,int>(test_first[i], test_second[i]);
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->pair_array, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->pair_array, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_pair_array"), 0);
    for (int i = 0; i < test_first.size(); i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_pair_array_" + std::to_string(i) + "_first"), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_pair_array_" + std::to_string(i) + "_second"), 0);
    }
}

TEST_F(MM_stl_restore, string_array ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.string_array")->attr;

    std::vector<std::string> test_data = get_test_data<std::string>(10);
    std::array<std::string, 10> expected;

    // Register the expected temporary variables with the memory manager
    std::string * temp_data = (std::string *) memmgr->declare_var("std::string my_alloc_string_array[10]");
    for (int i = 0; i < test_data.size(); i++) {
        temp_data[i] = std::string(test_data[i]);
        expected[i] = std::string(test_data[i]);
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->string_array, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->string_array, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_string_array"), 0);
}

TEST_F(MM_stl_restore, vec_array ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.vec_array")->attr;


    std::vector<int> test_data = get_test_data<int>(50);
    std::vector<int> lengths = {12, 4, 1, 1, 14, 10, 2, 2, 2, 2};

    // Register the expected temporary variables with the memory manager
    std::string my_alloc_vec_array[10];
    int start_index = 0;
    memmgr->declare_extern_var(&my_alloc_vec_array, "std::string my_alloc_vec_array[10]");
    for (int i = 0; i < lengths.size(); i++) {
        // std::string var_name = 
        my_alloc_vec_array[i] = "my_alloc_vec_array_" + std::to_string(i);

        std::string temp_var_name = "int my_alloc_vec_array_" + std::to_string(i) + "[" + std::to_string(lengths[i]) + "]";
        int * temp_arr = (int *)memmgr->declare_var(temp_var_name.c_str());
        for (int j = 0; j < lengths[i]; j++) {
            temp_arr[j] = test_data[start_index + j];
        }

        start_index += lengths[i];
    }

    // ACT
    (attr->restore_stl)((void *) &testbed->vec_array, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    ASSERT_EQ(testbed->vec_array.size(), lengths.size());
    start_index = 0;
    for (int i = 0; i < lengths.size(); i++) {
        ASSERT_EQ(testbed->vec_array[i].size(), lengths[i]); 
        EXPECT_EQ(testbed->vec_array[i], std::vector<int>(test_data.begin()+start_index, test_data.begin()+start_index+lengths[i]));
        start_index += lengths[i];
    }

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_vec_array"), 0);
    for (int i = 0; i < test_data.size()/2; i++) {
        EXPECT_EQ(memmgr->var_exists("my_alloc_vec_array_" + std::to_string(i)), 0);
        EXPECT_EQ(memmgr->var_exists("my_alloc_vec_array_" + std::to_string(i)), 0);
    }
}

TEST_F(MM_stl_restore, vec_user_defined ) {
    // ARRANGE
    // make a testbed object
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");  
    ATTRIBUTES* attr =  memmgr->ref_attributes("my_alloc.vec_user_defined")->attr;

    int numObj = 10;
    std::vector<int> test_ints = get_test_data<int>(5 * numObj);
    std::vector<long long> test_longs = get_test_data<long long>(numObj);
    std::vector<std::string> test_strings = get_test_data<std::string>(numObj);

    // Register the expected temporary variables with the memory manager
    UserClass * data = (UserClass *) memmgr->declare_var("UserClass my_alloc_vec_user_defined[10]");

    // Create an structure holding the expected final container
    std::vector<UserClass> expected;

    for (int i = 0; i < numObj; i++) {
        UserClass temp;
        for (int j = 0; j < 5; j++) {
            temp.a[j] = test_ints[i*5+j];
            data[i].a[j] = test_ints[i*5+j];
        }
        data[i].b = test_longs[i];
        data[i].c = test_strings[i];

        temp.b = test_longs[i];
        temp.c = test_strings[i];

        // just make a pointer to another one with the same data
        std::string var_decl = "UserClass ptr_" + std::to_string(i);
        UserClass * user_class_ptr = (UserClass *) memmgr->declare_var(var_decl.c_str());
        UserClass * expected_ptr = new UserClass();

        for (int j = 0; j < 5; j++) {
            user_class_ptr->a[j] = test_ints[i*5+j];
            expected_ptr->a[j] = test_ints[i*5+j];

        }
        user_class_ptr->b = test_longs[i];
        user_class_ptr->c = test_strings[i];

        expected_ptr->b = test_longs[i];
        expected_ptr->c = test_strings[i];

        data[i].d = user_class_ptr;
        temp.d = expected_ptr;

        expected.emplace_back(temp);
    }


    // ACT
    (attr->restore_stl)((void *) &testbed->vec_user_defined, "my_alloc", attr->name) ;

    // ASSERT
    // Make sure the STL has been populated
    EXPECT_EQ(testbed->vec_user_defined, expected);

    // Check that all the temporary variables have been deleted
    EXPECT_EQ(memmgr->var_exists("my_alloc_vec_user_defined"), 0);
}


TEST_F(MM_stl_restore, recursive_nightmare ) {
}
