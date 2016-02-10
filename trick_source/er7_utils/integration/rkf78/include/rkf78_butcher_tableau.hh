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
 * Defines Butcher tableau for Runge Kutta Fehlberg 7/8.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RKF78_BUTCHER_TABLEAU_HH
#define ER7_UTILS_RKF78_BUTCHER_TABLEAU_HH


namespace er7_utils {

/**
 * Contains the Runge Kutta Fehlberg 7/8 Butcher Tableau.
 * This is a class rather than a namespace because of swig issues.
 */
class RKFehlberg78ButcherTableau {

public:

   static const double RKa[13][13]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 7/8 Butcher tableau 'a' elements. */

   static const double RKb8[13]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 7/8 Butcher tableau eighth order 'b' elements. */

   static const double RKb7[13]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 7/8 Butcher tableau seventh order 'b' elements. */

   static const double RKc[13]; /**< trick_units(--) @n
      Runge Kutta Fehlberg 7/8 Butcher tableau 'c' elements. */


private:

   /**
    * Not implemented.
    */
   RKFehlberg78ButcherTableau ();

   /**
    * Not implemented.
    */
   ~RKFehlberg78ButcherTableau ();

   /**
    * Not implemented.
    */
   RKFehlberg78ButcherTableau (const RKFehlberg78ButcherTableau &);

   /**
    * Not implemented.
    */
   RKFehlberg78ButcherTableau & operator= (
      const RKFehlberg78ButcherTableau &);
};


/**
 * Contains the Runge Kutta Fehlberg 7/8 Butcher Tableau,
 * but with step 10 skipped.
 * This is a class rather than a namespace because of swig issues.
 */
class RKFehlberg78AbbreviatedButcherTableau {

public:

   static const double RKa[13][13]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 7/8 Butcher tableau 'a' elements. */

   static const double RKb8[13]; /**< trick_units(--) @n
    * Runge Kutta Fehlberg 7/8 Butcher tableau eighth order 'b' elements. */

   static const double RKc[13]; /**< trick_units(--) @n
      Runge Kutta Fehlberg 7/8 Butcher tableau 'c' elements. */


private:

   /**
    * Not implemented.
    */
   RKFehlberg78AbbreviatedButcherTableau ();

   /**
    * Not implemented.
    */
   ~RKFehlberg78AbbreviatedButcherTableau ();

   /**
    * Not implemented.
    */
   RKFehlberg78AbbreviatedButcherTableau (
      const RKFehlberg78AbbreviatedButcherTableau &);

   /**
    * Not implemented.
    */
   RKFehlberg78AbbreviatedButcherTableau & operator= (
      const RKFehlberg78AbbreviatedButcherTableau &);
};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
