#ifndef STLINTERFACE_HH
#define STLINTERFACE_HH
/*
    PURPOSE: ( STLInterface Class )
*/
#include <string>

namespace Trick {

class STLInterface {
    public:
        STLInterface() ;
        virtual ~STLInterface() ;

        virtual void checkpoint( std::string obj_name ) = 0 ;
        virtual void restart( std::string obj_name ) = 0 ;

        unsigned int stl_id ;

        bool restore ;

        virtual void post_checkpoint( std::string obj_name ) ;

} ;

} ;

#endif
