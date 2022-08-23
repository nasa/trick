#include "VariableReference_test.hh"

TEST_F(VariableReference_test, getName) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_int = 5;
    (void) memmgr->declare_extern_var(&test_int, "int test_int");
    Trick::VariableReference ref("test_int");

    // ACT
    // ASSERT
    EXPECT_EQ(strcmp(ref.getName(), "test_int"), 0);
}

TEST_F(VariableReference_test, validateAddress) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    
    float test_b = 5.0;
    (void) memmgr->declare_extern_var(&test_b, "float test_b");
    Trick::VariableReference ref_broken("test_b");
    memmgr->delete_var("test_b");

    float test_c = 5.0;
    (void) memmgr->declare_extern_var(&test_c, "float test_c");
    Trick::VariableReference ref_tagged("test_c");
    ref_tagged.tagAsInvalid();

    // ACT
    // ASSERT
    EXPECT_EQ(ref.validate(), true);
    EXPECT_EQ(ref_broken.validate(), false);
    // A reference already tagged as invalid is considered ok by this check
    EXPECT_EQ(ref_tagged.validate(), true);
}


TEST_F(VariableReference_test, stageValue_set) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.stageValue();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), true);
    EXPECT_EQ(ref.isWriteReady(), false);
}

TEST_F(VariableReference_test, prepareForWrite_set) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.stageValue();
    ref.prepareForWrite();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), false);
    EXPECT_EQ(ref.isWriteReady(), true);
}

TEST_F(VariableReference_test, prepareForWrite_fails_if_not_staged) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.prepareForWrite();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), false);
    EXPECT_EQ(ref.isWriteReady(), false);
}

TEST_F(VariableReference_test, writeValueAscii_fails_if_not_write_ready) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    // ASSERT
    EXPECT_EQ(ref.writeValueAscii(ss), -1);
}

TEST_F(VariableReference_test, var_does_not_exist) {
    // ARRANGE
    Trick::VariableReference ref("no_such_var");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ref.getType(), TRICK_NUMBER_OF_TYPES);
    EXPECT_EQ(ss.str(), "BAD_REF");
}

TEST_F(VariableReference_test, stl_var) {
    // ARRANGE
    TestObject my_test;
    my_test.vec.push_back(5);
    (void) memmgr->declare_extern_var(&my_test, "TestObject my_test");

    Trick::VariableReference ref("my_test.vec");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ref.isWriteReady(), true);
    EXPECT_EQ(ref.getType(), TRICK_NUMBER_OF_TYPES);
    EXPECT_EQ(ss.str(), "BAD_REF");
}

TEST_F(VariableReference_test, structured_var) {
    // ARRANGE
    TestObject my_test;
    my_test.obj.a = 5;
    (void) memmgr->declare_extern_var(&my_test, "TestObject my_test");

    Trick::VariableReference ref("my_test.obj");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ref.isWriteReady(), true);
    EXPECT_EQ(ref.getType(), TRICK_NUMBER_OF_TYPES);
    EXPECT_EQ(ss.str(), "BAD_REF");
}

TEST_F(VariableReference_test, printWithoutUnits) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.length = 5000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.length");
    std::stringstream ss;

    // ACT
    EXPECT_STREQ(ref.getBaseUnits(), "m");

    // ASSERT
    // Doesn't actually print with units unless set
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5000");
}

TEST_F(VariableReference_test, setUnits) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.length = 5000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.length");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("km");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5 {km}");
}

TEST_F(VariableReference_test, setUnitsTwice) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.length = 5000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.length");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("km");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5 {km}");
    ss.str("");

    // ACT
    ref.setRequestedUnits("mm");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5000000 {mm}");
}


TEST_F(VariableReference_test, setUnitsBadFromUnits) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.error_units = 50;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.error_units");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("mm");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "50");
}

TEST_F(VariableReference_test, setUnitsBadToUnits) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.a = 0.5;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.a");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("asldfjks");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "0.5");
}

TEST_F(VariableReference_test, setUnitsDouble) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.a = 0.5;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.a");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("ms");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "500 {ms}");
}

TEST_F(VariableReference_test, setUnitsLong) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.b = 1;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.b");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("m");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "1852 {m}");
}

TEST_F(VariableReference_test, setUnitsLongLong) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.c = 10000000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.c");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("m");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "254000 {m}");
}

