/******************************TRICK HEADER*************************************
PURPOSE:                     ( Test variable server )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( VS.o )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) 
								(Jackie Deans) (CACI) (11-30-2022) )
*******************************************************************************/
#include <iostream>
#include <stdexcept>
#include <limits>

#include "../include/VS.hh"
#include "trick/exec_proto.h"
#include "trick/memorymanager_c_intf.h"

VSTest::VSTest() {}
VSTest::~VSTest() {}

int VSTest::default_vars() {

	a = 'a';            //char
	b = 'b';            //unsigned char
	c = -1234;          //short
	d = 1234;           //unsigned short
	e = -123456;        //int
	f = 123456;         //unsigned int
	g = -1234567;       //long
	h = 123456789;      //unsigned long
	i = 1234.5678;      //float
	j = -1234.567890;   //double
	k = -12345678912345;//long long
	l = 12345678912345; //unsigned long
	m = true;          //boolean
	for (int i = 0; i < 5; i++) {
		n[i] = i;
	}
	o = std::string("You will rejoice to hear that no disaster has accompanied the commencement of an enterprise which you have regarded with such evil forebodings. I arrived here yesterday, and my first task is to assure my dear sister of my welfare and increasing confidence in the success of my undertaking.");
	p = const_cast<char *>("I am already far north of London, and as I walk in the streets of Petersburgh, I feel a cold northern breeze play upon my cheeks, which braces my nerves and fills me with delight. Do you understand this feeling?");
	q = const_cast<wchar_t *>(L"This breeze, which has travelled from the regions towards which I am advancing, gives me a foretaste of those icy climes. Inspirited by this wind of promise, my daydreams become more fervent and vivid.");

	for (int i = 0; i < 4000; i++) {
		large_arr[i] = i;
	}

	/* Mixed Types */
	// 3,-128,0,2112
    my_bitfield.var1 = (1 << (3 - 1)) - 1;
    my_bitfield.var2 = -(1 << (8 - 1));
    my_bitfield.var3 = 0;
    my_bitfield.var4 = (1 << (12 - 1)) + (1 << 12/2);;


	blocked_from_input = 500;
	blocked_from_output = 1000;

    // Initialize vector containers
    vec_int.push_back(10);
    vec_int.push_back(20);
    vec_int.push_back(30);

    vec_string.push_back("Hello");
    vec_string.push_back("Variable");
    vec_string.push_back("Server");

    vec_double.push_back(1.1);
    vec_double.push_back(2.2);
    vec_double.push_back(3.3);
    vec_double.push_back(4.4);

    vec_bool.push_back(true);
    vec_bool.push_back(false);
    vec_bool.push_back(true);
    vec_bool.push_back(true);
    vec_bool.push_back(false);

    vec_short.push_back(-100);
    vec_short.push_back(-200);
    vec_short.push_back(-300);

    vec_ushort.push_back(100);
    vec_ushort.push_back(200);
    vec_ushort.push_back(300);

    vec_uint.push_back(1000u);
    vec_uint.push_back(2000u);
    vec_uint.push_back(3000u);

    vec_long.push_back(10000L);
    vec_long.push_back(20000L);
    vec_long.push_back(30000L);

    vec_ulong.push_back(100000UL);
    vec_ulong.push_back(200000UL);
    vec_ulong.push_back(300000UL);

    vec_float.push_back(1.5f);
    vec_float.push_back(2.5f);
    vec_float.push_back(3.5f);

    vec_llong.push_back(1000000LL);
    vec_llong.push_back(2000000LL);
    vec_llong.push_back(3000000LL);

    vec_ullong.push_back(10000000ULL);
    vec_ullong.push_back(20000000ULL);
    vec_ullong.push_back(30000000ULL);

    // Initialize deque containers
    deq_float.push_back(1.5f);
    deq_float.push_back(2.5f);
    deq_float.push_back(3.5f);
    deq_float.push_back(4.5f);

    deq_uint.push_back(100u);
    deq_uint.push_back(200u);
    deq_uint.push_back(300u);

    deq_string.push_back("Deque");
    deq_string.push_back("of");
    deq_string.push_back("Strings");

    deq_short.push_back(-10);
    deq_short.push_back(-20);
    deq_short.push_back(-30);

    deq_llong.push_back(111111LL);
    deq_llong.push_back(222222LL);
    deq_llong.push_back(333333LL);

    // Initialize array containers
    arr_long = {1000L, 2000L, 3000L, 4000L, 5000L};
    arr_ushort = {10u, 20u, 30u, 40u};
    arr_string = {"Array", "of", "Strings"};
    arr_double = {10.5, 20.5, 30.5, 40.5};
    arr_int = {100, 200, 300};

    // Initialize enum containers
    vec_enum.push_back(COLOR_RED);
    vec_enum.push_back(COLOR_GREEN);
    vec_enum.push_back(COLOR_BLUE);

    deq_enum.push_back(COLOR_GREEN);
    deq_enum.push_back(COLOR_YELLOW);
    deq_enum.push_back(COLOR_RED);

    arr_enum = {COLOR_BLUE, COLOR_RED, COLOR_GREEN, COLOR_YELLOW};

    // Initialize Point (structured type) containers
    vec_point.push_back(Point(1.0, 2.0));
    vec_point.push_back(Point(3.0, 4.0));
    vec_point.push_back(Point(5.0, 6.0));

    // vector<class>.vector<supported_type> not supported yet
    //vec_point[0].coords = {1.1, 1.2, 1.3};
    //vec_point[1].coords = {2.1, 2.2, 2.3, 2.4, 2.5};
    //vec_point[2].coords = {3.1, 3.2, 3.3};

    deq_point.push_back(Point(10.0, 20.0));
    deq_point.push_back(Point(30.0, 40.0));
    deq_point.push_back(Point(50.0, 60.0));

    arr_point = {Point(100.0, 200.0), Point(300.0, 400.0), Point(500.0, 600.0)};

    // Initialize vector of Point pointers
    Point* p1 = (Point*)TMM_declare_var_s("Point");
    p1->x = 11.0;
    p1->y = 22.0;
    vec_point_ptr.push_back(p1);
    Point* p2 = (Point*)TMM_declare_var_s("Point");
    p2->x = 33.0;
    p2->y = 44.0;
    vec_point_ptr.push_back(p2);
    Point* p3 = (Point*)TMM_declare_var_s("Point");
    p3->x = 55.0;
    p3->y = 66.0;
    vec_point_ptr.push_back(p3);

	return 0;
}

int VSTest::init() {
	status = 2; 
	return (0);
}

int VSTest::fail() {
    status = 1; 
	return 0;
}

int VSTest::success() {
    status = 0;
	return 0; 
}

void VSTest::throw_exception() {
	throw std::logic_error("Pretend an error has occured for testing");
}

int VSTest::shutdown() {
	std::cout << "Shutting down with status: " << status << " Message: " << status_messages[status] << std::endl;

	// Check that the blocked_from_input variable hasnt changed
	if (blocked_from_input != 500 || blocked_from_output != 0) {
		status = 1;
	}

    exec_terminate_with_return( status , __FILE__ , __LINE__ , status_messages[status] ) ;
	
	return(0);
}