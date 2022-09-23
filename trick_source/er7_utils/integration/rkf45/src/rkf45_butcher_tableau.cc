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
 * Defines Butcher tableau for Runge Kutta Fehlberg 4/5.
 */

/*
Purpose: ()
*/


// Model includes
#include "../include/rkf45_butcher_tableau.hh"


namespace er7_utils {

// Runge Kutta Fehlberg 4/5 Butcher tableau 'a' elements
const double RKFehlberg45ButcherTableau::RKa[6][6] = {

   // a[0] = all zeros, and is not used.
   {0.0, 0.0, 0.0, 0.0, 0.0, 0.0},

   // a[1]: 1 element, sum = 1.0/4.0
   {1.0/4.0,
    0.0, 0.0, 0.0, 0.0, 0.0},

   // a[2]: 2 elements, sum = 3.0/8.0
   {3.0/32.0, 9.0/32.0,
    0.0, 0.0, 0.0, 0.0},

   // a[3]: 3 elements, sum = 12.0/13.0
   {1932.0/2197.0, -7200.0/2197.0, 7296.0/2197.0,
    0.0, 0.0, 0.0},

   // a[4]: 4 elements, sum = 1.0
   {439.0/216.0, -8.0, 3680.0/513.0, -845.0/4104.0,
    0.0, 0.0},

   // a[5]: 5 elements, sum = 0.5
   {-8.0/27.0, 2.0, -3544.0/2565.0, 1859.0/4104.0, -11.0/40.0,
    0.0}
};


// Runge Kutta Fehlberg 4/5 Butcher tableau fifth order 'b' elements
const double RKFehlberg45ButcherTableau::RKb5[6] = {
   16.0/135.0, 0.0, 6656.0/12825.0, 28561.0/56430.0, -9.0/50.0, 2.0/55.0
};


// Runge Kutta Fehlberg 4/5 Butcher tableau fourth order 'b' elements
const double RKFehlberg45ButcherTableau::RKb4[6] = {
   25.0/216.0, 0.0, 1408.0/2565.0, 2197.0/4104.0, -1.0/5.0, 0.0
};


// Runge Kutta Fehlberg 7/8 Butcher tableau 'c' elements
const double RKFehlberg45ButcherTableau::RKc[6] = {
   0.0, 1.0/4.0, 3.0/8.0, 12.0/13.0, 1.0, 1.0/2.0
};


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
