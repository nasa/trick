/**
@file

@verbatim
PURPOSE: (SignalA directly defines creal64_T - simulates a Simulink-generated header
          where the same typedef appears verbatim in multiple generated files.)
@endverbatim
*******************************************************************************/

#ifndef SIGNALA_HH
#define SIGNALA_HH

#ifndef CREAL64_T_DEFINED
#define CREAL64_T_DEFINED
typedef double real64_T;

typedef struct
{
        real64_T re;
        real64_T im;
} creal64_T;
#endif

class SignalA
{
    public:
        SignalA()
            : value()
        {
        }
        creal64_T value;
        creal64_T* value_a_array;
};

#endif
