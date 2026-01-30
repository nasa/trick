/********************************* TRICK HEADER *******************************
PURPOSE:
    ()
LIBRARY DEPENDENCY:
    ()
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#ifndef FOO_HH_
#define FOO_HH_

#include "FooB.hh"
#include "FooD.hh"

namespace FooNamespace
{

// RestartableScalarFirstOrderODEIntegrator
class FooContB
{
public:
    FooContB()
        : d(12.0)
    {
    }

    // RestartableFirstOrderODEIntegrator
    FooB<2> fooB;

    FooB<2> fooB2;

    double d;
};

// RestartableT3SecondOrderODEIntegrator
class FooContD
{
public:
    FooContD()
        : d(12.0)
    {
    }

    // RestartableSimpleSecondOrderODEIntegrator
#ifndef TESTING_SWIG
#ifndef SWIG
    // THIS SHOULD PREVENT SWIG FROM MAKING ANY TEMPLATE REFERENCES TO EXCLUDED FooD TYPE
    FooD<1> fooD;

    FooD<2> fooD2;
#endif
#endif

    int spacer;

#if !defined(SWIG)
    // THIS SHOULD PREVENT SWIG FROM MAKING ANY TEMPLATE REFERENCES TO EXCLUDED FooD TYPE
    FooD<3> fooD3;

    FooD<4> fooD4;
#endif

    double d;
};

} // namespace FooNamespace

#endif
