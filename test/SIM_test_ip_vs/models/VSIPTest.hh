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

class SimpleWrapper
{
    public:
        int a;
        std::vector<int> vec;
};

class UserClass
{
    public:
        int a[5];
        long long b;
        std::string c;
        UserClass* d;
};

enum class Color {
    RED,
    GREEN,
    BLUE
};

class VSIPTest
{
    public:
        VSIPTest() { };
        int addData();

        // Dynamic arrays for testing
        Color* color_p; /* -- blah */
        Color* color_ap[4]; /* -- blah */
        Color** color_pp;   /* -- blah */
        Color*** color_ppp; /* -- blah */

        short* sp;     /* -- blah */
        short* sap[4]; /* -- blah */
        short** spp;   /* -- blah */
        short*** sppp; /* -- blah */

        unsigned short* usp;     /* -- blah */
        unsigned short* usap[4]; /* -- blah */
        unsigned short** uspp;   /* -- blah */
        unsigned short*** usppp; /* -- blah */

        int* ip;     /* -- blah */
        int* iap[4]; /* -- blah */
        int** ipp;   /* -- blah */
        int*** ippp; /* -- blah */

        unsigned int* uip;     /* -- blah */
        unsigned int* uiap[4]; /* -- blah */
        unsigned int** uipp;   /* -- blah */
        unsigned int*** uippp; /* -- blah */

        long* lp;     /* -- blah */
        long* lap[4]; /* -- blah */
        long** lpp;   /* -- blah */
        long*** lppp; /* -- blah */

        unsigned long* ulp;     /* -- blah */
        unsigned long* ulap[4]; /* -- blah */
        unsigned long** ulpp;   /* -- blah */
        unsigned long*** ulppp; /* -- blah */

        long long* llp;     /* -- blah */
        long long* llap[4]; /* -- blah */
        long long** llpp;   /* -- blah */
        long long*** llppp; /* -- blah */

        unsigned long long* ullp;     /* -- blah */
        unsigned long long* ullap[4]; /* -- blah */
        unsigned long long** ullpp;   /* -- blah */
        unsigned long long*** ullppp; /* -- blah */

        float* fp;     /* -- blah */
        float* fap[4]; /* -- blah */
        float** fpp;   /* -- blah */
        float*** fppp; /* -- blah */

        double* dp;     /* -- blah */
        double* dap[4]; /* -- blah */
        double** dpp;   /* -- blah */
        double*** dppp; /* -- blah */

        bool* bp;     /* -- blah */
        bool* bap[4]; /* -- blah */
        bool** bpp;   /* -- blah */
        bool*** bppp; /* -- blah */

        SimpleWrapper** swp;   /* -- blah */
        SimpleWrapper*** swpp; /* -- blah */

        // STL containers for testing
        std::vector<Color> color_vector;
        std::vector<short> short_vector;
        std::vector<unsigned short> unsigned_short_vector;
        std::vector<int> int_vector;
        std::vector<unsigned int> unsigned_int_vector;
        std::vector<long> long_vector;
        std::vector<unsigned long> unsigned_long_vector;
        std::vector<long long> long_long_vector;
        std::vector<unsigned long long> unsigned_long_long_vector;
        std::vector<float> float_vector;
        std::vector<double> double_vector;
        std::vector<bool> bool_vector;
        std::vector<std::string> string_vector;

        std::deque<float> float_deque;
        std::deque<std::string> string_deque;

        std::array<int, 4> int_array;
        std::array<std::string, 5> string_array;

        std::vector<UserClass> vec_user_simple;
        std::vector<SimpleWrapper> vec_user_defined;
        std::vector<SimpleWrapper*> vec_user_defined_ptr;
};

#endif
