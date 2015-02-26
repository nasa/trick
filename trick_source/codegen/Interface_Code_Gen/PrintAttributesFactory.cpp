
#include <iostream>
#include <stdlib.h>

#include "PrintFileContentsBase.hh"
#include "PrintFileContents10.hh"
#include "PrintFileContents13.hh"

/** returns true if the command line attributes version is supported */
bool validAttributesVersion( int attr_version ) {
    switch (attr_version) {
        case 10:
        case 13:
            return true ;
        default:
            return false ;
    }
}

/**
    Returns a PrintFileContentsBase pointer to a version specific
    print class based on attribute version
 */
PrintFileContentsBase * createFileContents( int attr_version ) {
    switch (attr_version) {
        case 10:
            return new PrintFileContents10() ;
        case 13:
            return new PrintFileContents13() ;
        default:
            std::cout << "OK, how did you get here with an invalid attributes version " << attr_version << std::endl ;
            exit(-1) ;
    }
}

