from trick.unit_test import *

def vector_tests(stl_so, test_suite):
    test_name  = "Char Vectors"
    stl_so.stl_ip_test.char_types.vector.push_back('a')
    stl_so.stl_ip_test.char_types.vector.push_back('b')
    stl_so.stl_ip_test.char_types.vector.push_back('c')
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.vector[0], 'a', test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.vector[1], 'b', test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char_types.vector[2], 'c', test_suite, test_name)
    
    try:
        test_name = "Char Vectors"
        stl_so.stl_ip_test.char_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<char> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)


    test_name = "Signed Char Vectors"
    stl_so.stl_ip_test.signed_char_types.vector.push_back(-1)
    stl_so.stl_ip_test.signed_char_types.vector.push_back(-2)
    stl_so.stl_ip_test.signed_char_types.vector.push_back(-3)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.vector[0], -1, test_suite, test_name)    
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.vector[1], -2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.signed_char_types.vector[2], -3, test_suite, test_name)
    try:
        test_name = "Signed Char Vectors"
        stl_so.stl_ip_test.char_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<signed char> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    

    test_name = "Unsigned Char Vectors"
    stl_so.stl_ip_test.unsigned_char_types.vector.push_back(1)
    stl_so.stl_ip_test.unsigned_char_types.vector.push_back(2)
    stl_so.stl_ip_test.unsigned_char_types.vector.push_back(3)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.vector[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.vector[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_char_types.vector[2], 3, test_suite, test_name)
    try:
        test_name = "Unsigned Char Vectors"
        stl_so.stl_ip_test.char_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<unsigned char> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    

    test_name = "char16_t Vectors"
    '''
    # Calling push_back on char16_t vector directly here causes issues due to char16_t not supported in python
    stl_so.stl_ip_test.char16_t_types.vector.push_back(stl_so.stl_ip_test.to_char16_t('a'))
    stl_so.stl_ip_test.char16_t_types.vector.push_back(stl_so.stl_ip_test.to_char16_t('b'))
    stl_so.stl_ip_test.char16_t_types.vector.push_back(stl_so.stl_ip_test.to_char16_t('c'))
    '''
    stl_so.stl_ip_test.addCharToVector(stl_so.stl_ip_test.char16_t_types.vector, ord('a'))
    stl_so.stl_ip_test.addCharToVector(stl_so.stl_ip_test.char16_t_types.vector, ord('b'))
    stl_so.stl_ip_test.addCharToVector(stl_so.stl_ip_test.char16_t_types.vector, ord('c'))

    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char16_t_types.vector.size(), 3, test_suite, test_name)

    TRICK_EXPECT_EQ(stl_so.stl_ip_test.getCharFromVector(stl_so.stl_ip_test.char16_t_types.vector, 0), ord('a'), test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.getCharFromVector(stl_so.stl_ip_test.char16_t_types.vector, 1), ord('b'), test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.getCharFromVector(stl_so.stl_ip_test.char16_t_types.vector, 2), ord('c'), test_suite, test_name)

    try:
        test_name = "char16_t Vectors"
        stl_so.stl_ip_test.char16_t_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<char16_t> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    

    test_name = "char32_t Vectors"
    '''
    # Calling push_back on char32_t vector directly here causes issues due to char32_t not supported in python
    stl_so.stl_ip_test.char32_t_types.vector.push_back(stl_so.stl_ip_test.to_char32_t(ord('a')))
    stl_so.stl_ip_test.char32_t_types.vector.push_back(stl_so.stl_ip_test.to_char32_t(ord('b')))
    stl_so.stl_ip_test.char32_t_types.vector.push_back(stl_so.stl_ip_test.to_char32_t(ord('c')))
    '''
    stl_so.stl_ip_test.addCharToVector(stl_so.stl_ip_test.char32_t_types.vector, ord('a'))
    stl_so.stl_ip_test.addCharToVector(stl_so.stl_ip_test.char32_t_types.vector, ord('b'))
    stl_so.stl_ip_test.addCharToVector(stl_so.stl_ip_test.char32_t_types.vector, ord('c'))
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.char32_t_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.getCharFromVector(stl_so.stl_ip_test.char32_t_types.vector, 0), ord('a'), test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.getCharFromVector(stl_so.stl_ip_test.char32_t_types.vector, 1), ord('b'), test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.getCharFromVector(stl_so.stl_ip_test.char32_t_types.vector, 2), ord('c'), test_suite, test_name)
    try:
        test_name = "char32_t Vectors"
        stl_so.stl_ip_test.char32_t_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<char32_t> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)

    test_name = "short int Vectors"
    stl_so.stl_ip_test.short_int_types.vector.push_back(1)
    stl_so.stl_ip_test.short_int_types.vector.push_back(2)
    stl_so.stl_ip_test.short_int_types.vector.push_back(3)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.vector[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.vector[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.short_int_types.vector[2], 3, test_suite, test_name)
    try:
        test_name = "short int Vectors"
        stl_so.stl_ip_test.short_int_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<short int> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    

    test_name = "unsigned short int Vectors"
    stl_so.stl_ip_test.unsigned_short_int_types.vector.push_back(1)
    stl_so.stl_ip_test.unsigned_short_int_types.vector.push_back(2)
    stl_so.stl_ip_test.unsigned_short_int_types.vector.push_back(3)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.vector[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.vector[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_short_int_types.vector[2], 3, test_suite, test_name)
    #Pass invalid type(struct)
    try:
        test_name = "unsigned short int Vectors"
        stl_so.stl_ip_test.unsigned_short_int_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<unsigned short int> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
    #Pass invalid type(signed value)
    try:
        test_name = "unsigned short int Vectors"
        stl_so.stl_ip_test.unsigned_short_int_types.vector.push_back(-1)

    except Exception as e:
        print(f"Expected exception due to passing signed value into push_back for std::vector<unsigned short int> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)



    test_name = "int Vectors"
    stl_so.stl_ip_test.int_types.vector.push_back(1)
    stl_so.stl_ip_test.int_types.vector.push_back(2)
    stl_so.stl_ip_test.int_types.vector.push_back(3)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.vector[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.vector[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.int_types.vector[2], 3, test_suite, test_name)

    try:
        test_name = "int Vectors"
        stl_so.stl_ip_test.int_types.vector.push_back(trick.TestStructure())
    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<int> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)

    test_name = "unsigned int Vectors"
    stl_so.stl_ip_test.unsigned_int_types.vector.push_back(1)
    stl_so.stl_ip_test.unsigned_int_types.vector.push_back(2)
    stl_so.stl_ip_test.unsigned_int_types.vector.push_back(3)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.vector.size(), 3, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.vector[0], 1, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.vector[1], 2, test_suite, test_name)
    TRICK_EXPECT_EQ(stl_so.stl_ip_test.unsigned_int_types.vector[2], 3, test_suite, test_name)
    #Pass invalid type(struct)
    try:
        test_name = "unsigned int Vectors"
        stl_so.stl_ip_test.unsigned_int_types.vector.push_back(trick.TestStructure())

    except Exception as e:
        print(f"Expected exception due to passing TestStruct into push_back for std::vector<unsigned int> : {e}")
        TRICK_EXPECT_GT(len(e.__str__()), 0,  test_suite, test_name)
