
#include <math.h>

#ifdef TRICK_UNIT_TEST
#define TRICK_EXPECT_TRUE( a , test_suite , test_case) \
    if ( a ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_TRUE failed" ) ; \
    }

#define TRICK_EXPECT_FALSE( a , test_suite , test_case) \
    if ( !(a) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_FALSE failed" ) ; \
    }

#define TRICK_EXPECT_EQ( a , b , test_suite , test_case) \
    if ( (a) == (b) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_EQ failed" ) ; \
    }

#define TRICK_EXPECT_NE( a , b , test_suite , test_case) \
    if ( (a) != (b) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_NE failed" ) ; \
    }

#define TRICK_EXPECT_LT( a , b , test_suite , test_case) \
    if ( (a) < (b) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_LT failed" ) ; \
    }

#define TRICK_EXPECT_LE( a , b , test_suite , test_case) \
    if ( (a) <= (b) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_LE failed" ) ; \
    }

#define TRICK_EXPECT_GT( a , b , test_suite , test_case) \
    if ( (a) > (b) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_GT failed" ) ; \
    }

#define TRICK_EXPECT_GE( a , b , test_suite , test_case) \
    if ( (a) >= (b) ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "TRICK_EXPECT_GE failed" ) ; \
    }

#define TRICK_EXPECT_NEAR( a , b , tol , test_suite , test_case) \
    if ( fabs ( (a) - (b) ) < tol ) { \
        add_test_result( test_suite , test_case , "" ) ; \
    } else { \
        add_test_result( test_suite , test_case , "floating point not within tolerance" ) ; \
    }

#else
#define TRICK_EXPECT_TRUE( a , test_suite , test_case)
#define TRICK_EXPECT_FALSE( a , test_suite , test_case)
#define TRICK_EXPECT_EQ( a , test_suite , test_case)
#define TRICK_EXPECT_NE( a , test_suite , test_case)
#define TRICK_EXPECT_LT( a , test_suite , test_case)
#define TRICK_EXPECT_LE( a , test_suite , test_case)
#define TRICK_EXPECT_GT( a , test_suite , test_case)
#define TRICK_EXPECT_GE( a , test_suite , test_case)
#define TRICK_EXPECT_NEAR( a , b , tol , test_suite , test_case)
#endif

#ifdef __cplusplus
extern "C" {
#endif

int trick_test_enable(void) ;

int trick_test_set_file_name( const char * in_file_name ) ;

int trick_test_set_test_name( const char * in_test_name ) ;

int trick_test_add_parent(const char * in_test_suite_name,
                          const char * in_test_case,
                          const char * par_num ) ;

int add_test_result( const char * in_test_suite_name ,
                     const char * in_test_case ,
                     const char * in_failure_string ) ;
int call_write_output(void) ;

#ifdef __cplusplus
}
#endif

