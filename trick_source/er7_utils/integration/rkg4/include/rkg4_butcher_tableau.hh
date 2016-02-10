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


#ifndef ER7_UTILS_RKG4_BUTCHER_TABLEAU_HH
#define ER7_UTILS_RKG4_BUTCHER_TABLEAU_HH


namespace er7_utils {

/**
 * Contains the Runge Kutta Gill 4 Butcher Tableau.
 * This is a class rather than a namespace because of swig issues.
 */
class RKGill4ButcherTableau {

public:

   static const double RKa[4][4]; /**< trick_units(--) @n
    * Runge Kutta Gill 4 Butcher tableau 'a' elements. */

   static const double RKb[4]; /**< trick_units(--) @n
    * Runge Kutta Gill 4 Butcher tableau 'b' elements. */

   static const double RKc[4]; /**< trick_units(--) @n
      Runge Kutta Gill 4 Butcher tableau 'c' elements. */


private:

   /**
    * Not implemented.
    */
   RKGill4ButcherTableau ();

   /**
    * Not implemented.
    */
   ~RKGill4ButcherTableau ();

   /**
    * Not implemented.
    */
   RKGill4ButcherTableau (const RKGill4ButcherTableau &);

   /**
    * Not implemented.
    */
   RKGill4ButcherTableau & operator= (const RKGill4ButcherTableau &);
};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
