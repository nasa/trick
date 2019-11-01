#ifndef ENUMATTRIBUTESMAP_HH
#define ENUMATTRIBUTESMAP_HH
/*
    PURPOSE: (Provides a map of class/struct attributes and enumeration attributes)
*/
#include <map>
#include <string>
#include <fstream>
#include "trick/attributes.h"

namespace Trick {

/**
 * These maps stores all the attributes by class/struct/enum name
 * So these 2 classes could inherit from a template or something fancy, but
 * they are so simple I decided to leave them separate.
 */

    class EnumAttributesMap {

        public:
            /**
             * Returns a pointer to the singleton Trick::AttributesMap instance.
             * @return    A pointer to Trick::AttributesMap.
             */
            static EnumAttributesMap * attributes_map() {
                if ( pInstance == NULL ) {
                    pInstance = new Trick::EnumAttributesMap() ;
                }
                return pInstance ;
            }

            EnumAttributesMap() {} ;
            ~EnumAttributesMap() {} ;

            /**
             * Adds a type and the corresponding attributes
             * @param type    The name of the type.
             * @param attr    Pointer to the attributes.
             */
            void add_attr( std::string type , ENUM_ATTR * attr ) {
                name_to_attr_map[type] = attr ;
            }

            /**
             * Gets the attributes of a type.
             * @param type    The name of the type.
             * @return    The attributes of the type.
             */
            ENUM_ATTR * get_attr( std::string type ) {
                if ( name_to_attr_map.find(type) != name_to_attr_map.end() ) {
                    return name_to_attr_map[type] ;
                }
                return NULL ;
            }

            void print_xml( std::ofstream & outfile ) ;
            void print_json(std::ofstream & sie_out ) ;

        private:
            std::map<std::string, ENUM_ATTR *> name_to_attr_map ;
            static EnumAttributesMap * pInstance ;

    } ;
}

#endif

