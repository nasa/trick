
/**
 * Provides a map of class/struct name and calculated size difference between ICG and runtime
 */

#ifndef CLASSSIZECHECK_HH
#define CLASSSIZECHECK_HH

#include <map>
#include <string>
#include <iostream>
#include "trick/attributes.h"
#include "trick/AttributesMap.hh"

namespace Trick {

/**
 * These maps stores all the attributes by class/struct/enum name
 * So these 2 classes could inherit from a template or something fancy, but
 * they are so simple I decided to leave them separate.
 */
    struct ClassSizeDiffInfo {
        int diff ;
        std::string file_name ;
        ClassSizeDiffInfo( int in_diff , std::string in_file_name ) :
         diff(in_diff) , file_name(in_file_name) {}
    } ;

    class ClassSizeCheck {

        public:
            /**
             * Returns a pointer to the singleton Trick::ClassSizeCheck instance.
             * @return    A pointer to Trick::ClassSizeCheck
             */
            static ClassSizeCheck * class_size_check() {
                if ( pInstance == NULL ) {
                    pInstance = new Trick::ClassSizeCheck() ;
                }
                return pInstance ;
            }
            static void reset_instance() {
                delete pInstance ;
                pInstance = NULL ;
            }

            ClassSizeCheck() {} ;
            ~ClassSizeCheck() {} ;

            /**
             * Adds a type and the corresponding attributes
             * @param type    The name of the type.
             * @param attr    Pointer to the attributes.
             */
            void add_diff( std::string type , ClassSizeDiffInfo diff_info ) {
                name_to_diff.insert(std::pair<std::string,ClassSizeDiffInfo>(type,diff_info)) ;
            }

            /**
             * Prints all the types that have a difference in size
             */
            void print_nonzero_diffs() {
                std::map<std::string, ClassSizeDiffInfo>::iterator it ;
                for ( it = name_to_diff.begin() ; it != name_to_diff.end() ; it++ ) {
                    int curr_diff = it->second.diff ;
                    ATTRIBUTES * attr = Trick::AttributesMap::attributes_map()->get_attr(it->first) ;
                    unsigned int ii ;
                    for ( ii = 0 ; attr[ii].name[0] != '\0'; ii++ ) {
                        // Look for structured types that are not pointers
                        std::map<std::string, ClassSizeDiffInfo>::iterator search_it ;
                        if ( (attr[ii].type == TRICK_STRUCTURED) and
                             ((attr[ii].num_index == 0) or (attr[ii].index[attr[ii].num_index-1].size != 0)) and
                             ((search_it = name_to_diff.find(attr[ii].type_name)) != name_to_diff.end())
                           ) {
                            int num = 1 ;
                            int jj ;
                            // Calculate total number of structs in the variable
                            for ( jj = 0 ; jj < attr[ii].num_index ; jj++ ) {
                                num *= attr[ii].index[jj].size ;
                            }
                            // Subtract the combined difference
                            curr_diff -= num * search_it->second.diff ;
                        }
                    }
                    if ( curr_diff != 0 ) {
                        std::cerr << it->first << " ICG size and runtime size do not match" << std::endl ;
                        std::cerr << "Search for \"#ifndef TRICK_ICG\" within " << it->second.file_name << std::endl ;
                    }
                }
            }

        private:
            std::map<std::string, ClassSizeDiffInfo> name_to_diff ;
            static ClassSizeCheck * pInstance ;

    } ;

}

#endif

