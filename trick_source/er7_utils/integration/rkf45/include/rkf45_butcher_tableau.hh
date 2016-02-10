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


#ifndef ER7_UTILS_RKF45_BUTCHER_TABLEAU_HH
#define ER7_UTILS_RKF45_BUTCHER_TABLEAU_HH


namespace er7_utils {

/**
 * Contains the Runge Kutta Fehlberg 4/5 Butcher Tableau.
 * This is a class rather than a namespace because of swig issues.
 */
class RKFehlberg45ButcherTableau {

public:

   static const double RKa[6][6]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 4/5 Butcher tableau 'a' elements. */

   static const double RKb5[6]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 4/5 Butcher tableau fifth order 'b' elements. */

   static const double RKb4[6]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 4/5 Butcher tableau fourth order 'b' elements. */

   static const double RKc[6]; /**< trick_units(--) @n
      Runge Kutta Fehlberg 4/5 Butcher tableau 'c' elements. */


private:

   /**
    * Not implemented.
    */
   RKFehlberg45ButcherTableau ();

   /**
    * Not implemented.
    */
   ~RKFehlberg45ButcherTableau ();

   /**
    * Not implemented.
    */
   RKFehlberg45ButcherTableau (const RKFehlberg45ButcherTableau &);

   /**
    * Not implemented.
    */
   RKFehlberg45ButcherTableau & operator= (
      const RKFehlberg45ButcherTableau &);
};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
