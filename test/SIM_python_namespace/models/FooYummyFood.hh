/**
@file

@verbatim
PURPOSE: (Namespace Test)
PYTHON_MODULE: (Foo)
@endverbatim
*******************************************************************************/

#ifndef FOOYUMMYFOOD_HH
#define FOOYUMMYFOOD_HH

#include <iostream>

namespace Foo {

enum Yummy {
    Butter,
    Bacon,
    Doughnuts
};

class YummyFood {
  public:
    YummyFood() : yummy(Bacon) {}
    void print_me() { std::cout << "Foo::YummyFood::print_me!" << std::endl; }
    Yummy yummy;
};

}

#endif /* _BALL_HH_ */

