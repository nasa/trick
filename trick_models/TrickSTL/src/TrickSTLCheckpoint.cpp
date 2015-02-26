
#include "TrickSTLCheckpoint.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"

double matrix[3][3] = {{101,102,103},{104,105,106},{107,108,109}} ;

TrickSTLCheckpoint::TrickSTLCheckpoint()
 : vector_range(matrix[2], matrix[2] + 3) ,
   vector_fill( 5 , 200 ) ,
   vector_copy( vector_range ) ,
   list_range(matrix[2], matrix[2] + 3) ,
   list_fill( 5 , 200 ) ,
   list_copy( list_range ) ,
   deque_range(matrix[2], matrix[2] + 3) ,
   deque_fill( 5 , 200 ) ,
   deque_copy( deque_range ) ,
   pair_fill_int_int( 92 , 93 )
{
    ClassOfInts coi ;
    ClassOfInts * coi_ptr ;

    /* VECTOR */
    vector_double.push_back(1.0) ;
    vector_double.push_back(2.0) ;
    vector_double.push_back(3.0) ;

    vector_double_alloc.push_back(4.0) ;
    vector_double_alloc.push_back(5.0) ;
    vector_double_alloc.push_back(6.0) ;

    vector_string.push_back("I") ;
    vector_string.push_back("was") ;
    vector_string.push_back("here") ;

    coi.i1 = 1 ;
    coi.i2 = 2 ;
    coi.i3 = 3 ;
    vector_class.push_back(coi) ;
    coi.i1 = 4 ;
    coi.i2 = 5 ;
    coi.i3 = 6 ;
    vector_class.push_back(coi) ;

    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 11 ;
    coi_ptr->i2 = 12 ;
    coi_ptr->i3 = 13 ;
    vector_class_ptr.push_back(coi_ptr) ;
    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 14 ;
    coi_ptr->i2 = 15 ;
    coi_ptr->i3 = 16 ;
    vector_class_ptr.push_back(coi_ptr) ;

    Trick::vector< double > vector_d3( 3 , 300 ) ;
    Trick::vector< double > vector_d4( 4 , 400 ) ;
    Trick::vector< double > vector_d5( 5 , 500 ) ;

    vector_vector_double.push_back(vector_d3) ;
    vector_vector_double.push_back(vector_d4) ;
    vector_vector_double.push_back(vector_d5) ;

    /* LIST */
    list_double.push_back(1.0) ;
    list_double.push_back(2.0) ;
    list_double.push_back(3.0) ;

    list_double_alloc.push_back(4.0) ;
    list_double_alloc.push_back(5.0) ;
    list_double_alloc.push_back(6.0) ;

    list_string.push_back("I") ;
    list_string.push_back("was") ;
    list_string.push_back("here") ;

    coi.i1 = 1 ;
    coi.i2 = 2 ;
    coi.i3 = 3 ;
    list_class.push_back(coi) ;
    coi.i1 = 4 ;
    coi.i2 = 5 ;
    coi.i3 = 6 ;
    list_class.push_back(coi) ;

    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 11 ;
    coi_ptr->i2 = 12 ;
    coi_ptr->i3 = 13 ;
    list_class_ptr.push_back(coi_ptr) ;
    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 14 ;
    coi_ptr->i2 = 15 ;
    coi_ptr->i3 = 16 ;
    list_class_ptr.push_back(coi_ptr) ;

    Trick::list< double > list_d3( 3 , 300 ) ;
    Trick::list< double > list_d4( 4 , 400 ) ;
    Trick::list< double > list_d5( 5 , 500 ) ;
    list_list_double.push_back(list_d3) ;
    list_list_double.push_back(list_d4) ;
    list_list_double.push_back(list_d5) ;

    /* DEQUE */
    deque_double.push_back(1.0) ;
    deque_double.push_back(2.0) ;
    deque_double.push_back(3.0) ;

    deque_double_alloc.push_back(4.0) ;
    deque_double_alloc.push_back(5.0) ;
    deque_double_alloc.push_back(6.0) ;

    deque_string.push_back("I") ;
    deque_string.push_back("was") ;
    deque_string.push_back("here") ;

    coi.i1 = 1 ;
    coi.i2 = 2 ;
    coi.i3 = 3 ;
    deque_class.push_back(coi) ;
    coi.i1 = 4 ;
    coi.i2 = 5 ;
    coi.i3 = 6 ;
    deque_class.push_back(coi) ;

    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 11 ;
    coi_ptr->i2 = 12 ;
    coi_ptr->i3 = 13 ;
    deque_class_ptr.push_back(coi_ptr) ;
    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 14 ;
    coi_ptr->i2 = 15 ;
    coi_ptr->i3 = 16 ;
    deque_class_ptr.push_back(coi_ptr) ;

    Trick::deque< double > deque_d3( 3 , 300 ) ;
    Trick::deque< double > deque_d4( 4 , 400 ) ;
    Trick::deque< double > deque_d5( 5 , 500 ) ;
    deque_deque_double.push_back(deque_d3) ;
    deque_deque_double.push_back(deque_d4) ;
    deque_deque_double.push_back(deque_d5) ;

    /* SET */
    set_double.insert(1.0) ;
    set_double.insert(2.0) ;
    set_double.insert(3.0) ;

    set_double_compare_alloc.insert(4.0) ;
    set_double_compare_alloc.insert(5.0) ;
    set_double_compare_alloc.insert(6.0) ;

    set_string.insert("I") ;
    set_string.insert("was") ;
    set_string.insert("here") ;

    coi.i1 = 1 ;
    coi.i2 = 2 ;
    coi.i3 = 3 ;
    set_class.insert(coi) ;
    coi.i1 = 4 ;
    coi.i2 = 5 ;
    coi.i3 = 6 ;
    set_class.insert(coi) ;

    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 11 ;
    coi_ptr->i2 = 12 ;
    coi_ptr->i3 = 13 ;
    set_class_ptr.insert(coi_ptr) ;
    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 14 ;
    coi_ptr->i2 = 15 ;
    coi_ptr->i3 = 16 ;
    set_class_ptr.insert(coi_ptr) ;

    Trick::set< double > set_d3( matrix[0], matrix[0] + 3 ) ;
    Trick::set< double > set_d4( matrix[1], matrix[1] + 3 ) ;
    Trick::set< double > set_d5( matrix[2], matrix[2] + 3 ) ;
    set_set_double.insert(set_d3) ;
    set_set_double.insert(set_d4) ;
    set_set_double.insert(set_d5) ;

    /* MULTISET */
    multiset_double.insert(1.0) ;
    multiset_double.insert(2.0) ;
    multiset_double.insert(3.0) ;

    multiset_double_compare_alloc.insert(4.0) ;
    multiset_double_compare_alloc.insert(5.0) ;
    multiset_double_compare_alloc.insert(6.0) ;

    multiset_string.insert("I") ;
    multiset_string.insert("was") ;
    multiset_string.insert("here") ;

    coi.i1 = 1 ;
    coi.i2 = 2 ;
    coi.i3 = 3 ;
    multiset_class.insert(coi) ;
    coi.i1 = 4 ;
    coi.i2 = 5 ;
    coi.i3 = 6 ;
    multiset_class.insert(coi) ;
    // Add an extra copy for multi-set.
    multiset_class.insert(coi) ;

    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 11 ;
    coi_ptr->i2 = 12 ;
    coi_ptr->i3 = 13 ;
    multiset_class_ptr.insert(coi_ptr) ;
    coi_ptr = (ClassOfInts *)TMM_declare_var_1d("ClassOfInts" , 1) ;
    coi_ptr->i1 = 14 ;
    coi_ptr->i2 = 15 ;
    coi_ptr->i3 = 16 ;
    multiset_class_ptr.insert(coi_ptr) ;

    Trick::multiset< double > multiset_d3( matrix[0], matrix[0] + 3 ) ;
    Trick::multiset< double > multiset_d4( matrix[1], matrix[1] + 3 ) ;
    Trick::multiset< double > multiset_d5( matrix[2], matrix[2] + 3 ) ;
    multiset_multiset_double.insert(multiset_d3) ;
    multiset_multiset_double.insert(multiset_d4) ;
    multiset_multiset_double.insert(multiset_d5) ;

    /* MAP */
    map_double_double[11.1] = 111.1 ;
    map_double_double[22.2] = 222.2 ;
    map_double_double[33.3] = 333.3 ;

    map_string_int[std::string("one")] = 1 ;
    map_string_int[std::string("two")] = 2 ;
    map_string_int[std::string("three")] = 3 ;

    map_int_string[4] = std::string("vier") ;
    map_int_string[5] = std::string("fumf") ;
    map_int_string[6] = std::string("sechs") ;

    map_string_string[std::string("mother")] = std::string("Marge") ;
    map_string_string[std::string("father")] = std::string("Homer") ;
    map_string_string[std::string("son")] = std::string("Bart") ;

    map_int_vector_double[80] = vector_d3 ;
    map_int_vector_double[81] = vector_d4 ;
    map_int_vector_double[82] = vector_d5 ;

    map_string_vector_double[std::string("cat")] = vector_d3 ;
    map_string_vector_double[std::string("dog")] = vector_d4 ;
    map_string_vector_double[std::string("duck")] = vector_d5 ;

    Trick::list< int > list_int_i1(3 , 111) ;
    Trick::list< int > list_int_i2(3 , 222) ;

    map_list_int_double[list_int_i1] = 11.1 ;
    map_list_int_double[list_int_i2] = 22.2 ;
    // this line should overwrite the previous
    map_list_int_double[list_int_i2] = 33.3 ;

    map_list_int_string[list_int_i1] = std::string("banana") ;
    map_list_int_string[list_int_i2] = std::string("apple") ;

    map_list_int_vector_double[list_int_i1] = vector_d3 ;
    map_list_int_vector_double[list_int_i2] = vector_d4 ;

    /* MULTIMAP */
    multimap_double_double.insert(std::pair<double, double >(11.1,111.1)) ;
    multimap_double_double.insert(std::pair<double, double >(22.2,222.2)) ;
    multimap_double_double.insert(std::pair<double, double >(33.2,333.3)) ;

    multimap_string_int.insert(std::pair<std::string, int >(std::string("one"),1)) ;
    multimap_string_int.insert(std::pair<std::string, int >(std::string("two"),2)) ;
    multimap_string_int.insert(std::pair<std::string, int >(std::string("three"),3)) ;

    multimap_int_string.insert(std::pair<int, std::string >(4 , std::string("vier"))) ;
    multimap_int_string.insert(std::pair<int, std::string >(5 , std::string("fumf"))) ;
    multimap_int_string.insert(std::pair<int, std::string >(6 , std::string("sechs"))) ;

    multimap_string_string.insert(std::pair<std::string, std::string >(std::string("mother") , std::string("Marge"))) ;
    multimap_string_string.insert(std::pair<std::string, std::string >(std::string("father") , std::string("Homer"))) ;
    multimap_string_string.insert(std::pair<std::string, std::string >(std::string("son") , std::string("Bart"))) ;

    multimap_int_vector_double.insert(std::pair<int, Trick::vector<double > >(80, vector_d3)) ;
    multimap_int_vector_double.insert(std::pair<int, Trick::vector<double > >(81, vector_d4)) ;
    multimap_int_vector_double.insert(std::pair<int, Trick::vector<double > >(82, vector_d5)) ;

    multimap_string_vector_double.insert(std::pair<std::string, Trick::vector<double > >(std::string("cat"), vector_d3)) ;
    multimap_string_vector_double.insert(std::pair<std::string, Trick::vector<double > >(std::string("dog"), vector_d4)) ;
    multimap_string_vector_double.insert(std::pair<std::string, Trick::vector<double > >(std::string("duck"), vector_d5)) ;

    multimap_list_int_double.insert(std::pair< Trick::list<int >, double >(list_int_i1, 11.1)) ;
    multimap_list_int_double.insert(std::pair< Trick::list<int >, double >(list_int_i2, 22.2)) ;
    // this line should not overwrite the previous
    multimap_list_int_double.insert(std::pair< Trick::list<int >, double >(list_int_i2, 33.3)) ;

    multimap_list_int_string.insert(std::pair< Trick::list<int >, std::string >(list_int_i1, std::string("banana"))) ;
    multimap_list_int_string.insert(std::pair< Trick::list<int >, std::string >(list_int_i2, std::string("apple"))) ;

    multimap_list_int_vector_double.insert(std::pair< Trick::list<int >, Trick::vector<double > >(list_int_i1, vector_d3)) ;
    multimap_list_int_vector_double.insert(std::pair< Trick::list<int >, Trick::vector<double > >(list_int_i2, vector_d4)) ;

    stack_uint.push(60) ;
    stack_uint.push(61) ;
    stack_uint.push(62) ;
    stack_uint.push(63) ;

    stack_string.push("abc I") ;
    stack_string.push("abc want the one") ;
    stack_string.push("abc with the bigger") ;
    stack_string.push("abc Gee Bees") ;

    stack_vector_double.push(vector_d3) ;
    stack_vector_double.push(vector_d4) ;
    stack_vector_double.push(vector_d5) ;

    queue_uint.push(70) ;
    queue_uint.push(71) ;
    queue_uint.push(72) ;
    queue_uint.push(73) ;

    queue_string.push("efg I") ;
    queue_string.push("efg want the one") ;
    queue_string.push("efg with the bigger") ;
    queue_string.push("efg Gee Bees") ;

    queue_vector_double.push(vector_d3) ;
    queue_vector_double.push(vector_d4) ;
    queue_vector_double.push(vector_d5) ;

    priority_queue_uint.push(81) ;
    priority_queue_uint.push(80) ;
    priority_queue_uint.push(83) ;
    priority_queue_uint.push(82) ;

    priority_queue_string.push("efg I") ;
    priority_queue_string.push("efg want the one") ;
    priority_queue_string.push("efg with the bigger") ;
    priority_queue_string.push("efg Gee Bees") ;

    priority_queue_vector_double.push(vector_d3) ;
    priority_queue_vector_double.push(vector_d4) ;
    priority_queue_vector_double.push(vector_d5) ;

    pair_int_int = Trick::make_pair( 90 , 91 ) ;
    pair_int_string = Trick::make_pair( 94 , "earth" ) ;
    pair_int_list_int = Trick::make_pair( 95 , list_int_i1 ) ;
    pair_string_int = Trick::make_pair( "mars" , 2 ) ;
    pair_string_string = Trick::make_pair( "blue" , "moon" ) ;
    pair_string_list_int = Trick::make_pair( "tissue" , list_int_i2 ) ;

    pair_list_int_int = Trick::make_pair( list_int_i1 , 96 ) ;
    pair_list_int_string = Trick::make_pair( list_int_i1 , "bag" ) ;
    pair_list_int_list_int = Trick::make_pair( list_int_i1 , list_int_i2 ) ;
}

