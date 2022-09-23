/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Integration
 * @{
 * @endif
 */

/**
 * @file
 * Defines Butcher tableau for Runge Kutta Gill 4.
 */

/*
Purpose: ()
*/


// Model includes
#include "../include/rkg4_butcher_tableau.hh"


namespace {

   /**
    * sqrt(2)/2
    */
   const double sqrt2d2 = 0.5 * 1.41421356237309504880168872420969808;
}


namespace er7_utils {

/**
 * Runge Kutta Gill 4 Butcher tableau 'a' elements
 */
const double RKGill4ButcherTableau::RKa[4][4] = {

   // a[0] = all zeros, and is not used.
   {0.0, 0.0, 0.0, 0.0},

   // a[1]: 1 element, sum = 1/2
   {0.5, 0.0, 0.0, 0.0},

   // a[2]: 2 elements, sum = 1/2
   {sqrt2d2 - 0.5, 1.0 - sqrt2d2, 0.0, 0.0},

   // a[3]: 3 elements, sum = 1
   {0.0, -sqrt2d2, 1.0 + sqrt2d2, 0.0}
};


/**
 * Runge Kutta Gill 4 Butcher tableau 'b' elements
 */
const double RKGill4ButcherTableau::RKb[4] = {
   1.0/6.0, (1.0 - sqrt2d2)/3.0, (1.0 + sqrt2d2)/3.0, 1.0/6.0
};


/**
 * Runge Kutta Gill 4 Butcher tableau 'c' elements
 */
const double RKGill4ButcherTableau::RKc[4] = {
   0.0, 0.5, 0.5, 1.0};

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
