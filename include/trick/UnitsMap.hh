/*
    PURPOSE:
        (UnitsMap)
*/

#ifndef UNITSMAP_HH
#define UNITSMAP_HH

#include <map>
#include <string>

namespace Trick {

/**
 * This map stores all the variables and their units and
 * provides a convenient way of getting variables units.
 */

    class UnitsMap {

        public:
            /**
             * Returns a pointer to the singleton Trick::UnitsMap instance.
             * @return    A pointer to Trick::UnitsMap.
             */
            static Trick::UnitsMap * units_map() ;

            /**
             * Constructor.
             */
            UnitsMap() {} ;

            /**
             * Destructor.
             */
            ~UnitsMap() ;

            /**
             * Adds a variable with specified units to the map.
             * @param param    The name of the variable.
             * @param units    The units of the variable.
             */
            int add_param( std::string param , const char * units ) ;

            /**
             * Gets the units for a specified variable.
             * @param param    The name of the variable.
             * @return    The units of a specified variable.
             */
            std::string get_units( std::string param ) ;

        private:
            std::map<std::string, char * > param_units ;
            static Trick::UnitsMap* pInstance ;

    } ;

}

#endif

