/********************************* TRICK HEADER *******************************
PURPOSE:
    ()
LIBRARY DEPENDENCY:
    ()
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#ifndef FOOB_HH_
#define FOOB_HH_

namespace FooNamespace
{

// RestartableStateIntegrator
template<int T> class FooA
{
public:
    int A = T;
};

// RestartableFirstOrderODEIntegrator
template<int T> class FooB : public FooA<3>
{
public:
    int B = T;
};

} // namespace FooNamespace

#endif
