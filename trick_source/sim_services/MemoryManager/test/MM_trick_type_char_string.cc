
#include "trick/parameter_types.h"
#include <gtest/gtest.h>
#include <iostream>

/* ================================================================================
 *                                    Test Cases
 */

TEST(MM_tricktypecharstring, TRICK_VOID_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_VOID, name);
    EXPECT_STREQ(result, "void");
}

TEST(MM_tricktypecharstring, TRICK_CHARACTER_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_CHARACTER, name);
    EXPECT_STREQ(result, "char");
}

TEST(MM_tricktypecharstring, TRICK_UNSIGNED_CHARACTER_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_UNSIGNED_CHARACTER, name);
    EXPECT_STREQ(result, "unsigned char");
}

TEST(MM_tricktypecharstring, TRICK_STRING_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_STRING, name);
    EXPECT_STREQ(result, "std::string");
}

TEST(MM_tricktypecharstring, TRICK_SHORT_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_SHORT, name);
    EXPECT_STREQ(result, "short");
}

TEST(MM_tricktypecharstring, TRICK_UNSIGNED_SHORT_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_UNSIGNED_SHORT, name);
    EXPECT_STREQ(result, "unsigned short");
}

TEST(MM_tricktypecharstring, TRICK_INTEGER_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_INTEGER, name);
    EXPECT_STREQ(result, "int");
}

TEST(MM_tricktypecharstring, TRICK_UNSIGNED_INTEGER_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_UNSIGNED_INTEGER, name);
    EXPECT_STREQ(result, "unsigned int");
}

TEST(MM_tricktypecharstring, TRICK_LONG_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_LONG, name);
    EXPECT_STREQ(result, "long");
}

TEST(MM_tricktypecharstring, TRICK_UNSIGNED_LONG_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_UNSIGNED_LONG, name);
    EXPECT_STREQ(result, "unsigned long");
}

TEST(MM_tricktypecharstring, TRICK_FLOAT_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_FLOAT, name);
    EXPECT_STREQ(result, "float");
}

TEST(MM_tricktypecharstring, TRICK_DOUBLE_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_DOUBLE, name);
    EXPECT_STREQ(result, "double");
}

TEST(MM_tricktypecharstring, TRICK_BITFIELD_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_BITFIELD, name);
    EXPECT_STREQ(result, "TRICK_BITFIELD");
}

TEST(MM_tricktypecharstring, TRICK_UNSIGNED_BITFIELD_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_UNSIGNED_BITFIELD, name);
    EXPECT_STREQ(result, "TRICK_UNSIGNED_BITFIELD");
}

TEST(MM_tricktypecharstring, TRICK_LONG_LONG_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_LONG_LONG, name);
    EXPECT_STREQ(result, "long long");
}

TEST(MM_tricktypecharstring, TRICK_UNSIGNED_LONG_LONG_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_UNSIGNED_LONG_LONG, name);
    EXPECT_STREQ(result, "unsigned long long");
}

TEST(MM_tricktypecharstring, TRICK_FILE_PTR_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_FILE_PTR, name);
    EXPECT_STREQ(result, "FILE*");
}

TEST(MM_tricktypecharstring, TRICK_BOOLEAN_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_BOOLEAN, name);
    EXPECT_STREQ(result, "bool");
}

TEST(MM_tricktypecharstring, TRICK_WCHAR_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_WCHAR, name);
    EXPECT_STREQ(result, "wchar_t");
}

TEST(MM_tricktypecharstring, TRICK_WSTRING_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_WSTRING, name);
    EXPECT_STREQ(result, "wchar_t*");
}

TEST(MM_tricktypecharstring, TRICK_VOID_PTR_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_VOID_PTR, name);
    EXPECT_STREQ(result, "void*");
}

TEST(MM_tricktypecharstring, TRICK_ENUMERATED_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_ENUMERATED, name);
    EXPECT_STREQ(result, name);
}

TEST(MM_tricktypecharstring, TRICK_STRUCTURED_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_STRUCTURED, name);
    EXPECT_STREQ(result, name);
}

TEST(MM_tricktypecharstring, TRICK_OPAQUE_TYPE_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_OPAQUE_TYPE, name);
    EXPECT_STREQ(result, name);
}

TEST(MM_tricktypecharstring, TRICK_STL_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_STL, name);
    EXPECT_STREQ(result, name);
}

TEST(MM_tricktypecharstring, DEFAULT_test)
{
    const char * name = "some_name";
    const char * result = trickTypeCharString(TRICK_NUMBER_OF_TYPES, name);
    EXPECT_STREQ(result, "UNKNOWN_TYPE");
}
