/**
@file

@verbatim
PURPOSE: (SignalB directly defines creal64_T - simulates a Simulink-generated header
          where the same typedef appears verbatim in multiple generated files.)
@endverbatim
*******************************************************************************/

#ifndef SIGNALB_HH
#define SIGNALB_HH

#ifndef CREAL64_T_DEFINED
#define CREAL64_T_DEFINED
typedef double real64_T;

typedef struct
{
        real64_T re;
        real64_T im;
} creal64_T;
#endif

class SignalB
{
    public:
        SignalB()
            : value()
        {
        }
        creal64_T value;
        creal64_T* value_b_array;
};

#endif
