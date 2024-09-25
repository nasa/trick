#include "alloc_with_args.hh"

AllocTestWithArguments::AllocTestWithArguments(int* in_int, double& in_double, std::string in_string)
:
some_int(*in_int),
some_double(in_double)
{
    std::cout << in_string << "\nn";
    
}   