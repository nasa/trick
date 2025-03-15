/*
    PURPOSE: (Illustrate how to checkpoint STLs)
    LIBRARY_DEPENDENCIES: (
      ()
    )
*/


#define FOREACH_TYPE(MACRO)              \
    MACRO(char, char)                    \
    MACRO(signed char, signed_char)      \
    MACRO(unsigned char, unsigned_char)  \
    MACRO(char16_t, char16_t)            \
    MACRO(char32_t, char32_t)            \
    MACRO(short int, short_int)          \
    MACRO(unsigned short int, unsigned_short_int) \
    MACRO(int, int)                      \
    MACRO(unsigned int, unsigned_int)    \
    MACRO(long int, long_int)            \
    MACRO(unsigned long int, unsigned_long_int) \
    MACRO(long long int, long_long_int)  \
    MACRO(unsigned long long int, unsigned_long_long_int) \
    MACRO(float, float)                  \
    MACRO(double, double)                \
    MACRO(long double, long_double)      
