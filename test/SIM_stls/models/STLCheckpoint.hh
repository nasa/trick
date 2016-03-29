/*
    PURPOSE: (Illustrate how to checkpoint STLs) 
    LIBRARY_DEPENDENCIES: (
      (STLCheckpoint.o)
    )
*/

#ifndef STLCHECKPOINT_HH
#define STLCHECKPOINT_HH

#include <map>
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <string>
#include <stack>
#include <queue>
#include <utility>

class STLCheckpoint {

    public:
    
        STLCheckpoint() ;
        STLCheckpoint(std::string in_name) ;
        int speak() ;

        std::string name ;

        std::map< double , double > my_double_map ;
        std::map< std::string , int > my_string_key_map ;
        std::map< int , std::string > my_string_data_map ;
        std::map< std::string , std::string > my_string_map ;

        std::multimap< int , int > my_int_multimap ;
        std::multimap< std::string , int > my_string_key_multimap ;
        std::multimap< int , std::string > my_string_data_multimap ;
        std::multimap< std::string , std::string > my_string_multimap ;

        std::vector< double > my_double_vector ;
        std::vector< std::string > my_string_vector ;

        std::list< short > my_short_list ;
        std::list< std::string > my_string_list ;

        std::deque< float > my_float_deque ;
        std::deque< std::string > my_string_deque ;

        std::set< int > my_int_set ;
        std::set< std::string > my_string_set ;

        std::multiset< long > my_long_multiset ;
        std::multiset< std::string > my_string_multiset ;

        std::stack< unsigned int > my_uint_stack ;
        std::stack< std::string > my_string_stack ;

        std::queue< int > my_int_queue ;
        std::queue< std::string > my_string_queue ;

        std::priority_queue< int > my_int_priority_queue ;
        std::priority_queue< std::string > my_string_priority_queue ;

        std::pair< int , int > my_int_pair ;
        std::pair< std::string , int > my_string_first_pair ;
        std::pair< int , std::string > my_string_second_pair ;
        std::pair< std::string , std::string > my_string_pair ;

        std::vector< std::vector< double > > my_vector_vector_double ;
        std::vector< std::vector< std::vector< double > > > my_vector_vector_vector_double ;
        //std::vector< std::list< double >  > my_vector_list_double ;
} ;

#endif

