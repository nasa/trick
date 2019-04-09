/**
@file

@verbatim
PURPOSE: (Namespace Test)
PYTHON_MODULE: (Foo.Inner)
@endverbatim
*******************************************************************************/

#ifndef FOOINNERFOOD_HH
#define FOOINNERFOOD_HH

namespace Foo {
  namespace Inner {
    enum Fast {
        Burger,
        Taco,
        Pizza
    };

    class Food {
      public:
        Food() : fast(Taco) {}
        void print_me() { std::cout << "Foo::Inner::Food::print_me!" << std::endl; }
        Fast fast;
    };
  }
}

#endif /* _BALL_HH_ */

