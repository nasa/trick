/*
PURPOSE: (Conversion Function Class for Units Conversion Package)
 
PROGRAMMERS:
    (((John M. Penn) (L-3Com/Titan) (May 2006) (v1.0) 
      (Initial Release))) 
*/

/*
 * $Log: UCFn.hh,v $
 * Revision 1.2  2006-05-09 10:09:47-05  lin
 * gcc 4.1 doesn't like trick_source/data_products/Log/log.h
 *
 * Revision 1.1  2006-05-02 13:00:06-05  penn
 * Initial revision
 *
 *
 */
#ifndef UCFN_HH
#define UCFN_HH
#include <string.h>
#include <iostream>

class UCFn {
public:
    double C[2];          /* ** */
    char   t_name[32];    /* ** */
    char   f_name[32];    /* ** */

    UCFn( const char *t_name, const char *f_name, double C1, double C0);
    double eval( double val);

#ifndef SWIG
    friend std::ostream& operator<< ( std::ostream& s, const UCFn *p);
#endif
    
};

#endif
