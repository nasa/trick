/*
PURPOSE: (Units Conversion Package C++ Language Interface)
 
PROGRAMMERS:
    (((John M. Penn) (L-3Com/Titan) (May 2006) (v1.0) 
      (Initial Release))) 
*/

/*
 * $Log: Unit.hh,v $
 * Revision 1.5  2006-05-09 14:31:48-05  penn
 * In the new Units package, change getUnitname to getUnitName
 *
 * Revision 1.4  2006-05-09 10:09:47-05  lin
 * gcc 4.1 doesn't like trick_source/data_products/Log/log.h
 *
 * Revision 1.3  2006-05-03 16:15:15-05  penn
 * Add a setUnitName method to the new units conversion class
 *
 * Revision 1.2  2006-05-02 18:21:56-05  penn
 * Add a get_name method to the Unit class
 *
 * Revision 1.1  2006-05-02 13:00:06-05  penn
 * Initial revision
 *
 *
 */
#ifndef UNIT_HH
#define UNIT_HH
#include "units_conv.h"
#include "UCFn.hh"

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
