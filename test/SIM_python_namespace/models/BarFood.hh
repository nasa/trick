/**
@file

@verbatim
PURPOSE: (Namespace Test)
PYTHON_MODULE: (Bar)
@endverbatim
*******************************************************************************/

#ifndef BARFOOD_HH
#define BARFOOD_HH

#include <iostream>

namespace Bar {

enum Fast {
    Pizza,
    Burger,
    Taco
};

class Food {
  public:
    Food() : fast(Taco) {}
    void print_me() { std::cout << "Bar::Food::print_me!" << std::endl; }
    Fast fast;
};

}

#endif /* _BALL_HH_ */

