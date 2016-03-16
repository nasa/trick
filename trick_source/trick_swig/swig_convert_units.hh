
#ifndef SWIG_CONERT_UNITS_HH
#define SWIG_CONERT_UNITS_HH
#include <string>

int convert_united_value( std::string & to_units , std::string & from_units , long long * val ) ;
int convert_united_value( std::string & to_units , std::string & from_units , double * val ) ;

int scale_united_value( std::string & to_units , std::string & from_units , long long * val ) ;
int scale_united_value( std::string & to_units , std::string & from_units , double * val ) ;
#endif
