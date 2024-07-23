/********************************* TRICK HEADER *******************************
PURPOSE:
    ()
LIBRARY DEPENDENCY:
    ()
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#ifndef FOOD_HH_
#define FOOD_HH_

#include "FooB.hh"

namespace FooNamespace
{

// RestartableSecondOrderODEIntegrator
class FooC : public FooA<1>
{
public:
   int C;
};

// RestartableSimpleSecondOrderODEIntegrator
template<int T>
class FooD : public FooC
{
public:
   int D = T;
};


}

#endif
