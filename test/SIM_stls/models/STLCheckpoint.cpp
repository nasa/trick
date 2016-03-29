
#include "sim_services/Message/include/message_proto.h"
#include "STLCheckpoint.hh"

/* These 2 constructors add different data to an STLCheckpoint. */

STLCheckpoint::STLCheckpoint() {

    my_double_map[11.1] = 111.1 ;
    my_double_map[22.2] = 222.2 ;
    my_double_map[33.3] = 333.3 ;

    my_string_key_map[std::string("one")] = 1 ;
    my_string_key_map[std::string("two")] = 2 ;
    my_string_key_map[std::string("three")] = 3 ;

    my_string_data_map[4] = std::string("vier") ;
    my_string_data_map[5] = std::string("fumf") ;
    my_string_data_map[6] = std::string("sechs") ;

    my_string_map[std::string("mother")] = std::string("Marge") ;
    my_string_map[std::string("father")] = std::string("Homer") ;
    my_string_map[std::string("son")] = std::string("Bart") ;

    my_int_multimap.insert(std::pair<int, int>(11,111)) ;
    my_int_multimap.insert(std::pair<int, int>(22,222)) ;
    my_int_multimap.insert(std::pair<int, int>(33,333)) ;
    my_int_multimap.insert(std::pair<int, int>(11,111)) ;
    my_int_multimap.insert(std::pair<int, int>(22,222)) ;
    my_int_multimap.insert(std::pair<int, int>(33,333)) ;

    my_string_key_multimap.insert(std::pair<std::string, int>("one", 1)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("two", 2)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("three", 3)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("one", 1)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("two", 2)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("three", 3)) ;

    my_string_data_multimap.insert(std::pair<int, std::string>(4, "vier")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(5, "fumf")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(6, "sechs")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(4, "four")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(5, "five")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(6, "six")) ;

    my_string_multimap.insert(std::pair<std::string, std::string>("mother","Marge")) ;
    my_string_multimap.insert(std::pair<std::string, std::string>("father","Homer")) ;
    my_string_multimap.insert(std::pair<std::string, std::string>("mother","Lois")) ;
    my_string_multimap.insert(std::pair<std::string, std::string>("father","Meg")) ;

    my_double_vector.push_back(1.0) ;
    my_double_vector.push_back(2.0) ;
    my_double_vector.push_back(3.0) ;

    my_string_vector.push_back("I") ;
    my_string_vector.push_back("was") ;
    my_string_vector.push_back("here") ;

    my_short_list.push_back(300) ;
    my_short_list.push_back(301) ;
    my_short_list.push_back(302) ;

    my_string_list.push_back("I") ;
    my_string_list.push_back("was") ;
    my_string_list.push_back("there") ;

    my_float_deque.push_back(12.3) ;
    my_float_deque.push_back(45.6) ;
    my_float_deque.push_back(78.9) ;

    my_string_deque.push_back("meow") ;
    my_string_deque.push_back("bark") ;
    my_string_deque.push_back("quack") ;

    my_int_set.insert(8) ;
    my_int_set.insert(4) ;
    my_int_set.insert(2) ;
    my_int_set.insert(1) ;

    my_string_set.insert("e") ;
    my_string_set.insert("a") ;
    my_string_set.insert("d") ;

    my_long_multiset.insert(8) ;
    my_long_multiset.insert(4) ;
    my_long_multiset.insert(4) ;
    my_long_multiset.insert(2) ;
    my_long_multiset.insert(1) ;

    my_string_multiset.insert("e") ;
    my_string_multiset.insert("a") ;
    my_string_multiset.insert("d") ;
    my_string_multiset.insert("e") ;
    my_string_multiset.insert("a") ;
    my_string_multiset.insert("d") ;

    my_uint_stack.push(10) ;
    my_uint_stack.push(20) ;
    my_uint_stack.push(30) ;
    my_uint_stack.push(40) ;

    my_string_stack.push("abc I") ;
    my_string_stack.push("abc want the one") ;
    my_string_stack.push("abc with the bigger") ;
    my_string_stack.push("abc Gee Bees") ;

    my_int_queue.push(10) ;
    my_int_queue.push(20) ;
    my_int_queue.push(30) ;
    my_int_queue.push(40) ;

    my_string_queue.push("abc I") ;
    my_string_queue.push("abc want") ;
    my_string_queue.push("abc an") ;
    my_string_queue.push("abc iPhone 4") ;

    my_int_priority_queue.push(30) ;
    my_int_priority_queue.push(20) ;
    my_int_priority_queue.push(40) ;
    my_int_priority_queue.push(10) ;

    my_string_priority_queue.push("abc I") ;
    my_string_priority_queue.push("abc want") ;
    my_string_priority_queue.push("abc an") ;
    my_string_priority_queue.push("abc iPhone 4") ;

    my_int_pair.first = 1 ;
    my_int_pair.second = 2 ;

    my_string_first_pair.first = "abc string first" ;
    my_string_first_pair.second = 2 ;

    my_string_second_pair.first = 2 ;
    my_string_second_pair.second = "abc string second" ;

    my_string_pair.first = "abc pair first string" ;
    my_string_pair.second = "abc pair second string" ;
    return ;
}

