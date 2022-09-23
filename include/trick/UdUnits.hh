#ifndef UDUNITS_HH
#define UDUNITS_HH
/*
    PURPOSE: ( UdUnits Class )
*/
#include <udunits2.h>
#include <string>

namespace Trick {

class UdUnits {
    public:
        static ut_system * get_u_system() ;
        int read_default_xml() ;
        int read_user_xml(std::string) ;

    protected:
        static ut_system * u_system ; // ** system of units
} ;

}
#endif
