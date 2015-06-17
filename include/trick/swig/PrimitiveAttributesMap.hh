/*
    PURPOSE:
        (Map between type name and attributes structure)
    REFERENCE:
          ()
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.))
    PROGRAMMERS:
        (((Alex Lin) (NASA) (1/12) (--) (--)))
*/

#ifndef PRIMITIVEATTRIBUTESMAP_HH
#define PRIMITIVEATTRIBUTESMAP_HH

#include <map>
#include <string>

#include "trick/attributes.h"

namespace Trick {

/**
 * This map stores starting attributes structures keyed on the
 * type name
 */

    class PrimitiveAttributesMap {

        public:
            PrimitiveAttributesMap() ;
            ~PrimitiveAttributesMap() ;

            /**
             * Returns a pointer to the singleton Trick::UnitsMap instance.
             * @return    A pointer to Trick::UnitsMap.
             */
            static Trick::PrimitiveAttributesMap * attributes_map() ;

            /**
             * Gets the units for a specified variable.
             * @param param    The name of the variable.
             * @return    The units of a specified variable.
             */
            ATTRIBUTES * get_attr( std::string param ) ;

        private:
            std::map<std::string, ATTRIBUTES * > param_attr ;

            /**
             * Adds a variable with specified units to the map.
             * @param param    The name of the variable.
             * @param units    The units of the variable.
             */
            int add_type( std::string param , TRICK_TYPE type , int size ) ;

    } ;

}

#endif

