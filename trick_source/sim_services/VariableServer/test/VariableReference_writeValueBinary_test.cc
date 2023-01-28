#include "VariableReference_test.hh"


TEST_F(VariableReference_test, writeValueBinary_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 4095;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    
    unsigned char expected_bytes[4] = {0xFF, 0x0F, 0x00, 0x00};
    char * actual_bytes = (char *) malloc (sizeof(int));

    ss.read(actual_bytes, 4);

    // ASSERT
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}

TEST_F(VariableReference_test, writeValueBinary_float) {
    // ARRANGE
    // Create a variable to make a reference for
    float test_a = 40.95;
    (void) memmgr->declare_extern_var(&test_a, "float test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);
    
    unsigned char expected_bytes[4] = {0xcd, 0xcc, 0x23, 0x42};

    char * actual_bytes = (char *) malloc (sizeof(float));

    ss.read(actual_bytes, 4);

    // ASSERT
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}

TEST_F(VariableReference_test, writeValueBinary_int_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a[3] = {1, 2, 3};
    (void) memmgr->declare_extern_var(&test_a, "int test_a[3]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (sizeof(int )* 3);
    ss.read(actual_bytes, sizeof(int) * 3);
    unsigned char expected_bytes[12] = {0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};


    // ASSERT
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}


TEST_F(VariableReference_test, writeValueBinary_double_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    double test_a[3] = {1.0, 10.0, 100.0};
    (void) memmgr->declare_extern_var(&test_a, "double test_a[3]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (sizeof(double )* 3);
    ss.read(actual_bytes, sizeof(double) * 3);
    unsigned char expected_bytes[24] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x40};

    // ASSERT
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}


TEST_F(VariableReference_test, writeValueBinary_string) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "abcdef";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (test_a.length());
    ss.read(actual_bytes, test_a.length());
    unsigned char expected_bytes[6] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66};

    // ASSERT
    for (int i = 0; i < test_a.length(); i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}

TEST_F(VariableReference_test, writeNameBinary) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "abcdef";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeNameBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (sizeof(int) + strlen(ref.getName()));
    ss.read(actual_bytes, sizeof(int) +  6);
    unsigned char expected_bytes[sizeof(int) +  6] = {0x06, 0x00, 0x00, 0x00, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x61};

    // ASSERT
    for (int i = 0; i < sizeof(int) +  strlen(ref.getName()); i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}


TEST_F(VariableReference_test, writeValueBinaryByteswap_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 4095;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss, true);

    
    unsigned char expected_bytes[4] = {0x00, 0x00, 0x0F, 0xFF};
    char * actual_bytes = (char *) malloc (sizeof(int));

    ss.read(actual_bytes, 4);

    // ASSERT
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}