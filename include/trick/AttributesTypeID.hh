
#ifndef ATTRIBUTESTYPEID_HH
#define ATTRIBUTESTYPEID_HH
#include <string>
#include <string.h>
#include <iostream>

namespace Trick {
    template<typename T> const char * AttributesTypeID(__attribute__((unused))T * not_used) {
        std::string pretty_func = __PRETTY_FUNCTION__ ;
        std::string key ;
        size_t found ;

        /* Try the GCC style function declaration first */
        key = "T = " ;
        found = pretty_func.find(key) ;

        if ( found != std::string::npos ) {
            pretty_func.replace(0, found+key.length(), "") ;

            key = "]" ;
            found = pretty_func.rfind(key) ;
            pretty_func.replace(found, pretty_func.length(), "") ;
        } else {

            /* Try the LLVM style function declaration */
            key = "(" ;
            found = pretty_func.find(key) ;
            if ( found != std::string::npos ) {
                pretty_func.replace(0, found+key.length(), "") ;

                key = "*" ;
                found = pretty_func.rfind(key) ;
                pretty_func.replace(found, pretty_func.length(), "") ;

                key = " \t" ;
                found = pretty_func.find_last_not_of(key) ;
                pretty_func.replace(found + 1, pretty_func.length(), "") ;
            } else {
                /* oh-no, trouble */
                return NULL ;
            }
        }

        return strdup(pretty_func.c_str()) ;
    }
}

#endif
