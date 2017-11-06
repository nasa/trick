
import math
import trick

def TRICK_EXPECT_TRUE( a , test_suite, test_case):
    if a:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_TRUE failed") ;

def TRICK_EXPECT_FALSE( a , test_suite, test_case):
    if not a:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_FALSE failed") ;

def TRICK_EXPECT_EQ( a , b, test_suite, test_case):
    if a == b:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EQ failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

def TRICK_EXPECT_NE( a , b, test_suite, test_case):
    if a != b:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_EQ failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

def TRICK_EXPECT_LT( a , b, test_suite, test_case):
    if a < b:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_LT failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

def TRICK_EXPECT_LE( a , b, test_suite, test_case):
    if a <= b:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_LE failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

def TRICK_EXPECT_GT( a , b, test_suite, test_case):
    if a > b:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_GT failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

def TRICK_EXPECT_GE( a , b, test_suite, test_case):
    if a >= b:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_GE failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

def TRICK_EXPECT_NEAR( a , b, tol, test_suite, test_case):
    if math.fabs(a - b) < tol:
        trick.add_test_result( test_suite , test_case , "") ;
    else:
        trick.add_test_result( test_suite , test_case , "TRICK_EXPECT_NEAR failed") ;
        print ("a = {}".format(a))
        print ("b = {}".format(b))

