
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

STLCheckpoint::STLCheckpoint(std::string in_name) {

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
    return ;
}

int STLCheckpoint::speak() {
    message_publish(1,"Quack!\n") ;
    return 0 ;
}

