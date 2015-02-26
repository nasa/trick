/* 
    PURPOSE: 
        (Trick runtime simulation executive parameter definition.) 
    REFERENCE: 
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment) 
          (NASA:JSC #37943) 
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--))) 
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.)) 
    PROGRAMMERS: 
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime)) 
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

/* 
 * $Id: UnitsMap.hh 2073 2011-12-09 21:05:52Z lin $
 */

/**
 * @file UnitsMap.hh
 * Provides a map of variables and their units.
 */

#ifndef _UNITSMAP_HH_
#define _UNITSMAP_HH_

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