template< typename T>
void print_sequence(T & container) {
    typename T::iterator lit ;
    for ( lit = container.begin() ; lit != container.end() ; lit++ ) {
        std::cout << (*lit) << " " ;
    }
    std::cout << std::endl ;
}

template< typename T>
void print_class_of_ints(T & container) {
    typename T::iterator lit ;
    unsigned int ii ;
    for ( ii = 0 , lit = container.begin() ; lit != container.end() ; ii++ , lit++ ) {
        std::cout << "ii = " << ii << ": " << (*lit).i1 << " " << (*lit).i2 ;
        std::cout << " " << (*lit).i3 << std::endl ;
    }
}

template< typename T>
void print_2d_container(T & container) {
    typename T::iterator lldit ;
    for ( lldit = container.begin() ; lldit != container.end() ; lldit++ ) {
        std::cout << "size = " << (*lldit).size() << std::endl ;
        typename T::value_type::iterator ldit ;
        for ( ldit = (*lldit).begin() ; ldit != (*lldit).end() ; ldit++ ) {
            std::cout << (*ldit) << " " ;
        }
        std::cout << std::endl ;
    }
}

template< typename T>
void print_map(T & container) {
    typename T::iterator lit ;
    unsigned int ii ;
    for ( ii = 0 , lit = container.begin() ; lit != container.end() ; ii++ , lit++ ) {
        std::cout << "ii = " << ii << ": " << (*lit).first << " " << (*lit).second << std::endl ;
    }
}

