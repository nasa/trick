#ifndef INTERPOLATOR_HH
#define INTERPOLATOR_HH

#include <stdexcept>

namespace Trick {

    class Interpolator {

    public:

    Interpolator (double* Table, double** BreakPointArrays, unsigned int* BreakPointArraySizes, unsigned int NParams)
    : table(Table),
      breakPointArrays(BreakPointArrays),
      breakPointArraySizes(BreakPointArraySizes),
      nParams(NParams) {};

    double eval (double param1, ...) ;
    double eval (double params[]) ;

    private:

    Interpolator(){};
    double eval (double param[], unsigned int param_index, unsigned int offset) ;

    // DATA MEMBERS
    double*  table;                  /**< Interpolation data. */
    double** breakPointArrays;       /**< Array (of length n [below]]) of pointers to the breakpoint arrays.*/
    unsigned int*     breakPointArraySizes;   /**< Array that specifies the size of each breakpoint array.*/
    unsigned int      nParams;                /**< Number of independent variables. Same as number of breakpoint arrays. */

    };

} // endof namespace Trick

#endif
