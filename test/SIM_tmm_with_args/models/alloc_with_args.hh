/********************************* TRICK HEADER *******************************
PURPOSE: ( Test tmm_alloc_args in a sim environment )
LIBRARY DEPENDENCY:
((alloc_with_args.o))
*******************************************************************************/

#include <iostream>

namespace Test {
    struct SomeStruct
    {
        SomeStruct(double new_val)
        {
            val = new_val;
        }
        double val = 5.0;
    };
}

class AllocTestWithArguments {

    friend class InputProcessor;
    
    public:

    AllocTestWithArguments()
     :
    some_int(0),
    some_double(0.0)
    {

    }

    AllocTestWithArguments(int in_int, double in_double) 
    :
    some_int(in_int),
    some_double(in_double)
    {
        std::cout << "AllocTestWithArguments constructor with: \n";
        std::cout << "in_int: " << in_int << "\n";
        std::cout << "in_double: " << in_double << "\n";
    }


    AllocTestWithArguments(int* in_int, double *in_double, std::string &a_name) 
    :
    some_int(*in_int),
    some_double(*in_double)
    {
        std::cout << "AllocTestWithArguments constructor with: \n";
        std::cout << a_name << std::endl;
        std::cout << "in_int: " << in_int << "\n";
        std::cout << "in_double: " << in_double << "\n";
    }

    AllocTestWithArguments(int*, double&, std::string);

    ~AllocTestWithArguments() {
        std::cout << "AllocTestWithArguments desctruct.\n";
    }

    int some_int;
    double some_double;
};