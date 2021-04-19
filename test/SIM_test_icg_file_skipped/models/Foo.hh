// @trick_parse{everything}
#ifndef FOO_HH
#define FOO_HH

class Foo {
public:
    #ifndef TRICK_ICG
    int i;
    int j;
    int k;
    Foo() : i(1), j(2), k(3) {}
    #endif
};

#endif
