#include "VariableReference_test.hh"

TEST_F(VariableReference_test, getName) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_int = 5;
    (void) memmgr->declare_extern_var(&test_int, "int test_int");
    Trick::VariableReference ref("test_int");

    // ACT
    // ASSERT
    EXPECT_EQ(ref.getName(), std::string("test_int"));
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
    Trick::VariableReference ref_elem("my_test.vec[0]");
    std::stringstream ss;

    // ACT
    ref_elem.stageValue();
    ref_elem.prepareForWrite();
    ref_elem.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ref.getType(), TRICK_STL);
    EXPECT_EQ(ref_elem.isWriteReady(), true);
    EXPECT_EQ(ref_elem.getType(), TRICK_INTEGER);
    EXPECT_EQ(ss.str(), "5");
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
    EXPECT_EQ(ref.getBaseUnits(), std::string("m"));

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

// ============================================================================
// Tests for Issue #1849: Address & Value Access APIs in VariableReference
// ============================================================================

TEST_F(VariableReference_test, resolveAddress_and_getAddress) {
    // ARRANGE
    int target_var = 12345;
    memmgr->declare_extern_var(&target_var, "int target_var");
    Trick::VariableReference ref("target_var");

    // ACT: resolveAddress without calling stageValue()
    void* resolved_addr = ref.resolveAddress();

    // ASSERT: address matches target_var address
    EXPECT_EQ(resolved_addr, (void*)&target_var);
    EXPECT_EQ(ref.getAddress(), (void*)&target_var);
    EXPECT_EQ(ref.getAddress<int>(), &target_var);
}

TEST_F(VariableReference_test, getValue_direct_memory_peek) {
    // ARRANGE
    double target_double = 98.765;
    memmgr->declare_extern_var(&target_double, "double target_double");
    Trick::VariableReference ref("target_double");

    // ACT & ASSERT: reading directly before staging
    ref.resolveAddress();
    EXPECT_DOUBLE_EQ(ref.getValue<double>(), 98.765);

    // Mutate underlying variable directly in memory
    target_double = 123.456;
    EXPECT_DOUBLE_EQ(ref.getValue<double>(), 123.456);
}

TEST_F(VariableReference_test, getStagedValue_and_getStageBuffer) {
    // ARRANGE
    int staged_int = 42;
    memmgr->declare_extern_var(&staged_int, "int staged_int");
    Trick::VariableReference ref("staged_int");

    // ACT: stage the value
    ref.stageValue();

    // ASSERT: stage buffer has the value
    EXPECT_NE(ref.getStageBuffer(), nullptr);
    EXPECT_EQ(ref.getStagedValue<int>(), 42);

    // Mutating memory directly should not affect the already staged snapshot
    staged_int = 999;
    EXPECT_EQ(ref.getStagedValue<int>(), 42);
    EXPECT_EQ(ref.getValue<int>(), 999);
}

TEST_F(VariableReference_test, getRefInfo_and_getAttributes) {
    // ARRANGE
    int test_attr_var = 10;
    memmgr->declare_extern_var(&test_attr_var, "int test_attr_var");
    Trick::VariableReference ref("test_attr_var");

    // ACT
    const REF2* ref_info = ref.getRefInfo();
    const ATTRIBUTES* attr = ref.getAttributes();

    // ASSERT
    ASSERT_NE(ref_info, nullptr);
    ASSERT_NE(attr, nullptr);
    EXPECT_EQ(attr->type, TRICK_INTEGER);
    EXPECT_EQ(attr->size, sizeof(int));
    EXPECT_STREQ(attr->units, "--");
}

TEST_F(VariableReference_test, writeDirect_fast_poke) {
    // ARRANGE
    int direct_var = 100;
    memmgr->declare_extern_var(&direct_var, "int direct_var");
    Trick::VariableReference ref("direct_var");

    // ACT: direct poke into memory
    int status = ref.writeDirect<int>(200);

    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_EQ(direct_var, 200);
    EXPECT_EQ(ref.getValue<int>(), 200);
}

TEST_F(VariableReference_test, setVariable_int) {
    // ARRANGE
    int test_val = 10;
    memmgr->declare_extern_var(&test_val, "int test_val");
    Trick::VariableReference ref("test_val");

    // ACT: poke via setVariable
    int status = ref.setVariable(55);

    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_EQ(test_val, 55);
    EXPECT_EQ(ref.getValue<int>(), 55);
}

TEST_F(VariableReference_test, setVariable_double) {
    // ARRANGE
    double test_val = 3.14159;
    memmgr->declare_extern_var(&test_val, "double test_val");
    Trick::VariableReference ref("test_val");

    // ACT
    int status = ref.setVariable(2.71828);

    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_DOUBLE_EQ(test_val, 2.71828);
    EXPECT_DOUBLE_EQ(ref.getValue<double>(), 2.71828);
}

TEST_F(VariableReference_test, setVariable_bool) {
    // ARRANGE
    bool test_val = false;
    memmgr->declare_extern_var(&test_val, "bool test_val");
    Trick::VariableReference ref("test_val");

    // ACT
    int status = ref.setVariable(true);

    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_EQ(test_val, true);
    EXPECT_EQ(ref.getValue<bool>(), true);
}

TEST_F(VariableReference_test, setVariable_string) {
    // ARRANGE
    char test_str[32] = "Initial";
    memmgr->declare_extern_var(test_str, "char test_str[32]");
    Trick::VariableReference ref("test_str");

    // ACT
    int status = ref.setVariable("UpdatedString");

    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_STREQ(test_str, "UpdatedString");
}

TEST_F(VariableReference_test, setVariable_with_unit_conversion) {
    // ARRANGE: TestObject has "double a; /* s time */" in seconds
    TestObject obj;
    obj.a = 60.0; // 60 seconds
    memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.a");

    // ACT: Poke 2 minutes ("min") into seconds
    int status = ref.setVariable(2.0, "min");

    // ASSERT: 2 minutes = 120 seconds
    EXPECT_EQ(status, 0);
    EXPECT_DOUBLE_EQ(obj.a, 120.0);
    EXPECT_DOUBLE_EQ(ref.getValue<double>(), 120.0);
}

TEST_F(VariableReference_test, setVariable_unsigned) {
    // ARRANGE
    unsigned int test_val = 100u;
    memmgr->declare_extern_var(&test_val, "unsigned int test_val");
    Trick::VariableReference ref("test_val");

    // ACT: Poke unsigned integer
    int status = ref.setVariable(500u);

    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_EQ(test_val, 500u);
    EXPECT_EQ(ref.getValue<unsigned int>(), 500u);
}

TEST_F(VariableReference_test, setVariable_invalid_ref) {
    // ARRANGE: Reference to non-existent variable
    Trick::VariableReference ref("non_existent_var");

    // ACT
    int status = ref.setVariable(42);

    // ASSERT: Returns error status
    EXPECT_EQ(status, -1);
}