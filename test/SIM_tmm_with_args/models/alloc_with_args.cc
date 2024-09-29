#include "alloc_with_args.hh"


AllocTestWithArguments::AllocTestWithArguments()
:
some_int(0),
some_double(0.0)
{

}


AllocTestWithArguments::AllocTestWithArguments(int* in_int, double& in_double, std::string in_string)
:
some_int(*in_int),
some_double(in_double)
{
    std::cout << in_string << "\nn";
    
}   

AllocTestWithArguments::AllocTestWithArguments(int* in_int, double *in_double, std::string &a_name)
:
some_int(*in_int),
some_double(*in_double)
{
    std::cout << "AllocTestWithArguments constructor with: \n";
    std::cout << a_name << std::endl;
    std::cout << "in_int: " << in_int << "\n";
    std::cout << "in_double: " << in_double << "\n";
}

AllocTestWithArguments::AllocTestWithArguments(int in_int, double in_double) 
:
some_int(in_int),
some_double(in_double)
{
    std::cout << "AllocTestWithArguments constructor with: \n";
    std::cout << "in_int: " << in_int << "\n";
    std::cout << "in_double: "
     << in_double << "\n";
}


AllocTestWithArguments::AllocTestWithArguments(Test::SomeStruct in_struct)
{

}