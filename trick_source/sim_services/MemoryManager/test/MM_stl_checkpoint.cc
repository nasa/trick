#include "gtest/gtest.h"
#define private public

#include "trick/MemoryManager.hh"
#include "trick/memorymanager_c_intf.h"
#include "MM_test.hh"
#include "MM_stl_testbed.hh"

/*
 This tests the implementations of checkpoint_stl
 */
class MM_stl_checkpoint : public ::testing::Test {

protected:
    Trick::MemoryManager *memmgr;
    
    MM_stl_checkpoint() {
        try {
            memmgr = new Trick::MemoryManager;
        } catch (std::logic_error e) {
            memmgr = NULL;
        }
    }
    ~MM_stl_checkpoint() {
        delete memmgr;
    }
    void SetUp() {}
    void TearDown() {} 
};

template <typename T>
void validate_single (Trick::MemoryManager * memmgr, std::string object_name, std::string var_name, T expected_data) {
    std::string temp_name = object_name + "_" + var_name;
    ASSERT_TRUE(memmgr->var_exists(temp_name) == 1);

    REF2 * data_ref = memmgr->ref_attributes(temp_name.c_str());
    T * data = (T *) data_ref->address;

    ASSERT_TRUE(data != NULL);

    EXPECT_EQ(*data, expected_data);
    
    free (data_ref);
}

template <typename T>
void validate_temp_sequence (Trick::MemoryManager * memmgr, std::string object_name, std::string var_name, std::vector<T> expected_data) {
    std::string temp_name = object_name + "_" + var_name;
    ASSERT_TRUE(memmgr->var_exists(temp_name) == 1);

    REF2 * data_ref = memmgr->ref_attributes(temp_name.c_str());
    T * data = (T *) data_ref->address;

    ASSERT_TRUE(data != NULL);
    EXPECT_EQ(data_ref->attr->num_index, 1);
    ASSERT_EQ(data_ref->attr->index[0].size, expected_data.size());

    for (int i = 0; i < expected_data.size(); i++) {
        EXPECT_EQ(data[i], expected_data[i]);
    }

    free (data_ref);

}

void validate_links_sequences (Trick::MemoryManager * memmgr, std::string object_name, std::string top_level_name, std::vector<int> lengths) {
    std::string temp_name = object_name + "_" + top_level_name;
    ASSERT_TRUE(memmgr->var_exists(temp_name) == 1);

    REF2 * data_ref = memmgr->ref_attributes(temp_name.c_str());
    std::string * data = (std::string *) data_ref->address;
    ASSERT_EQ(data_ref->attr->index[0].size, lengths.size());

    for (int i = 0; i < lengths.size(); i++) {
        std::string inner_name = temp_name + "_" + std::to_string(i);
        EXPECT_EQ(inner_name, data[i]);
        REF2 * ref = memmgr->ref_attributes(inner_name.c_str());
        ASSERT_EQ(ref->attr->index[0].size, lengths[i]);
    }
}

void validate_links_pairs (Trick::MemoryManager * memmgr, std::string object_name, std::string top_level_name, int num_pairs) {
    std::string temp_name = object_name + "_" + top_level_name;
    ASSERT_TRUE(memmgr->var_exists(temp_name) == 1);

    REF2 * data_ref = memmgr->ref_attributes(temp_name.c_str());
    std::string * data = (std::string *) data_ref->address;
    ASSERT_EQ(data_ref->attr->index[0].size, num_pairs);

    for (int i = 0; i < num_pairs; i++) {
        std::string inner_name = temp_name + "_" + std::to_string(i);
        EXPECT_EQ(inner_name, data[i]);
        std::string first_name = inner_name + "_first";
        std::string second_name = inner_name + "_second";

        ASSERT_TRUE(memmgr->var_exists(first_name) == 1);
        ASSERT_TRUE(memmgr->var_exists(second_name) == 1);
    }
}

void validate_link_from_pair (Trick::MemoryManager * memmgr, std::string object_name, std::string top_level_name) {
    std::string temp_name = object_name + "_" + top_level_name;
    ASSERT_TRUE(memmgr->var_exists(temp_name) == 1);

    REF2 * link_ref = memmgr->ref_attributes(temp_name.c_str());
    std::string * link_name = (std::string *) link_ref->address;
    EXPECT_EQ(link_name[0], "inner");
}


template <typename T>
void validate_temp_set (Trick::MemoryManager * memmgr, std::string object_name, std::string var_name, std::set<T> expected_data) {
    std::string temp_name = object_name + "_" + var_name;
    ASSERT_TRUE(memmgr->var_exists(temp_name) == 1);

    REF2 * data_ref = memmgr->ref_attributes(temp_name.c_str());
    T * data = (T *) data_ref->address;

    ASSERT_TRUE(data != NULL);
    EXPECT_EQ(data_ref->attr->num_index, 1);
    ASSERT_EQ(data_ref->attr->index[0].size, expected_data.size());

    std::set<T> reconstructed;
    for (int i = 0; i < expected_data.size(); i++) {
        reconstructed.insert(data[i]);
    }
    
    EXPECT_EQ(reconstructed, expected_data);

    free (data_ref);

}


