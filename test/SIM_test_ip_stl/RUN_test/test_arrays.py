from trick.unit_test import *

def array_tests(stl_so, test_suite):
    test_name  = "Char Arrays"
    stl_so.stl_ip_test.char_types.array[0] = 'a'
    stl_so.stl_ip_test.char_types.array[1] = 'b'
    stl_so.stl_ip_test.char_types.array[2] = 'c'
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.char_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.array[0], 'a', test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.array[1], 'b', test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.array[2], 'c', test_suite, test_name)
    
    try:
        test_name = "Char Arrays"
        stl_so.stl_ip_test.char_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<char, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)


    test_name = "Signed Char Arrays"
    stl_so.stl_ip_test.signed_char_types.array[0] = -1
    stl_so.stl_ip_test.signed_char_types.array[1] = -2
    stl_so.stl_ip_test.signed_char_types.array[2] = -3
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.signed_char_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.array[0], -1, test_suite, test_name)    
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.array[1], -2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.array[2], -3, test_suite, test_name)
    try:
        test_name = "Signed Char Arrays"
        stl_so.stl_ip_test.signed_char_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<signed char, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    

    test_name = "Unsigned Char Arrays"
    stl_so.stl_ip_test.unsigned_char_types.array[0] = 1
    stl_so.stl_ip_test.unsigned_char_types.array[1] = 2
    stl_so.stl_ip_test.unsigned_char_types.array[2] = 3
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.unsigned_char_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.array[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.array[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.array[2], 3, test_suite, test_name)
    try:
        test_name = "Unsigned Char Arrays"
        stl_so.stl_ip_test.unsigned_char_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<unsigned char, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    
    #TODO: Fix test for char16_t
    '''
    test_name = "char16_t Arrays"
    stl_so.stl_ip_test.char16_t_types.array[0] = stl_so.stl_ip_test.to_char16_t('a')
    stl_so.stl_ip_test.char16_t_types.array[1] = stl_so.stl_ip_test.to_char16_t('b')
    stl_so.stl_ip_test.char16_t_types.array[2] = stl_so.stl_ip_test.to_char16_t('c')
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.char16_t_types.array), 3, test_suite, test_name)

    try:
        TRICK_EXPECT_EQ(stl_so.stl_ip_test.char16_t_types.array[0], stl_so.stl_ip_test.to_char16_t('a'), test_suite, test_name)
        TRICK_EXPECT_EQ(stl_so.stl_ip_test.char16_t_types.array[1], stl_so.stl_ip_test.to_char16_t('b'), test_suite, test_name)
        TRICK_EXPECT_EQ(stl_so.stl_ip_test.char16_t_types.array[2], stl_so.stl_ip_test.to_char16_t('c'), test_suite, test_name)
    except Exception as e:
        print(f"Exception {e}")

    try:
        test_name = "char16_t Arrays"
        stl_so.stl_ip_test.char16_t_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<char16_t, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    


    test_name = "char32_t Arrays"
    stl_so.stl_ip_test.char32_t_types.array[0] = 'a'
    stl_so.stl_ip_test.char32_t_types.array[1] = 'b'
    stl_so.stl_ip_test.char32_t_types.array[2] = 'c'
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.char32_t_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char32_t_types.array[0], 'a', test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char32_t_types.array[1], 'b', test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char32_t_types.array[2], 'c', test_suite, test_name)
    try:
        test_name = "char32_t Arrays"
        stl_so.stl_ip_test.char32_t_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<char32_t, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    '''

    test_name = "short int Arrays"
    stl_so.stl_ip_test.short_int_types.array[0] = 1
    stl_so.stl_ip_test.short_int_types.array[1] = 2
    stl_so.stl_ip_test.short_int_types.array[2] = 3
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.short_int_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.array[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.array[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.array[2], 3, test_suite, test_name)
    try:
        test_name = "short int Arrays"
        stl_so.stl_ip_test.short_int_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<short int, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    

    test_name = "unsigned short int Arrays"
    stl_so.stl_ip_test.unsigned_short_int_types.array[0] = 1
    stl_so.stl_ip_test.unsigned_short_int_types.array[1] = 2
    stl_so.stl_ip_test.unsigned_short_int_types.array[2] = 3
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.unsigned_short_int_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.array[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.array[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.array[2], 3, test_suite, test_name)
    #Pass invalid type(struct)
    try:
        test_name = "unsigned short int Arrays"
        stl_so.stl_ip_test.unsigned_short_int_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<unsigned short int, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    #Pass invalid type(signed value)
    try:
        test_name = "unsigned short int Arrays"
        stl_so.stl_ip_test.unsigned_short_int_types.array[0] = -1

    except Exception as e:
        print(f"Expected exception due to passing signed value into std::array<unsigned short int, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)



    test_name = "int Arrays"
    stl_so.stl_ip_test.int_types.array[0] = 1
    stl_so.stl_ip_test.int_types.array[1] = 2
    stl_so.stl_ip_test.int_types.array[2] = 3
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.int_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.array[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.array[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.array[2], 3, test_suite, test_name)

    try:
        test_name = "int Arrays"
        stl_so.stl_ip_test.int_types.array[0] = trick.TestStructure()
    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<int, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)

    test_name = "unsigned int Arrays"
    stl_so.stl_ip_test.unsigned_int_types.array[0] = 1
    stl_so.stl_ip_test.unsigned_int_types.array[1] = 2
    stl_so.stl_ip_test.unsigned_int_types.array[2] = 3
    TRICK_EXPECT_EQ(len(stl_so.stl_ip_test.unsigned_int_types.array), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.array[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.array[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.array[2], 3, test_suite, test_name)
    #Pass invalid type(struct)
    try:
        test_name = "unsigned int Arrays"
        stl_so.stl_ip_test.unsigned_int_types.array[0] = trick.TestStructure()

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into std::array<unsigned int, 3> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)