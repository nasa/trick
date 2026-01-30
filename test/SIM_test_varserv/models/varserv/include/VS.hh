/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Variable server test )
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) ) ( (Jackie Deans) (CACI) (11-30-2022) )
********************************************************************************/
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"
#include <string.h>
#include <string>
#include <wchar.h>

#ifndef VS_HH
#define VS_HH

typedef struct
{
    unsigned char var1 : 3;
    short var2 : 8;
    int var3 : 9;
    unsigned int var4 : 12;
} bitfield;

// Enum for testing STL containers of enums
typedef enum
{
    COLOR_RED = 0,
    COLOR_GREEN = 1,
    COLOR_BLUE = 2,
    COLOR_YELLOW = 3
} Color;

// Simple class for testing STL containers of structured types
class PointLabel
{
public:
    char label_text[35];
};

class Point
{
public:
    double x;
    double y;
    // std::vector<double> coords;

    PointLabel point_label_obj;
    PointLabel * point_dyn_label_array;
    PointLabel point_static_label_array[3];

    Point()
        : x(0.0),
          y(0.0),
          point_label_obj(),
          point_dyn_label_array(nullptr)
    {
    }

    Point(double x_val, double y_val)
        : x(x_val),
          y(y_val),
          point_label_obj(),
          point_dyn_label_array(nullptr)
    {
    }
};

class VSTest
{
public:
    char a;
    unsigned char b;
    short c;
    unsigned short d;
    int e; /*  m xy-position */
    unsigned int f;
    long g;
    unsigned long h;
    float i;
    double j;
    long long k;
    unsigned long long l;
    bool m;
    int n[5];
    std::string o;
    char * p;
    wchar_t * q; /**< -- trick_chkpnt_io(**) */

    bitfield my_bitfield;

    int large_arr[4000];

    int blocked_from_input;  /**< -- trick_io(*o) */
    int blocked_from_output; /**< -- trick_io(*i) */

    int status;

    // Vector containers for different types
    std::vector<int> vec_int;
    std::vector<std::string> vec_string;
    std::vector<double> vec_double;
    std::vector<bool> vec_bool;
    std::vector<short> vec_short;
    std::vector<unsigned short> vec_ushort;
    std::vector<unsigned int> vec_uint;
    std::vector<long> vec_long;
    std::vector<unsigned long> vec_ulong;
    std::vector<float> vec_float;
    std::vector<long long> vec_llong;
    std::vector<unsigned long long> vec_ullong;

    // Deque containers
    std::deque<float> deq_float;
    std::deque<unsigned int> deq_uint;
    std::deque<std::string> deq_string;
    std::deque<short> deq_short;
    std::deque<long long> deq_llong;

    // Array containers
    std::array<long, 5> arr_long;
    std::array<unsigned short, 4> arr_ushort;
    std::array<std::string, 3> arr_string;
    std::array<double, 4> arr_double;
    std::array<int, 3> arr_int;

    // Enum and structured type containers
    std::vector<Color> vec_enum;
    std::deque<Color> deq_enum;
    std::array<Color, 4> arr_enum;

    std::vector<Point> vec_point;
    std::deque<Point> deq_point;
    std::array<Point, 3> arr_point;

    std::vector<Point *> vec_point_ptr;

    Point * point_dyn_array;
    Point point_static_array[2];

    VSTest();
    ~VSTest();

    int init();
    int default_vars();

    int shutdown();

    int success();
    int fail();

    void throw_exception();

    const char * status_messages[3] = {"Variable Server Test Success",
                                       "Variable Server Test Failure",
                                       "Client failed to connect"};
};

#endif
