/**
@file

@verbatim
PURPOSE:
    (Template tests)
@endverbatim
*******************************************************************************/

#ifndef TEMPLATETEST_HH
#define TEMPLATETEST_HH

template<class A, class B> class TTT_test
{
public:
    TTT_test()
    {
        aa = 0;
        bb = 0;
        cc = NULL;
        dd = NULL;
    }

    A aa;
    B bb;
    TTT_test<A, B> * ttt;

    typedef TTT_test<A, B> C;
    C * cc;
    typedef TTT_test<B, A> D;
    D * dd;
};

class TemplateTest
{
    friend class InputProcessor;
    friend void init_attrTemplateTest();

public:
    TTT_test<int, double> TTT_var;
};

#ifdef SWIG
%struct_str(TemplateTest)
#endif

// Verify we can build templates/intsantiations defined in different combinations of namespaces
template<class T> struct TemplateNoNS
{
    T x;
};

namespace NS1
{
template<class T> struct TemplateNS
{
    T y;
};
} // namespace NS1

class ClassNoNS
{
public:
    TemplateNoNS<int> tnns;
    NS1::TemplateNS<int> tns;
};

namespace NS2
{
class ClassNS
{
public:
    TemplateNoNS<int> tnns;
    NS1::TemplateNS<int> tns;
};
} // namespace NS2

// Verify we can build templates/intsantiations defined in the same namespace
namespace a
{

template<class T> struct Bar
{
    T z;
};

class Foo
{
public:
    Bar<int> bar;
};

} // namespace a

// Verify we can build templates/intsantiations defined in different namespaces
namespace b
{

class Foo2
{
public:
    a::Bar<int> bar;
};

} // namespace b

// Verify we can build with templated functions (isn't actually SWIG-ified, but should be ignored)
template<typename T> void templated_function() {}

#endif /* _BALL_HH_ */
