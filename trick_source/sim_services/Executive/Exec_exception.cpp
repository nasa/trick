
#include <iostream>
#include <sstream>

#include "trick/Exec_exception.hh"

Trick::Exec_exception::Exec_exception(int in_ret , std::string in_file , int line , std::string in_message ) {

    std::ostringstream o ;

    ret_code = in_ret ;
    o << in_file << ":" << line ;
    file = o.str() ;
    message = in_message ;

}

Trick::Exec_exception::~Exec_exception() throw () {
}

const char* Trick::Exec_exception::what() const throw() {
    return message.c_str() ;
}