template <typename First, typename Second>
void validate_temp_pair (Trick::MemoryManager * memmgr, std::string object_name, std::string var_name, std::pair<First, Second> expected_data) {
    std::string temp_name = object_name + "_" + var_name;
    std::string first_name = temp_name + "_first";
    std::string second_name = temp_name + "_second";

    ASSERT_TRUE(memmgr->var_exists(first_name) == 1);
    ASSERT_TRUE(memmgr->var_exists(second_name) == 1);

    REF2 * first_data_ref = memmgr->ref_attributes(first_name.c_str());
    First * first_data = (First *) first_data_ref->address;

    REF2 * second_data_ref = memmgr->ref_attributes(second_name.c_str());
    Second * second_data = (Second *) second_data_ref->address;

    ASSERT_TRUE(first_data != NULL);
    ASSERT_TRUE(second_data != NULL);

    EXPECT_EQ(expected_data.first, *first_data);
    EXPECT_EQ(expected_data.second, *second_data);

    free (first_data_ref);
    free (second_data_ref);
}


template <typename Key, typename Val>
void validate_temp_map (Trick::MemoryManager * memmgr, std::string object_name, std::string var_name, std::map<Key, Val> expected_data) {
    std::string temp_name = object_name + "_" + var_name;
    std::string keys_name = temp_name + "_keys";
    std::string vals_name = temp_name + "_data";

    ASSERT_TRUE(memmgr->var_exists(keys_name) == 1);
    ASSERT_TRUE(memmgr->var_exists(vals_name) == 1);

    REF2 * keys_data_ref = memmgr->ref_attributes(keys_name.c_str());
    Key * keys_data = (Key *) keys_data_ref->address;

    REF2 * vals_data_ref = memmgr->ref_attributes(vals_name.c_str());
    Val * vals_data = (Val *) vals_data_ref->address;

    ASSERT_TRUE(keys_data != NULL);
    ASSERT_TRUE(vals_data != NULL);

    EXPECT_EQ(keys_data_ref->attr->num_index, 1);
    EXPECT_EQ(vals_data_ref->attr->num_index, 1);

    ASSERT_EQ(keys_data_ref->attr->index[0].size, expected_data.size());
    ASSERT_EQ(vals_data_ref->attr->index[0].size, expected_data.size());

    // This is a little tricky - i don't think we can rely on the order being
    // the same, but want to check that the data is exactly the same.
    // Lazy way - make a set of all the keys that were accessed in our expected
    // map, and make sure that the size is the same.
    std::set <Key> keySet;
    for (int i = 0; i < expected_data.size(); i++) {
        Key k = keys_data[i];
        Val v = vals_data[i];

        // To check for completeness, like if we accidentally wrote out duplicate keys but the size was the same
        keySet.insert(k);

        // These should be a pair in the expected map
        auto result = expected_data.find(k);
        ASSERT_TRUE(result != expected_data.end());
        EXPECT_EQ(result->second, v);
    }

    ASSERT_EQ(keySet.size(), expected_data.size());

    free (keys_data_ref);
    free (vals_data_ref);
}

// Vectors

TEST_F(MM_stl_checkpoint, i_vec ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->i_vec.push_back(test_data[i]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_vec")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_vec, "my_alloc", vec_attr->name) ;

    // ASSERT
    validate_temp_sequence<int>(memmgr, std::string("my_alloc"), std::string("i_vec"), test_data);
}

TEST_F(MM_stl_checkpoint, i_s_vec ) {
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<std::vector<int>> test_data;
    for (int i = 0; i < 10; i++) {
        std::vector<int> test_data_inner = get_test_data<int>(10);
        testbed->i_s_vec.emplace_back(test_data_inner);
        test_data.emplace_back(test_data_inner);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_s_vec")->attr;

    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_s_vec, "my_alloc", vec_attr->name) ;

    for (int i = 0; i < test_data.size(); i++) {
        std::string var_name = "i_s_vec_" + std::to_string(i);
        validate_temp_sequence<int>(memmgr, std::string("my_alloc"), var_name, test_data[i]);
    }

    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("i_s_vec"), std::vector<int>(10, 10));
}