TEST_F(VariableReference_test, setUnitsUnsignedLong) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.d = 1;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.d");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("mi");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5878625079535 {mi}");
}

TEST_F(VariableReference_test, setUnitsUnsignedLongLong) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.e = 1;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.e");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("mm");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "1609344 {mm}");
}

// Byteswap tests

TEST_F(VariableReference_test, byteswap_chars) {
    // ARRANGE
    char in = 'a';
    char out;

    memmgr->declare_extern_var(&in, "char a");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var(&out, &in, ref);

    // ASSERT
    EXPECT_EQ(in, 'a');
    EXPECT_EQ(out, 'a');
}

TEST_F(VariableReference_test, byteswap_char_array) {
    // ARRANGE
    char in[7] = "Jackie";
    char out[7];

    memmgr->declare_extern_var(&in, "char a[7]");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var(out, in, ref);

    // ASSERT
    for (int i = 0; i < 7; i++) {
        EXPECT_EQ(out[i], in[i]);
    }
}

// Pointers to the beginning of the variables
bool check_that_val_is_byteswapped (char * expected, char * byteswap, int val_size) {
    for (int i = 0; i < val_size; i++) {
        if (expected[i] != byteswap[val_size-i-1]) return false;
    }
    return true;
}

TEST_F(VariableReference_test, byteswap_short) {
    // ARRANGE
    short in = 100; 
    short out;      

    memmgr->declare_extern_var(&in, "short a");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var((char *)(&out),(char *)(&in), ref);

    // ASSERT
    EXPECT_TRUE(check_that_val_is_byteswapped((char *)(&out),(char *)(&in), sizeof(short)));
}

TEST_F(VariableReference_test, byteswap_int) {
    // ARRANGE
    int in = 123456;
    int out;      

    memmgr->declare_extern_var(&in, "int a");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var((char *)(&out),(char *)(&in), ref);

    // ASSERT
    EXPECT_TRUE(check_that_val_is_byteswapped((char *)(&out),(char *)(&in), sizeof(int)));
}

TEST_F(VariableReference_test, byteswap_long) {
    // ARRANGE
    long in = 123456789; 
    long out;

    memmgr->declare_extern_var(&in, "long a");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var((char *)(&out),(char *)(&in), ref);

    // ASSERT
    EXPECT_TRUE(check_that_val_is_byteswapped((char *)(&out),(char *)(&in), sizeof(long)));
}

TEST_F(VariableReference_test, byteswap_long_arr) {
    // ARRANGE
    long in[5] = {123456789, 123456780, __LONG_MAX__, -100000000, 0}; 
    long out[5];

    memmgr->declare_extern_var(&in, "long a[5]");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var((char *)(out),(char *)(in), ref);

    // ASSERT
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(check_that_val_is_byteswapped((char *)(&out[i]),(char *)(&in[i]), sizeof(long)));
    }
}

TEST_F(VariableReference_test, byteswap_int_arr) {
    // ARRANGE
    int in[5] = {20945, -29384293, INT32_MAX, INT32_MIN, 0}; 
    int out[5];

    memmgr->declare_extern_var(&in, "int a[5]");
    Trick::VariableReference ref("a");

    // ACT
    Trick::VariableReference::byteswap_var((char *)(out),(char *)(in), ref);

    // ASSERT
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(check_that_val_is_byteswapped((char *)(&out[i]),(char *)(&in[i]), sizeof(int)));
    }
}

TEST_F(VariableReference_test, byteswap_int_multidimensional_arr) {
    // ARRANGE
    int multidim_arr[5][4][3][2];
    int out[5][4][3][2];


    int counter = 500;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 2; l++) {
                    multidim_arr[i][j][k][l] = counter++;
                }
            }
        }
    }

    memmgr->declare_extern_var(&multidim_arr, "int multidim_arr[5][4][3][2]");
    Trick::VariableReference ref("multidim_arr");

    // ACT
    Trick::VariableReference::byteswap_var((char *)(out),(char *)(multidim_arr), ref);

    // ASSERT
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < 2; l++) {
                    EXPECT_TRUE(check_that_val_is_byteswapped((char *)(&(out[i][j][k][l])),(char *)(&(multidim_arr[i][j][k][l])), sizeof(int)));
                }
            }
        }
    }
}