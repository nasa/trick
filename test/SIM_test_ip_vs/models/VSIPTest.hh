/*
    PURPOSE: (Dynamic arrays and STL bounds checking via variable_server Python interface.)
    LIBRARY_DEPENDENCIES: (
      (VSIPTest.o)
    )
*/

#ifndef VSIPTEST_HH
#define VSIPTEST_HH

#include <array>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

class SimpleWrapper {
    public:
        int a;
        std::vector<int> vec;
};

class UserClass {
    public:
        int a[5];
        long long b;
        std::string c;
        UserClass * d;
};

class VSIPTest {

    public:

        VSIPTest() {};
        int addData() ;

        // Dynamic arrays for testing
        double* dp; /* -- blah */
        double* dap[4]; /* -- blah */
        double** dpp; /* -- blah */
        double*** dppp; /* -- blah */

        SimpleWrapper** swp; /* -- blah */
        SimpleWrapper*** swpp; /* -- blah */


        // STL containers for testing
        std::vector< double > double_vector ;
        std::vector< std::string > string_vector ;

        std::deque< float > float_deque ;
        std::deque< std::string > string_deque ;

        std::array<int, 4> int_array;
        std::array< std::string, 5> string_array;

        std::vector<UserClass> vec_user_simple;
        std::vector<SimpleWrapper> vec_user_defined;
        std::vector<SimpleWrapper *> vec_user_defined_ptr;

} ;

#endif

