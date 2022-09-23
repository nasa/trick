/*
PURPOSE: (Units Conversion Package C++ Language Interface)

PROGRAMMERS:
    (((John M. Penn) (L-3Com/Titan) (May 2006) (v1.0)
      (Initial Release)))
*/

#ifndef UNIT_HH
#define UNIT_HH
#include "trick/units_conv.h"
#include "trick/UCFn.hh"

/**
 * Unit (of measurement) class.
 */
class Unit {

 public:
#ifndef SWIG
    class CONVERSION_ERROR {}; // Exception Class
#endif
    /**
     *
     */
    Unit();                                                 // Throws CONVERSION_ERROR
    Unit(const char *name);                                 // Throws CONVERSION_ERROR
    ~Unit();

    /**
     Initialize the Unit object from the given units specifier.
     Throw an exception if the specifier is invalid.
     */
    std::string setUnitName(const char *units_spec);             // Throws CONVERSION_ERROR

    /**
     Return the units specifier of this Unit object.
     */
    const char *getUnitName() { return this->units_name.c_str(); }

    /**
     Return a base and offset that converts these units to the units
     specified by the given units specifier.  Throw an exception if its not possible.
     */
    UCFn *Conversion_to( const char *units_spec);           // Throws CONVERSION_ERROR

    /**
     Return a base and offset that converts these units to the units
     specified by the given Unit object. Throw an exception if its not possible.
     */
    UCFn *Conversion_to( const Unit *u);                    // Throws CONVERSION_ERROR

    /**
     Convert the value from these units to those specified by the given units specifier.
     */
    double Convert_to( double val, const char *unit_spec);  // Throws CONVERSION_ERROR

    /**
     Convert the value from these units to those specified by the Unit parameter.
     */
    double Convert_to( double val, const Unit *u);          // Throws CONVERSION_ERROR

#ifndef SWIG
    friend std::ostream& operator<< ( std::ostream& s, const Unit *u);
#endif

 private:

    std::string  units_name;
    Units_t *units;

};
#endif
