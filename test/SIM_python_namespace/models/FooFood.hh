/**
@file

@verbatim
PURPOSE: (Namespace Test)
PYTHON_MODULE: (Foo)
@endverbatim
*******************************************************************************/

#ifndef FOOFOOD_HH
#define FOOFOOD_HH

#include <iostream>

namespace Foo {

enum Fast {
    Taco,
    Pizza,
    Burger
};

class Food {
  public:
    Food() : fast(Taco) {}
    void print_me() { std::cout << "Foo::Food::print_me!" << std::endl; }
    Fast fast;
};

}

#endif /* _BALL_HH_ */