template< typename T>
void print_map_keys(T & container) {
    typename T::iterator lit ;
    unsigned int ii ;
    for ( ii = 0 , lit = container.begin() ; lit != container.end() ; ii++ , lit++ ) {
        std::cout << "ii = " << ii << ": " << (*lit).first << std::endl ;
    }
}

template< typename T>
void print_map_data(T & container) {
    typename T::iterator lit ;
    unsigned int ii ;
    for ( ii = 0 , lit = container.begin() ; lit != container.end() ; ii++ , lit++ ) {
        std::cout << "ii = " << ii << ": " << (*lit).second << std::endl ;
    }
}

int TrickSTLCheckpoint::speak() {

    std::cout << "VECTOR" << std::endl ;
    print_class_of_ints< Trick::vector< ClassOfInts > >( vector_class ) ;
    print_2d_container< Trick::vector< Trick::vector< double > > >( vector_vector_double ) ;
    std::cout << std::endl ;

    std::cout << "LIST" << std::endl ;
    print_class_of_ints< Trick::list< ClassOfInts > >( list_class ) ;
    print_2d_container< Trick::list< Trick::list< double > > >( list_list_double ) ;
    std::cout << std::endl ;

    std::cout << "DEQUE" << std::endl ;
    print_class_of_ints< Trick::deque< ClassOfInts > >( deque_class ) ;
    print_2d_container< Trick::deque< Trick::deque< double > > >( deque_deque_double ) ;
    std::cout << std::endl ;

    std::cout << "SET" << std::endl ;
    print_class_of_ints< Trick::set< ClassOfInts > >( set_class ) ;
    print_2d_container< Trick::set< Trick::set< double > > >( set_set_double ) ;
    std::cout << std::endl ;

    std::cout << "MULTISET" << std::endl ;
    print_class_of_ints< Trick::multiset< ClassOfInts > >( multiset_class ) ;
    print_2d_container< Trick::multiset< Trick::multiset< double > > >( multiset_multiset_double ) ;
    std::cout << std::endl ;

    std::cout << "MAP" << std::endl ;
    print_map< Trick::map< double,double > >( map_double_double ) ;
    print_map< Trick::map< std::string,int > >( map_string_int ) ;
    print_map< Trick::map< int,std::string > >( map_int_string ) ;
    print_map< Trick::map< std::string,std::string > >( map_string_string ) ;
    print_map_keys< Trick::map< int , Trick::vector< double > > >( map_int_vector_double ) ;
    print_map_keys< Trick::map< std::string , Trick::vector< double > > >( map_string_vector_double ) ;
    print_map_data< Trick::map< Trick::list< int > , double > >( map_list_int_double ) ;
    print_map_data< Trick::map< Trick::list< int > , std::string > >( map_list_int_string ) ;
    std::cout << std::endl ;

    std::cout << "MULTIMAP" << std::endl ;
    print_map< Trick::multimap< double,double > >( multimap_double_double ) ;
    print_map< Trick::multimap< std::string,int > >( multimap_string_int ) ;
    print_map< Trick::multimap< int,std::string > >( multimap_int_string ) ;
    print_map< Trick::multimap< std::string,std::string > >( multimap_string_string ) ;
    print_map_keys< Trick::multimap< int , Trick::vector< double > > >( multimap_int_vector_double ) ;
    print_map_keys< Trick::multimap< std::string , Trick::vector< double > > >( multimap_string_vector_double ) ;
    print_map_data< Trick::multimap< Trick::list< int > , double > >( multimap_list_int_double ) ;
    print_map_data< Trick::multimap< Trick::list< int > , std::string > >( multimap_list_int_string ) ;
    std::cout << std::endl ;

    std::cout << "STACK" << std::endl ;
    std::cout << "top " << stack_uint.top() << std::endl ;
    std::cout << "top " << stack_string.top() << std::endl ;
    print_sequence< Trick::vector< double > >( stack_vector_double.top() ) ;
    std::cout << std::endl ;

    std::cout << "QUEUE" << std::endl ;
    std::cout << "front " << queue_uint.front() << std::endl ;
    std::cout << "front " << queue_string.front() << std::endl ;
    print_sequence< Trick::vector< double > >( queue_vector_double.front() ) ;
    std::cout << std::endl ;

    std::cout << "PRIORITY QUEUE" << std::endl ;
    std::cout << "top " << priority_queue_uint.top() << std::endl ;
    std::cout << "top " << priority_queue_string.top() << std::endl ;
    //print_sequence< Trick::vector< double > >( priority_queue_vector_double.top() ) ;
    std::cout << std::endl ;

    std::cout << "PAIR" << std::endl ;
    std::cout << "pair_int_int " << pair_int_int.first << " " << pair_int_int.second << std::endl ;
    std::cout << "pair_fill_int_int " << pair_fill_int_int.first << " " << pair_fill_int_int.second << std::endl ;
    std::cout << "pair_int_string " << pair_int_string.first << " " << pair_int_string.second << std::endl ;
    std::cout << "pair_int_list_int " << pair_int_list_int.first << std::endl ;
    print_sequence< Trick::list< int > >(pair_int_list_int.second) ;
    std::cout << "pair_string_int " << pair_string_int.first << " " << pair_string_int.second << std::endl ;
    std::cout << "pair_string_string " << pair_string_string.first << " " << pair_string_string.second << std::endl ;
    std::cout << "pair_string_list_int " << pair_string_list_int.first << std::endl ;
    print_sequence< Trick::list< int > >(pair_string_list_int.second) ;
    std::cout << "pair_list_int_int " << std::endl ;
    print_sequence< Trick::list< int > >(pair_list_int_int.first) ;
    std::cout << pair_list_int_int.second << std::endl ;
    std::cout << "pair_list_int_string " << std::endl ;
    print_sequence< Trick::list< int > >(pair_list_int_string.first) ;
    std::cout << pair_list_int_string.second << std::endl ;
    std::cout << "pair_list_int_list_int " << std::endl ;
    print_sequence< Trick::list< int > >(pair_list_int_list_int.first) ;
    print_sequence< Trick::list< int > >(pair_list_int_list_int.second) ;
    std::cout << std::endl ;

    return 0 ;
}