TEST_F(MM_stl_checkpoint, string_vec ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<std::string> test_data = get_test_data<std::string>(20);
    for (int i = 0; i < test_data.size(); i++) {
        testbed->string_vec.emplace_back(test_data[i]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.string_vec")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->string_vec, "my_alloc", vec_attr->name) ;
    std::string var_name = "string_vec";
    
    // ASSERT
    validate_temp_sequence<std::string>(memmgr, std::string("my_alloc"), std::string("string_vec"), test_data);   
}

TEST_F(MM_stl_checkpoint, s_vec ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(20);
    for (int i = 0; i < test_data.size(); i+=2) {
        testbed->s_vec.emplace_back(test_data[i], test_data[i+1]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_vec")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_vec, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    for (int i = 0; i < test_data.size()/2; i++) {
        std::string var_name = "s_vec_" + std::to_string(i);
        validate_temp_pair<int, int>(memmgr, std::string("my_alloc"), var_name, std::pair<int,int>(test_data[i*2], test_data[i*2+1]));   
    }

    validate_links_pairs(memmgr, std::string("my_alloc"), std::string("s_vec"), test_data.size()/2);
}


// Pairs
// TODO: Nested sequences do not work

TEST_F(MM_stl_checkpoint, i_i_pair ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<double> test_data = get_test_data<double>(2);
    testbed->i_i_pair.first = test_data[0];
    testbed->i_i_pair.second = (float) test_data[1];

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_i_pair")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_i_pair, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_temp_pair<double,float>(memmgr, std::string("my_alloc"), vec_attr->name, std::pair<double,float>(test_data[0], test_data[1]));  
}

TEST_F(MM_stl_checkpoint, i_s_pair ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<double> test_key = get_test_data<double>(1);
    std::vector<bool> test_val = get_test_data<bool>(20);

    testbed->i_s_pair.first = test_key[0];
    for (bool val : test_val) {
        testbed->i_s_pair.second.push(val);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_s_pair")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_s_pair, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_temp_sequence<bool>(memmgr, std::string("my_alloc"), std::string("i_s_pair_second_inner"), test_val);  
    validate_link_from_pair(memmgr, std::string("my_alloc"), std::string("i_s_pair_second"));
}

TEST_F(MM_stl_checkpoint, s_i_pair ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<double> test_val = get_test_data<double>(1);
    std::vector<bool> test_key = get_test_data<bool>(20);

    testbed->s_i_pair.second = test_val[0];
    for (bool val : test_key) {
        testbed->s_i_pair.first.push(val);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_i_pair")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_i_pair, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_temp_sequence<bool>(memmgr, std::string("my_alloc"), std::string("s_i_pair_first_inner"), test_key);  
    validate_link_from_pair(memmgr, std::string("my_alloc"), std::string("s_i_pair_first"));
}



TEST_F(MM_stl_checkpoint, i_v_pair ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<double> test_key = get_test_data<double>(1);
    std::vector<bool> test_val = get_test_data<bool>(20);

    testbed->i_v_pair.first = test_key[0];
    for (bool val : test_val) {
        testbed->i_v_pair.second.push_back(val);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_v_pair")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_v_pair, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_temp_sequence<bool>(memmgr, std::string("my_alloc"), std::string("i_v_pair_second_inner"), test_val);  
    validate_link_from_pair(memmgr, std::string("my_alloc"), std::string("i_v_pair_second"));
}


TEST_F(MM_stl_checkpoint, pair_pair ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    testbed->pair_pair.first.first = 1;
    testbed->pair_pair.first.second = 2;

    testbed->pair_pair.second = 3;


    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.pair_pair")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->pair_pair, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_single(memmgr, std::string("my_alloc"), std::string("pair_pair_second"), 3);
    validate_temp_pair(memmgr, std::string("my_alloc"), std::string("pair_pair_first_inner"), std::pair<int,int>(1, 2));
    validate_link_from_pair(memmgr, std::string("my_alloc"), std::string("pair_pair_first"));
}



// Maps

TEST_F(MM_stl_checkpoint, i_i_map ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_keys = get_test_data<int>(20);
    std::vector<double> test_vals = get_test_data<double>(20);
    std::map<int, double> test_map;

    for (int i = 0; i < test_keys.size(); i++) {
        test_map[test_keys[i]] = test_vals[i];
        testbed->i_i_map[test_keys[i]] = test_vals[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_i_map")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_i_map, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_temp_map(memmgr, std::string("my_alloc"), std::string("i_i_map"), test_map);  
}

TEST_F(MM_stl_checkpoint, i_s_map ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_keys = get_test_data<int>(3);
    std::vector<std::string> test_strings = get_test_data<std::string>(20);
    std::map<int, std::stack<std::string>> test_map;

    int start_index = 0;
    std::vector<int> lengths = {7, 1, 12};
    for (int i = 0; i < lengths.size(); i++) {
        for (int j = 0; j < lengths[i]; j++) {
            testbed->i_s_map[test_keys[i]].push(test_strings[start_index + j]);
            test_map[test_keys[i]].push(test_strings[start_index + j]);
        }
        start_index += lengths[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_s_map")->attr;
    std::set<int> keyset (test_keys.begin(), test_keys.end());

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_s_map, "my_alloc", vec_attr->name) ;
    
    // ASSERT
    validate_temp_set(memmgr, std::string("my_alloc"), std::string("i_s_map_keys"), keyset);

    std::vector<int> sorted_lengths;
    int i = 0;
    for (auto it : test_map) {
        std::vector<std::string> expected_data;
        while (!it.second.empty()) {
            expected_data.push_back(it.second.top());
            it.second.pop();
        }        
        std::string var_name = "i_s_map_data_" + std::to_string(i++);
        validate_temp_sequence(memmgr, std::string("my_alloc"), var_name, expected_data);
        sorted_lengths.push_back(expected_data.size());
    }

    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("i_s_map_data"), sorted_lengths);
}


TEST_F(MM_stl_checkpoint, s_i_map ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_keys = get_test_data<int>(20);
    std::vector<std::string> test_vals = get_test_data<std::string>(3);

    std::map<std::set<int>, std::string> test_map;

    std::vector<int> lengths = {7, 1, 12};

    int start_index = 0;
    for (int i = 0; i < lengths.size(); i++) {
        test_map[std::set<int>(test_keys.begin()+start_index, test_keys.begin()+start_index+lengths[i])] = test_vals[i];
        testbed->s_i_map[std::set<int>(test_keys.begin()+start_index, test_keys.begin()+start_index+lengths[i])] = test_vals[i];
        start_index += lengths[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_i_map")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_i_map, "my_alloc", vec_attr->name) ;

    
    // the ordering of the map is making life hard
    // ASSERT
    int i = 0;
    std::vector<std::string> values_in_the_right_order;
    std::vector<int> lengths_in_the_right_order;

    for (auto it : test_map) {
        std::string var_name = "s_i_map_keys_" + std::to_string(i++);
        validate_temp_set(memmgr, std::string("my_alloc"), var_name, it.first);
        values_in_the_right_order.push_back(it.second);
        lengths_in_the_right_order.push_back(it.first.size());
    }
    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("s_i_map_keys"), lengths_in_the_right_order);
    validate_temp_sequence(memmgr, std::string("my_alloc"), std::string("s_i_map_data"), values_in_the_right_order);
}

TEST_F(MM_stl_checkpoint, s_s_map ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_keys = get_test_data<int>(20);
    std::vector<int> test_vals = get_test_data<int>(40);

    std::map<std::pair<int,int>, std::vector<int>> test_map;

    for (int i = 0; i < 10; i++) {
        testbed->s_s_map[std::pair<int,int>(test_keys[i*2], test_keys[(i*2)+1])] = std::vector<int>(test_vals.begin() + (i*4), test_vals.begin() + (i+1)*4);
        test_map[std::pair<int,int>(test_keys[i*2], test_keys[(i*2)+1])] = std::vector<int>(test_vals.begin() + (i*4), test_vals.begin() + (i+1)*4);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_s_map")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_s_map, "my_alloc", vec_attr->name) ;

    // the ordering of the map is making life hard
    // ASSERT
    int i = 0;
    std::vector<int> data_lengths;
    for (auto it : test_map) {
        std::pair<int,int> expected_key = it.first;
        std::string key_name = "s_s_map_keys_" + std::to_string(i);
        validate_temp_pair(memmgr, std::string("my_alloc"), key_name, expected_key);

        std::vector<int> expected_data = it.second;
        std::string data_name = "s_s_map_data_" + std::to_string(i);
        validate_temp_sequence(memmgr, std::string("my_alloc"), data_name, expected_data);

        data_lengths.push_back(expected_data.size());
        i++;
    }

    validate_links_pairs(memmgr, std::string("my_alloc"), std::string("s_s_map_keys"), test_map.size());
    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("s_s_map_data"), data_lengths);
}


// Queue


TEST_F(MM_stl_checkpoint, i_queue ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->i_queue.push(test_data[i]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_queue")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_queue, "my_alloc", vec_attr->name) ;

    // ASSERT
    validate_temp_sequence<int>(memmgr, std::string("my_alloc"), std::string("i_queue"), test_data);
}

TEST_F(MM_stl_checkpoint, s_queue ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_first = get_test_data<int>(10);
    std::vector<double> test_second = get_test_data<double>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_first.size(); i++) {
        testbed->s_queue.push(std::pair<int,double>(test_first[i], test_second[i]));
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_queue")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_queue, "my_alloc", vec_attr->name) ;

    // ASSERT
    for (int i = 0; i < test_first.size(); i++) {
        std::string var_name = "s_queue_" + std::to_string(i);
        validate_temp_pair(memmgr, std::string("my_alloc"), var_name, std::pair<int,double>(test_first[i], test_second[i]));
    }
    validate_links_pairs(memmgr, std::string("my_alloc"), std::string("s_queue"), test_first.size());
}


TEST_F(MM_stl_checkpoint, nested_list_queue ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<float> test_data = get_test_data<float>(10);
    std::vector<int> list_sizes = {2, 7, 1};
    // Prepare the STL to be tested

    int data_index = 0;
    for (int i = 0; i < list_sizes.size(); i++) {
        testbed->nested_list_queue.push(std::list<float>(test_data.begin() + data_index, test_data.begin() + data_index + list_sizes[i]));
        data_index += list_sizes[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.nested_list_queue")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->nested_list_queue, "my_alloc", vec_attr->name) ;

    // ASSERT
    data_index = 0;
    for (int i = 0; i < list_sizes.size(); i++) {
        std::string var_name = "nested_list_queue_" + std::to_string(i);
        validate_temp_sequence(memmgr, std::string("my_alloc"), var_name, std::vector<float>(test_data.begin() + data_index, test_data.begin() + data_index + list_sizes[i]));
        data_index += list_sizes[i];
    }

    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("nested_list_queue"), list_sizes);
}



// Stacks

TEST_F(MM_stl_checkpoint, i_stack ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<long long> test_data = get_test_data<long long>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->i_stack.push(test_data[i]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_stack")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_stack, "my_alloc", vec_attr->name) ;

    // ASSERT
    // Have the pass the test data in reverse order
    validate_temp_sequence<long long>(memmgr, std::string("my_alloc"), std::string("i_stack"), std::vector<long long> (test_data.rbegin(), test_data.rend()));
}

TEST_F(MM_stl_checkpoint, s_stack ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<short> test_first = get_test_data<short>(10);
    std::vector<double> test_second = get_test_data<double>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_first.size(); i++) {
        testbed->s_stack.push(std::pair<short,double>(test_first[i], test_second[i]));
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_stack")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_stack, "my_alloc", vec_attr->name) ;

    // ASSERT
    for (int i = 0; i < test_first.size(); i++) {
        std::string var_name = "s_stack_" + std::to_string(i);
        // data in reverse order
        validate_temp_pair(memmgr, std::string("my_alloc"), var_name, std::pair<short,double>(test_first[test_first.size()-1-i], test_second[test_first.size()-1-i]));
    }

    validate_links_pairs(memmgr, std::string("my_alloc"), std::string("s_stack"), test_first.size());
}


TEST_F(MM_stl_checkpoint, nested_list_stack ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<float> test_data = get_test_data<float>(10);
    std::vector<int> list_sizes = {2, 7, 1};
    // Prepare the STL to be tested

    int data_index = 0;
    for (int i = 0; i < list_sizes.size(); i++) {
        testbed->nested_list_stack.push(std::list<float>(test_data.begin() + data_index, test_data.begin() + data_index + list_sizes[i]));
        data_index += list_sizes[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.nested_list_stack")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->nested_list_stack, "my_alloc", vec_attr->name) ;

    // ASSERT
    data_index = 0;
    for (int i = 0; i < list_sizes.size(); i++) {
        // reverse data - just go through the lists backwards
        std::string var_name = "nested_list_stack_" + std::to_string(list_sizes.size()-1-i);
        validate_temp_sequence(memmgr, std::string("my_alloc"), var_name, std::vector<float>(test_data.begin() + data_index, test_data.begin() + data_index + list_sizes[i]));
        data_index += list_sizes[i];
    }

    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("nested_list_stack"), std::vector<int>(list_sizes.rbegin(), list_sizes.rend()));
}


TEST_F(MM_stl_checkpoint, i_set ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    // Likely to be some repeats in here
    std::vector<char> test_data = get_test_data<char>(1000);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->i_set.insert(test_data[i]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_set")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_set, "my_alloc", vec_attr->name) ;

    // ASSERT
    validate_temp_set<char>(memmgr, std::string("my_alloc"), std::string("i_set"), std::set<char> (test_data.begin(), test_data.end()));
}

TEST_F(MM_stl_checkpoint, vector_set ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<char> test_data = get_test_data<char>(1000);
    std::vector<int> sizes = {500, 250, 125, 62, 31, 15, 7, 3, 1, 6};
    // The ordering is confusing on these, so just make a copy and use that
    std::set<std::vector<int>> expected_data;
    // Prepare the STL to be tested
    int start_index = 0;
    for (int i = 0; i < sizes.size(); i++) {
        std::vector<int> temp_vec;
        for (int j = 0; j < sizes[i]; j++) {
            temp_vec.push_back(test_data[start_index+j]);
        }
        testbed->vector_set.insert(std::vector<int>(temp_vec));
        expected_data.insert(std::vector<int>(temp_vec));

        start_index += sizes[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.vector_set")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->vector_set, "my_alloc", vec_attr->name) ;

    // ASSERT
    int index = 0;
    std::vector<int> sizes_in_right_order;
    for (std::vector<int> vec : expected_data) {
        std::string var_name = "vector_set_" + std::to_string(index++); 
        validate_temp_sequence<int>(memmgr, std::string("my_alloc"), var_name, vec);
        sizes_in_right_order.push_back(vec.size());
    }
    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("vector_set"), sizes_in_right_order);
    // validate_temp_set<char>(memmgr, std::string("my_alloc"), std::string("vector_set"), std::set<char> (test_data.begin(), test_data.end()));
}

TEST_F(MM_stl_checkpoint, s_set ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_first = get_test_data<int>(10);
    std::vector<int> test_second = get_test_data<int>(10);

    // make a copy to deal with ordering
    std::set<std::pair<int,int>> expected;

    // Prepare the STL to be tested
    for (int i = 0; i < test_first.size(); i++) {
        testbed->s_set.insert(std::pair<int,int>(test_first[i], test_second[i]));
        expected.insert(std::pair<int,int>(test_first[i], test_second[i]));
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_set")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_set, "my_alloc", vec_attr->name) ;

    // ASSERT
    int index = 0;
    for (std::pair<int,int> expected_pair : expected) {
        std::string var_name = "s_set_" + std::to_string(index++);
        validate_temp_pair(memmgr, std::string("my_alloc"), var_name, std::pair<int,int>(expected_pair.first, expected_pair.second));
    }
    validate_links_pairs(memmgr, "my_alloc", "s_set", test_first.size());
}


TEST_F(MM_stl_checkpoint, nested_map_set ) {
    // // ARRANGE
    // // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<short> test_keys = get_test_data<short>(100);
    std::vector<double> test_vals = get_test_data<double>(100);
    // Prepare the STL to be tested
    
    // Make a copy to deal with ordering mess
    std::set<std::map<short,double>> expected;

    for (int i = 0; i < 10; i++) {
        std::map<short,double> temp_map;
        for (int j = 0; j < 10; j++) {
            temp_map[test_keys[i*10+j]] = test_vals[i*10+j];
        }
        expected.emplace(temp_map);
        testbed->nested_map_set.emplace(temp_map);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.nested_map_set")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->nested_map_set, "my_alloc", vec_attr->name) ;

    // ASSERT
    int index = 0;
    for (std::map<short,double> expected_map : expected) {
        std::string var_name = "nested_map_set_" + std::to_string(index++);
        validate_temp_map(memmgr, "my_alloc", var_name, expected_map);
    }

    // Validate links?
}

// Multisets

TEST_F(MM_stl_checkpoint, i_multiset ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    // Likely to be some repeats in here
    std::vector<int> test_data = get_test_data<int>(1000);

    std::multiset<int> expected;

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->i_multiset.insert(test_data[i]);
        testbed->i_multiset.insert(test_data[i]);
        expected.insert(test_data[i]);
        expected.insert(test_data[i]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_multiset")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_multiset, "my_alloc", vec_attr->name) ;

    // ASSERT
    validate_temp_sequence<int>(memmgr, std::string("my_alloc"), std::string("i_multiset"), std::vector<int> (expected.begin(), expected.end()));
}

TEST_F(MM_stl_checkpoint, vector_multiset ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(1000);
    std::vector<int> sizes = {500, 250, 125, 62, 31, 15, 7, 3, 1, 6};
    // The ordering is confusing on these, so just make a copy and use that
    std::multiset<std::vector<int>> expected_data;
    // Prepare the STL to be tested
    int start_index = 0;
    for (int i = 0; i < sizes.size(); i++) {
        std::vector<int> temp_vec;
        for (int j = 0; j < sizes[i]; j++) {
            temp_vec.push_back(test_data[start_index+j]);
        }
        // just add 2?
        testbed->vector_multiset.insert(std::vector<int>(temp_vec));
        testbed->vector_multiset.insert(std::vector<int>(temp_vec));
        expected_data.insert(std::vector<int>(temp_vec));
        expected_data.insert(std::vector<int>(temp_vec));

        start_index += sizes[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.vector_multiset")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->vector_multiset, "my_alloc", vec_attr->name) ;

    // ASSERT
    int index = 0;
    std::vector<int> sizes_in_right_order;
    for (std::vector<int> vec : expected_data) {
        std::string var_name = "vector_multiset_" + std::to_string(index++); 
        validate_temp_sequence<int>(memmgr, std::string("my_alloc"), var_name, vec);
        sizes_in_right_order.push_back(vec.size());
    }
    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("vector_multiset"), sizes_in_right_order);
}

TEST_F(MM_stl_checkpoint, s_multiset ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_first = get_test_data<int>(10);
    std::vector<int> test_second = get_test_data<int>(10);

    // make a copy to deal with ordering
    std::multiset<std::pair<int,int>> expected;

    // Prepare the STL to be tested
    for (int i = 0; i < test_first.size(); i++) {
        testbed->s_multiset.insert(std::pair<int,int>(test_first[i], test_second[i]));
        testbed->s_multiset.insert(std::pair<int,int>(test_first[i], test_second[i]));
        expected.insert(std::pair<int,int>(test_first[i], test_second[i]));
        expected.insert(std::pair<int,int>(test_first[i], test_second[i]));
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.s_multiset")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->s_multiset, "my_alloc", vec_attr->name) ;

    // ASSERT
    int index = 0;
    for (std::pair<int,int> expected_pair : expected) {
        std::string var_name = "s_multiset_" + std::to_string(index++);
        validate_temp_pair(memmgr, std::string("my_alloc"), var_name, std::pair<int,int>(expected_pair.first, expected_pair.second));
    }
    validate_links_pairs(memmgr, "my_alloc", "s_multiset", expected.size());
}


TEST_F(MM_stl_checkpoint, nested_map_multiset ) {
    // // ARRANGE
    // // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<short> test_keys = get_test_data<short>(100);
    std::vector<double> test_vals = get_test_data<double>(100);
    // Prepare the STL to be tested
    
    // Make a copy to deal with ordering mess
    std::multiset<std::map<short,double>> expected;

    for (int i = 0; i < 10; i++) {
        std::map<short,double> temp_map;
        for (int j = 0; j < 10; j++) {
            temp_map[test_keys[i*10+j]] = test_vals[i*10+j];
        }
        expected.emplace(temp_map);
        expected.emplace(temp_map);
        testbed->nested_map_multiset.emplace(temp_map);
        testbed->nested_map_multiset.emplace(temp_map);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.nested_map_multiset")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->nested_map_multiset, "my_alloc", vec_attr->name) ;

    // ASSERT
    int index = 0;
    for (std::map<short,double> expected_map : expected) {
        std::string var_name = "nested_map_multiset_" + std::to_string(index++);
        validate_temp_map(memmgr, "my_alloc", var_name, expected_map);
    }

    // Validate links?
}

// Arrays


TEST_F(MM_stl_checkpoint, i_array ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<char> test_data = get_test_data<char>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->i_array[i] = test_data[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.i_array")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->i_array, "my_alloc", vec_attr->name) ;

    // ASSERT
    validate_temp_sequence<char>(memmgr, std::string("my_alloc"), std::string("i_array"), test_data);
}


TEST_F(MM_stl_checkpoint, pair_array ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(20);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i+=2) {
        testbed->pair_array[i/2] = std::pair<int,int>(test_data[i],test_data[i+1]);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.pair_array")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->pair_array, "my_alloc", vec_attr->name) ;

    // ASSERT
    for (int i = 0; i < test_data.size(); i+=2) {
        std::string var_name = "pair_array_" + std::to_string(i/2);
        validate_temp_pair(memmgr, std::string("my_alloc"), var_name, std::pair<int,int>(test_data[i],test_data[i+1]));
    }

    validate_links_pairs(memmgr, std::string("my_alloc"), std::string("pair_array"), test_data.size()/2);
}


TEST_F(MM_stl_checkpoint, string_array ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<std::string> test_data = get_test_data<std::string>(10);

    // Prepare the STL to be tested
    for (int i = 0; i < test_data.size(); i++) {
        testbed->string_array[i] = test_data[i];
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.string_array")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->string_array, "my_alloc", vec_attr->name) ;

    // ASSERT
    validate_temp_sequence(memmgr, std::string("my_alloc"), std::string("string_array"), test_data);
}


TEST_F(MM_stl_checkpoint, vec_array ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(100);

    // Prepare the STL to be tested
    for (int i = 0; i < 10; i++) {
        // testbed->vec_array[i] = std::vector<std::string>(test_data.begin() + (i*10), test_data.begin()+(i+1)*10);
        testbed->vec_array[i].insert(testbed->vec_array[i].end(), test_data.begin() + (i*10), test_data.begin() +((i+1)*10));
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.vec_array")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->vec_array, "my_alloc", vec_attr->name) ;

    // ASSERT
    for (int i = 0; i < 10; i++) {
        std::string var_name = "vec_array_" + std::to_string(i);
        validate_temp_sequence(memmgr, std::string("my_alloc"), var_name, std::vector<int>(test_data.begin() + (i*10), test_data.begin() +((i+1)*10)));
    }

    validate_links_sequences(memmgr, std::string("my_alloc"), std::string("vec_array"), std::vector<int>(10, 10));
}

TEST_F(MM_stl_checkpoint, vec_user_defined ) {
    // Make a testbed
    // ARRANGE
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.vec_user_defined")->attr;

    int numObj = 10;
    std::vector<int> test_ints = get_test_data<int>(5 * numObj);
    std::vector<long long> test_longs = get_test_data<long long>(numObj);
    std::vector<std::string> test_strings = get_test_data<std::string>(numObj);

    for (int i = 0; i < numObj; i++) {
        UserClass temp;
        for (int j = 0; j < 5; j++) {
            temp.a[j] = test_ints[i*5+j];
        }
        temp.b = test_longs[i];
        temp.c = test_strings[i];

        // just make a pointer to another one with the same data
        std::string var_decl = "UserClass ptr_" + std::to_string(i);
        UserClass * user_class_ptr = (UserClass *) memmgr->declare_var(var_decl.c_str());
        for (int j = 0; j < 5; j++) {
            user_class_ptr->a[j] = test_ints[i*5+j];
        }
        user_class_ptr->b = test_longs[i];
        user_class_ptr->c = test_strings[i];

        temp.d = user_class_ptr;

        testbed->vec_user_defined.emplace_back(temp);
    }

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->vec_user_defined, "my_alloc", vec_attr->name) ;

    // ASSERT
    ASSERT_TRUE(memmgr->var_exists("my_alloc_vec_user_defined") == 1);

    REF2 * data_ref = memmgr->ref_attributes("my_alloc_vec_user_defined");
    UserClass * data = (UserClass *) data_ref->address;

    ASSERT_TRUE(data != NULL);
    EXPECT_EQ(data_ref->attr->num_index, 1);
    ASSERT_EQ(data_ref->attr->index[0].size, numObj);

    for (int i = 0; i < numObj; i++) {
        std::string var_name = "ptr_" + std::to_string(i);
        ASSERT_TRUE(memmgr->var_exists(var_name));
        REF2 * ptr_ref = memmgr->ref_attributes(var_name.c_str());
        UserClass * ptr_data = (UserClass *) ptr_ref->address;

        for (int j = 0; j < 5; j++) {
            EXPECT_EQ(data[i].a[j], test_ints[i*5+j]);
            EXPECT_EQ(ptr_data->a[j], test_ints[i*5+j]);
        }

        EXPECT_EQ(data[i].b, test_longs[i]);
        EXPECT_EQ(ptr_data->b, test_longs[i]);

        EXPECT_EQ(data[i].c, test_strings[i]);
        EXPECT_EQ(ptr_data->c, test_strings[i]);   
    }
}

TEST_F(MM_stl_checkpoint, DISABLED_vec_user_defined_stl ) {
    // Make a testbed
    // ARRANGE
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<std::vector<int>> test_data;
    for (int i = 0; i < 10; i++) {
        std::vector<int> test_data_inner = get_test_data<int>(10);
        UserClassStl temp_wrapper;
        temp_wrapper.vec = std::vector<int>(test_data_inner);
        testbed->vec_user_defined_stl.emplace_back(temp_wrapper);
        test_data.emplace_back(test_data_inner);
    }

    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.vec_user_defined_stl")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->vec_user_defined_stl, "my_alloc", vec_attr->name) ;

    // ASSERT
    
}


// If this ridiculous data structure can be checkpointed, then I will
// have faith that any arbitrary nesting of STLs can be checkpointed
TEST_F(MM_stl_checkpoint, recursive_nightmare ) {
    // ARRANGE
    // Make a testbed
    STLTestbed * testbed = (STLTestbed *) memmgr->declare_var("STLTestbed my_alloc");

    std::vector<int> test_data = get_test_data<int>(50);
    std::vector<std::string> test_strings = get_test_data<std::string>(100);

    // Just build another one alongside and check the data inside?
    // TODO: still need to figure out how to check the links between the datastructures in checkpointed form
    std::array<std::map<std::pair<int, int>,std::vector<std::stack<std::string>>>, 5> recursive_nightmare_copy;

    // Prepare the STL to be tested
    for (int i = 0; i < 5; i++) {
        // iterate through array

        std::vector<std::pair<int,int>> keys;
        // Make pairs as keys
        for (int j = i*10; j < (i+1)*10; j+=2) {
            keys.emplace_back(test_data[i], test_data[i+1]);
        }

        std::vector<std::vector<std::stack<std::string>>> vals;

        // Make vectors of stacks of strings as values
        for (int j = i * 20; j < (i+1) * 20; j+=4) {
            std::vector<std::stack<std::string>> val;
            std::stack<std::string> inner1;
            std::stack<std::string> inner2;

            inner1.push(test_strings[j]);
            inner1.push(test_strings[j+1]);
            inner2.push(test_strings[j+2]);
            inner2.push(test_strings[j+3]);

            val.emplace_back(inner1);
            val.emplace_back(inner2);

            vals.emplace_back(val);
        }

        // Make the map
        for (int m = 0; m < keys.size(); m++) {
            testbed->recursive_nightmare[i].insert(std::pair<std::pair<int, int>,std::vector<std::stack<std::string>>>(keys[m], vals[m]));
            recursive_nightmare_copy[i].insert(std::pair<std::pair<int, int>,std::vector<std::stack<std::string>>>(keys[m], vals[m]));
        }

    }


    ATTRIBUTES* vec_attr = memmgr->ref_attributes("my_alloc.recursive_nightmare")->attr;

    // ACT
    // Call the checkpoint function that is being tested
    (vec_attr->checkpoint_stl)((void *) &testbed->recursive_nightmare, "my_alloc", vec_attr->name) ;

    // ASSERT
    for (int i = 0; i < 5; i++) {

        int j = 0;
        for (auto it = recursive_nightmare_copy[i].begin(); it != recursive_nightmare_copy[i].end(); it++, j++) {
            std::pair<int,int> key = it->first;
            std::vector<std::stack<std::string>> val = it->second;

            // keys first
            std::string key_var_name = "recursive_nightmare_" + std::to_string(i) + "_keys_" + std::to_string(j);
            validate_temp_pair(memmgr, std::string("my_alloc"), key_var_name, key);

            for (int k = 0; k < val.size(); k++) {
                std::string val_var_name = "recursive_nightmare_" + std::to_string(i) + "_data_" + std::to_string(j) + "_" + std::to_string(k);
                std::vector<std::string> val_expected;
                while (!val[k].empty()) {
                    val_expected.push_back(val[k].top());
                    val[k].pop();
                }
                validate_temp_sequence(memmgr, std::string("my_alloc"), val_var_name, val_expected);
            }
        }
    }
}
