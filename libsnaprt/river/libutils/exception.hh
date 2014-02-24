#ifndef EXCEPTION_HH
#define EXCEPTION_HH

#include <stdexcept>
#include <iostream>
#include <sstream>

/** \ingroup libtc
 * Basic exception class, specialized from std::runtime_error to standardize
 * exception types in simdata.
 */
class Exception: public std::runtime_error  // I use runtime_error a lot
{                                   // its derived from std::exception
    public:                         // And has an implementation of what()
        Exception(std::string const& msg):
            runtime_error(msg)
        {}
        Exception(std::stringstream const& strm):
            runtime_error(strm.str())
        {}
};

#endif