STLCheckpoint::STLCheckpoint(std::string in_name) : 
  my_vector_vector_double(4, std::vector<double>(3)) ,
  my_vector_vector_vector_double(5, std::vector<std::vector<double> >(4, std::vector<double>(3)))
{

    name = in_name ;

    my_double_map[44.4] = 444.4 ;
    my_double_map[55.5] = 555.5 ;
    my_double_map[66.6] = 666.6 ;

    my_string_key_map[std::string("four")] = 4 ;
    my_string_key_map[std::string("five")] = 5 ;
    my_string_key_map[std::string("six")] = 6 ;

    my_string_data_map[7] = std::string("seiben") ;
    my_string_data_map[8] = std::string("acht") ;
    my_string_data_map[9] = std::string("neun") ;

    my_string_map[std::string("sister")] = std::string("Lisa") ;
    my_string_map[std::string("dog")] = std::string("Santa's Little Helper") ;

    my_int_multimap.insert(std::pair<int, int>(44,444)) ;
    my_int_multimap.insert(std::pair<int, int>(55,555)) ;
    my_int_multimap.insert(std::pair<int, int>(66,666)) ;
    my_int_multimap.insert(std::pair<int, int>(44,444)) ;
    my_int_multimap.insert(std::pair<int, int>(55,555)) ;
    my_int_multimap.insert(std::pair<int, int>(66,666)) ;

    my_string_key_multimap.insert(std::pair<std::string, int>("four", 4)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("five", 5)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("six", 6)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("four", 44)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("five",  55)) ;
    my_string_key_multimap.insert(std::pair<std::string, int>("six", 66)) ;

    my_string_data_multimap.insert(std::pair<int, std::string>(7, "seiben")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(8, "acht")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(9, "neun")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(7, "seven")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(8, "eight")) ;
    my_string_data_multimap.insert(std::pair<int, std::string>(9, "nine")) ;

    my_string_multimap.insert(std::pair<std::string, std::string>("sister","Lisa")) ;
    my_string_multimap.insert(std::pair<std::string, std::string>("dog","Santa's Little Helper")) ;
    my_string_multimap.insert(std::pair<std::string, std::string>("sister","Meg")) ;
    my_string_multimap.insert(std::pair<std::string, std::string>("dog","Brian")) ;

    my_double_vector.push_back(4.0) ;
    my_double_vector.push_back(5.0) ;
    my_double_vector.push_back(6.0) ;

    my_string_vector.push_back("It") ;
    my_string_vector.push_back("has") ;
    my_string_vector.push_back("the") ;
    my_string_vector.push_back("Wi-Fies") ;

    my_short_list.push_back(400) ;
    my_short_list.push_back(401) ;
    my_short_list.push_back(402) ;

    my_string_list.push_back("I") ;
    my_string_list.push_back("don't") ;
    my_string_list.push_back("care") ;

    my_float_deque.push_back(98.7) ;
    my_float_deque.push_back(65.4) ;
    my_float_deque.push_back(32.1) ;

    my_string_deque.push_back("Welcome") ;
    my_string_deque.push_back("to") ;
    my_string_deque.push_back("PhoneMart") ;

    my_int_set.insert(8000) ;
    my_int_set.insert(4000) ;
    my_int_set.insert(2000) ;
    my_int_set.insert(1000) ;

    my_string_set.insert("efg") ;
    my_string_set.insert("abc") ;
    my_string_set.insert("def") ;

    my_long_multiset.insert(8000) ;
    my_long_multiset.insert(4000) ;
    my_long_multiset.insert(4000) ;
    my_long_multiset.insert(2000) ;
    my_long_multiset.insert(1000) ;

    my_string_multiset.insert("efg") ;
    my_string_multiset.insert("abc") ;
    my_string_multiset.insert("def") ;
    my_string_multiset.insert("efg") ;
    my_string_multiset.insert("abc") ;
    my_string_multiset.insert("def") ;

    my_uint_stack.push(1) ;
    my_uint_stack.push(2) ;
    my_uint_stack.push(3) ;
    my_uint_stack.push(4) ;

    my_string_stack.push("I") ;
    my_string_stack.push("want the one") ;
    my_string_stack.push("with the bigger") ;
    my_string_stack.push("Gee Bees") ;

    my_int_queue.push(1) ;
    my_int_queue.push(2) ;
    my_int_queue.push(3) ;
    my_int_queue.push(4) ;

    my_string_queue.push("I") ;
    my_string_queue.push("want") ;
    my_string_queue.push("an") ;
    my_string_queue.push("iPhone 4") ;

    my_int_priority_queue.push(3) ;
    my_int_priority_queue.push(2) ;
    my_int_priority_queue.push(4) ;
    my_int_priority_queue.push(1) ;

    my_string_priority_queue.push("I") ;
    my_string_priority_queue.push("want") ;
    my_string_priority_queue.push("an") ;
    my_string_priority_queue.push("iPhone 4") ;

    my_int_pair.first = 10 ;
    my_int_pair.second = 20 ;

    my_string_first_pair.first = "string first" ;
    my_string_first_pair.second = 25 ;

    my_string_second_pair.first = 25 ;
    my_string_second_pair.second = "string second" ;

    my_string_pair.first = "pair first string" ;
    my_string_pair.second = "pair second string" ;

    my_vector_vector_double[0][0] = 100 ;
    my_vector_vector_double[0][1] = 101 ;
    my_vector_vector_double[0][2] = 102 ;
    my_vector_vector_double[1][0] = 103 ;
    my_vector_vector_double[1][1] = 104 ;
    my_vector_vector_double[1][2] = 105 ;
    my_vector_vector_double[2][0] = 106 ;
    my_vector_vector_double[2][1] = 107 ;
    my_vector_vector_double[2][2] = 108 ;
    my_vector_vector_double[3][0] = 109 ;
    my_vector_vector_double[3][1] = 110 ;
    my_vector_vector_double[3][2] = 111 ;

    my_vector_vector_vector_double[0][0][0] = 0 ;
    my_vector_vector_vector_double[0][0][1] = 1 ;
    my_vector_vector_vector_double[0][0][2] = 2 ;
    my_vector_vector_vector_double[0][1][0] = 3 ;
    my_vector_vector_vector_double[0][1][1] = 4 ;
    my_vector_vector_vector_double[0][1][2] = 5 ;
    my_vector_vector_vector_double[0][2][0] = 6 ;
    my_vector_vector_vector_double[0][2][1] = 7 ;
    my_vector_vector_vector_double[0][2][2] = 8 ;
    my_vector_vector_vector_double[0][3][0] = 9 ;
    my_vector_vector_vector_double[0][3][1] = 10 ;
    my_vector_vector_vector_double[0][3][2] = 11 ;

    my_vector_vector_vector_double[1][0][0] = 1000 ;
    my_vector_vector_vector_double[1][0][1] = 1001 ;
    my_vector_vector_vector_double[1][0][2] = 1002 ;
    my_vector_vector_vector_double[1][1][0] = 1003 ;
    my_vector_vector_vector_double[1][1][1] = 1004 ;
    my_vector_vector_vector_double[1][1][2] = 1005 ;
    my_vector_vector_vector_double[1][2][0] = 1006 ;
    my_vector_vector_vector_double[1][2][1] = 1007 ;
    my_vector_vector_vector_double[1][2][2] = 1008 ;
    my_vector_vector_vector_double[1][3][0] = 1009 ;
    my_vector_vector_vector_double[1][3][1] = 1010 ;
    my_vector_vector_vector_double[1][3][2] = 1011 ;

    my_vector_vector_vector_double[2][0][0] = 2000 ;
    my_vector_vector_vector_double[2][0][1] = 2001 ;
    my_vector_vector_vector_double[2][0][2] = 2002 ;
    my_vector_vector_vector_double[2][1][0] = 2003 ;
    my_vector_vector_vector_double[2][1][1] = 2004 ;
    my_vector_vector_vector_double[2][1][2] = 2005 ;
    my_vector_vector_vector_double[2][2][0] = 2006 ;
    my_vector_vector_vector_double[2][2][1] = 2007 ;
    my_vector_vector_vector_double[2][2][2] = 2008 ;
    my_vector_vector_vector_double[2][3][0] = 2009 ;
    my_vector_vector_vector_double[2][3][1] = 2010 ;
    my_vector_vector_vector_double[2][3][2] = 2011 ;

    my_vector_vector_vector_double[3][0][0] = 3000 ;
    my_vector_vector_vector_double[3][0][1] = 3001 ;
    my_vector_vector_vector_double[3][0][2] = 3002 ;
    my_vector_vector_vector_double[3][1][0] = 3003 ;
    my_vector_vector_vector_double[3][1][1] = 3004 ;
    my_vector_vector_vector_double[3][1][2] = 3005 ;
    my_vector_vector_vector_double[3][2][0] = 3006 ;
    my_vector_vector_vector_double[3][2][1] = 3007 ;
    my_vector_vector_vector_double[3][2][2] = 3008 ;
    my_vector_vector_vector_double[3][3][0] = 3009 ;
    my_vector_vector_vector_double[3][3][1] = 3010 ;
    my_vector_vector_vector_double[3][3][2] = 3011 ;

    my_vector_vector_vector_double[4][0][0] = 4000 ;
    my_vector_vector_vector_double[4][0][1] = 4001 ;
    my_vector_vector_vector_double[4][0][2] = 4002 ;
    my_vector_vector_vector_double[4][1][0] = 4003 ;
    my_vector_vector_vector_double[4][1][1] = 4004 ;
    my_vector_vector_vector_double[4][1][2] = 4005 ;
    my_vector_vector_vector_double[4][2][0] = 4006 ;
    my_vector_vector_vector_double[4][2][1] = 4007 ;
    my_vector_vector_vector_double[4][2][2] = 4008 ;
    my_vector_vector_vector_double[4][3][0] = 4009 ;
    my_vector_vector_vector_double[4][3][1] = 4010 ;
    my_vector_vector_vector_double[4][3][2] = 4011 ;

    return ;
}

int STLCheckpoint::speak() {
    //message_publish(1,"Quack!\n") ;
    //message_publish(1,"my_double_vector: %f %f %f\n", my_double_vector[0], my_double_vector[1], my_double_vector[2]) ;
    message_publish(1,"my_vector_vector_double[1]: %f %f %f\n",
     my_vector_vector_double[1][0], my_vector_vector_double[1][1], my_vector_vector_double[1][2]) ;
    message_publish(1,"my_vector_vector_vector_double[4][2]: %f %f %f\n",
     my_vector_vector_vector_double[4][2][0], my_vector_vector_vector_double[4][2][1], my_vector_vector_vector_double[4][2][2]) ;
    return 0 ;
}

