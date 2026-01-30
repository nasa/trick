/**
@file

@verbatim
PURPOSE: (Test that an empty PYTHON_MODULE won't cause errors or have any effect.)
PYTHON_MODULE: ()
@endverbatim
*******************************************************************************/

#ifndef TRICKFOOD_HH
#define TRICKFOOD_HH

#include <iostream>

enum Fast
{
    Pizza,
    Burger,
    Taco
};

class Food
{
public:
    Food()
        : fast(Taco)
    {
    }

    void print_me()
    {
        std::cout << "Food::print_me!" << std::endl;
    }

    Fast fast;
};

#endif
