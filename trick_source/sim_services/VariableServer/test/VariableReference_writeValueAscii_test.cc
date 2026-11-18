#include "VariableReference_test.hh"


TEST_F(VariableReference_test, writeValueAscii_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "5");
}

TEST_F(VariableReference_test, writeValueAscii_int_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a[5] = {1, 2, 3, 4, 5};
    (void) memmgr->declare_extern_var(&test_a, "int test_a[5]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "1,2,3,4,5");
}

TEST_F(VariableReference_test, writeValueAscii_double) {
    // ARRANGE
    // Create a variable to make a reference for
    double test_a = 867.309;
    (void) memmgr->declare_extern_var(&test_a, "double test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ss_a;

    double test_b = std::numeric_limits<double>::max();
    (void) memmgr->declare_extern_var(&test_b, "double test_b");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ss_b;

    double test_c = std::numeric_limits<double>::min();
    (void) memmgr->declare_extern_var(&test_c, "double test_c");
    Trick::VariableReference ref_c("test_c");
    std::stringstream ss_c;

    // ACT
    ref_a.stageValue();
    ref_a.prepareForWrite();
    ref_a.writeValueAscii(ss_a);

    ref_b.stageValue();
    ref_b.prepareForWrite();
    ref_b.writeValueAscii(ss_b);

    ref_c.stageValue();
    ref_c.prepareForWrite();
    ref_c.writeValueAscii(ss_c);

    // ASSERT
    EXPECT_EQ(ss_a.str(), "867.309");
    EXPECT_EQ(ss_b.str(), "1.797693134862316e+308");
    EXPECT_EQ(ss_c.str(), "2.225073858507201e-308");
}

TEST_F(VariableReference_test, writeValueAscii_char) {
    // ARRANGE
    // Create a variable to make a reference for
    char test_a = 'j';
    (void) memmgr->declare_extern_var(&test_a, "char test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    char test_b[7] = "jackie";
    (void) memmgr->declare_extern_var(&test_b, "char test_b[7]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

    // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);


    // ASSERT
    EXPECT_EQ(ssa.str(), "106");
    EXPECT_EQ(ssb.str(), "jackie");
}


TEST_F(VariableReference_test, writeValueAscii_unsigned_char) {
    // ARRANGE
    // Create a variable to make a reference for
    unsigned char test_a = 'j';
    (void) memmgr->declare_extern_var(&test_a, "unsigned char test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    unsigned char test_b[8] = "jackie\n";
    (void) memmgr->declare_extern_var(&test_b, "unsigned char test_b[8]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

    // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);

    // ASSERT
    EXPECT_EQ(ssa.str(), "106");
    EXPECT_EQ(ssb.str(), "jackie\\n");
}


TEST_F(VariableReference_test, writeValueAscii_wide_char) {
    // ARRANGE
    // Create a variable to make a reference for
    wchar_t test_a = L'J';
    (void) memmgr->declare_extern_var(&test_a, "wchar_t test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    wchar_t test_b[15] = L"jackiebutwider";
    (void) memmgr->declare_extern_var(&test_b, "wchar_t test_b[15]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

   // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);

    // ASSERT
    // Original variable server behavior prints individual wchar as its ascii value
    EXPECT_EQ(ssa.str(), "74");
    EXPECT_EQ(ssb.str(), "jackiebutwider");
}

TEST_F(VariableReference_test, DISABLED_writeValueAscii_wide_char_unconstrained) {
    TestObject obj;
    obj.wchar_str = (wchar_t *) malloc (sizeof(wchar_t) * 7);
    for (int i = 0; i < 6; i++) {
        obj.wchar_str[i] = L'j';
    }
    obj.wchar_str[6] = L'\0';
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    // (void) memmgr->declare_extern_var(&obj.wchar_str, "wchar_t * obj.wchar_str");

    Trick::VariableReference ref("obj.wchar_str");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "jjjjjj");

}

TEST_F(VariableReference_test, writeValueAscii_std_string) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "jackiebutstringy";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "jackiebutstringy");
}

TEST_F(VariableReference_test, writeValueAscii_escape_characters) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "\n\t\b\a\"\f\r\v";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "\\n\\t\\b\\a\"\\f\\n\\v");
}

TEST_F(VariableReference_test, writeValueAscii_boolean) {
    // ARRANGE
    // Create a variable to make a reference for
    bool test_a = true;
    (void) memmgr->declare_extern_var(&test_a, "bool test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "1");
}

TEST_F(VariableReference_test, writeValueAscii_short) {
    // ARRANGE
    // Create a variable to make a reference for
    short test_a = 255;
    (void) memmgr->declare_extern_var(&test_a, "short test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "255");
}

// Writes the value of obj.<name> through the variable server and returns it
static std::string writeEnumAscii(const char* name)
{
    Trick::VariableReference ref(name);
    std::stringstream ss;
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    return ss.str();
}

TEST_F(VariableReference_test, writeValueAscii_enum_1byte)
{
    // ARRANGE
    EnumObject obj;
    obj.uc = static_cast<EnumObject::UC>(200);
    obj.sc = static_cast<EnumObject::SC>(-1);
    (void)memmgr->declare_extern_var(&obj, "EnumObject obj");

    // ACT/ASSERT
    EXPECT_EQ(writeEnumAscii("obj.uc"), "200");
    EXPECT_EQ(writeEnumAscii("obj.sc"), "-1");
}

TEST_F(VariableReference_test, writeValueAscii_enum_2byte)
{
    // ARRANGE
    EnumObject obj;
    obj.us = static_cast<EnumObject::US>(40000);
    obj.ss = static_cast<EnumObject::SS>(-30000);
    (void)memmgr->declare_extern_var(&obj, "EnumObject obj");

    // ACT/ASSERT
    EXPECT_EQ(writeEnumAscii("obj.us"), "40000");
    EXPECT_EQ(writeEnumAscii("obj.ss"), "-30000");
}

TEST_F(VariableReference_test, writeValueAscii_enum_4byte)
{
    // ARRANGE
    EnumObject obj;
    obj.ui = static_cast<EnumObject::UI>(0xFFFFFFFFu);
    obj.si = static_cast<EnumObject::SI>(-2000000000);
    (void)memmgr->declare_extern_var(&obj, "EnumObject obj");

    // ACT/ASSERT
    // 4-byte enums are written as int regardless of signedness, matching the enum values in ENUM_ATTR
    EXPECT_EQ(writeEnumAscii("obj.ui"), "-1");
    EXPECT_EQ(writeEnumAscii("obj.si"), "-2000000000");
}

TEST_F(VariableReference_test, writeValueAscii_enum_8byte)
{
    // ARRANGE
    EnumObject obj;
    obj.ul = static_cast<EnumObject::UL>(5000000000ull);
    obj.sl = static_cast<EnumObject::SL>(-5000000000ll);
    (void)memmgr->declare_extern_var(&obj, "EnumObject obj");

    // ACT/ASSERT
    EXPECT_EQ(writeEnumAscii("obj.ul"), "5000000000");
    EXPECT_EQ(writeEnumAscii("obj.sl"), "-5000000000");
}

TEST_F(VariableReference_test, writeValueAscii_enum_arr)
{
    // ARRANGE
    EnumObject obj;
    obj.sc_arr[0] = static_cast<EnumObject::SC>(-1);
    obj.sc_arr[1] = static_cast<EnumObject::SC>(-2);
    obj.sc_arr[2] = static_cast<EnumObject::SC>(-3);
    (void)memmgr->declare_extern_var(&obj, "EnumObject obj");

    // ACT/ASSERT
    EXPECT_EQ(writeEnumAscii("obj.sc_arr"), "-1,-2,-3");
}
