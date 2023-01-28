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
