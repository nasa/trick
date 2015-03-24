/*
    PURPOSE: (Illustrate how to checkpoint STLs)
    LIBRARY_DEPENDENCIES: (
      (TrickSTLCheckpoint.o)
    )
*/

#ifndef TRICKSTLCHECKPOINT_HH_
#define TRICKSTLCHECKPOINT_HH_

#include <string>
#include "sim_services/STL/include/trick_vector.hh"
#include "sim_services/STL/include/trick_list.hh"
#include "sim_services/STL/include/trick_deque.hh"
#include "sim_services/STL/include/trick_set.hh"
#include "sim_services/STL/include/trick_map.hh"
#include "sim_services/STL/include/trick_stack.hh"
#include "sim_services/STL/include/trick_queue.hh"
#include "sim_services/STL/include/trick_pair.hh"

class ClassOfInts {
    public:
        int i1 ;
        int i2 ;
        int i3 ;
        bool operator < ( const ClassOfInts & c ) const {
            return i1 > c.i1 ;
        }
} ;

class TrickSTLCheckpoint {

    public:

        TrickSTLCheckpoint() ;
        int speak() ;

        std::string name ;

        /* VECTOR */
        Trick::vector< double > vector_double ;
        Trick::vector< double , std::allocator<double> > vector_double_alloc ;
        Trick::vector< std::string > vector_string ;

        Trick::vector< double > vector_range ;
        Trick::vector< double > vector_fill ;
        Trick::vector< double > vector_copy ;

        Trick::vector< ClassOfInts > vector_class ;
        Trick::vector< ClassOfInts * > vector_class_ptr ;

        Trick::vector< Trick::vector< double > > vector_vector_double ;

        /* LIST */
        Trick::list< double > list_double ;
        Trick::list< double , std::allocator<double> > list_double_alloc ;
        Trick::list< std::string > list_string ;

        Trick::list< double > list_range ;
        Trick::list< double > list_fill ;
        Trick::list< double > list_copy ;

        Trick::list< ClassOfInts > list_class ;
        Trick::list< ClassOfInts * > list_class_ptr ;

        Trick::list< Trick::list< double > > list_list_double ;

        /* DEQUE */
        Trick::deque< double > deque_double ;
        Trick::deque< double , std::allocator<double> > deque_double_alloc ;
        Trick::deque< std::string > deque_string ;

        Trick::deque< double > deque_range ;
        Trick::deque< double > deque_fill ;
        Trick::deque< double > deque_copy ;

        Trick::deque< ClassOfInts > deque_class ;
        Trick::deque< ClassOfInts * > deque_class_ptr ;

        Trick::deque< Trick::deque< double > > deque_deque_double ;

        /* SET */
        Trick::set< double > set_double ;
        Trick::set< double , std::less< double > > set_double_compare ;
        Trick::set< double , std::less< double > , std::allocator<double> > set_double_compare_alloc ;
        Trick::set< std::string > set_string ;

        Trick::set< double > set_range ;
        Trick::set< double > set_fill ;
        Trick::set< double > set_copy ;

        Trick::set< ClassOfInts > set_class ;
        Trick::set< ClassOfInts * > set_class_ptr ;

        Trick::set< Trick::set< double > > set_set_double ;

        /* MULTISET */
        Trick::multiset< double > multiset_double ;
        Trick::multiset< double , std::less< double > > multiset_double_compare ;
        Trick::multiset< double , std::less< double > , std::allocator<double> > multiset_double_compare_alloc ;
        Trick::multiset< std::string > multiset_string ;

        Trick::multiset< double > multiset_range ;
        Trick::multiset< double > multiset_fill ;
        Trick::multiset< double > multiset_copy ;

        Trick::multiset< ClassOfInts > multiset_class ;
        Trick::multiset< ClassOfInts * > multiset_class_ptr ;

        Trick::multiset< Trick::multiset< double > > multiset_multiset_double ;

        /* MAP */
        Trick::map< double , double > map_double_double ;
        Trick::map< int , std::string > map_int_string ;
        Trick::map< int , Trick::vector< double > > map_int_vector_double ;

        Trick::map< std::string , int > map_string_int ;
        Trick::map< std::string , std::string > map_string_string ;
        Trick::map< std::string , Trick::vector< double > > map_string_vector_double ;

        Trick::map< Trick::list< int > , double > map_list_int_double ;
        Trick::map< Trick::list< int > , std::string > map_list_int_string ;
        Trick::map< Trick::list< int > , Trick::vector< double > > map_list_int_vector_double ;

        /* MULTIMAP */
        Trick::multimap< double , double > multimap_double_double ;
        Trick::multimap< int , std::string > multimap_int_string ;
        Trick::multimap< int , Trick::vector< double > > multimap_int_vector_double ;

        Trick::multimap< std::string , int > multimap_string_int ;
        Trick::multimap< std::string , std::string > multimap_string_string ;
        Trick::multimap< std::string , Trick::vector< double > > multimap_string_vector_double ;

        Trick::multimap< Trick::list< int > , double > multimap_list_int_double ;
        Trick::multimap< Trick::list< int > , std::string > multimap_list_int_string ;
        Trick::multimap< Trick::list< int > , Trick::vector< double > > multimap_list_int_vector_double ;

        /* STACK */
        Trick::stack< unsigned int > stack_uint ;
        Trick::stack< std::string > stack_string ;
        Trick::stack< Trick::vector< double > > stack_vector_double ;

        /* QUEUE */
        Trick::queue< unsigned int > queue_uint ;
        Trick::queue< std::string > queue_string ;
        Trick::queue< Trick::vector< double > > queue_vector_double ;

        /* PRIORITY QUEUE */
        Trick::priority_queue< unsigned int > priority_queue_uint ;
        Trick::priority_queue< std::string > priority_queue_string ;
        Trick::priority_queue< Trick::vector< double > > priority_queue_vector_double ;

        /* PAIRS */
        Trick::pair< int , int > pair_int_int ;
        Trick::pair< int , int > pair_fill_int_int ;
        Trick::pair< int , std::string > pair_int_string ;
        Trick::pair< int , Trick::list< int > > pair_int_list_int ;
        Trick::pair< std::string , int > pair_string_int ;
        Trick::pair< std::string , std::string > pair_string_string ;
        Trick::pair< std::string , Trick::list< int > > pair_string_list_int ;
        Trick::pair< Trick::list< int > , int > pair_list_int_int ;
        Trick::pair< Trick::list< int > , std::string > pair_list_int_string ;
        Trick::pair< Trick::list< int > , Trick::list< int > > pair_list_int_list_int ;
} ;

#endif

