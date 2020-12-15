/*
    PURPOSE:
        (Sie creator)
*/

#ifndef SIE_HH
#define SIE_HH

#include <string>
#include <fstream>

#include "trick/AttributesMap.hh"
#include "trick/EnumAttributesMap.hh"

namespace Trick {

    /**
     *
     * This class wraps the MemoryManager class for use in Trick simulations
     * @author Alexander S. Lin
     *
     */
    class Sie {

        public:

            Sie() ;

            /**
             * Currently process_sim_args is an empty function
             * @return always 0
             */
            int process_sim_args() ;

            /**
             * Writes the S_sie.resource file using MemoryManager information
             * @return always 0
             */
            void sie_print_xml() ;
            void class_attr_map_print_xml() ;
            void enum_attr_map_print_xml() ;
            void top_level_objects_print_xml() ;
            void sie_print_json() ;
            void sie_append_runtime_objs() ;
            void runtime_objects_print(std::fstream & sie_out) ;

        private:

            void top_level_objects_print(std::ofstream & sie_out) ;
            void top_level_objects_json(std::ofstream & sie_out) ;

            // These are singleton maps holding all attributes known to the sim
            Trick::AttributesMap * class_attr_map ; /* ** -- This is be ignored by ICG */
            Trick::EnumAttributesMap * enum_attr_map ;   /* ** -- This is be ignored by ICG */

    } ;
}

#endif

