/**
@file

@verbatim
PURPOSE:
    (Template tests)
@endverbatim
*******************************************************************************/

#ifndef FOO_HH
#define FOO_HH

class MyEnumClass {
    public:
    enum class Enum {};
    class Inner {};
};

template <class T>
class Foo {
    public:
        T t; /* (--) */
};

enum Bar {
    Bar_1,
    Bar_2
};

#endif /* FOO_HH */
