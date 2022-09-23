#ifndef MTV_HH
#define MTV_HH
/*
    PURPOSE: ( MTV Class - Malfunction events for Malfunction Trick View )
*/
#include <string>
#include "trick/IPPythonEvent.hh"

namespace Trick {

class MTV {
    public:

        MTV() ;

        Trick::IPPythonEvent  dummy_event ;           /**< trick_io(*io) trick_units(--) */

        Trick::IPPythonEvent ** mtv_list ;            /**< trick_io(*io) trick_units(--) */

        /** Count of user events to be displayed by mtv.\n */
        unsigned int mtv_count ;                      /**< trick_io(*io) trick_units(--) */

        /** Running count of last new event or event deletion, so mtv knows when to update.\n */
        int mtv_update_ticker;                        /**< trick_io(*io) trick_units(--) */

        int add_event ( Trick::IPPythonEvent * in_event ) ;
        int delete_event ( Trick::IPPythonEvent * in_event ) ;

        Trick::IPPythonEvent * get_event ( std::string event_name ) ;

        int send_event_data() ;
} ;

}

int mtv_add_event(Trick::IPPythonEvent * in_event) ;
int mtv_delete_event(Trick::IPPythonEvent * in_event) ;
Trick::IPPythonEvent * mtv_get_event(std::string event_name) ;
int mtv_send_event_data() ;

#endif

