
#include <iostream>
#include <fstream>

class PrintFileContentsBase ;

/** returns true if the command line attributes version is supported */
bool validAttributesVersion( int attr_version ) ;

/**
    Returns a PrintFileContentsBase pointer to a version specific
    print class based on attribute version
 */
PrintFileContentsBase * createFileContents( int attr_version ) ;

