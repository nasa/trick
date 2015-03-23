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

/* This shows the most difficult case in checkpointing STLs where all
   STLs are private.  There are no accessor routines to get individual
   items out of the STLs. (In fact getting to all items in some of the
   STLs forces changes in the contents, such as stacks and queues.) Anyways
   in this case we need to specify a friend that carries out the checkpoint.
   There are 2 methods one can use to call the Trick stl checkpoint routines:
   1) Provide a companion class that calls the Trick routines for each STL
      (STLCompanion in this example).
    -- OR --
   2) Invoke Trick's CHECKPOINT_STL macro in the sim object class for each STL
      (theSimObject in this example).
   Whichever method is chosen, make that class a friend of the class with STLs
      (STLCheckpoint in this example).
   Public STLs can be checkpointed in the same way (but no need to specify friend).
*/
class STLCheckpoint {

    friend class STLCompanion ; // method 1
    friend class theSimObject ; // method 2

    public:
    
        STLCheckpoint() ;
        STLCheckpoint(std::string in_name) ;
        int speak() ;

        std::string name ;

    private:

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

} ;

#endif

