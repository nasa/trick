/*
    PURPOSE:
        (Real Time Injector)
*/

#ifndef RTIEXEC_HH
#define RTIEXEC_HH

#include <vector>

#include "trick/RtiList.hh"

namespace Trick {

class RtiExec {

    public:
     RtiExec() ;
     virtual ~RtiExec() ;

     /**
        Executes all events on the firing line.
        @return - always returns zero (should probably be void then)
     */
     void AddToFireList ( RtiList * rti_list ) ;

     /**
        Executes all events on the firing line.
        @return - always returns zero (should probably be void then)
     */
     virtual int Exec () ;

     /**
         Sets the debug status for printouts
         @param on_off - true or false
     */
     void SetDebug ( bool on_off ) ;

     /**
         Sets the execution to only execute at a multiple number of frames
         @param mult - the multiple to set
     */
     int SetFrameMultiple ( unsigned int mult ) ;

     /**
         Offsets execution a number of frames
         @param offset - the offset to set
         @return 0 if the offset is valid, -1 if offset >= frame_multiple
     */
     int SetFrameOffset ( unsigned int offset ) ;

     /*
        leaving these variables public so the memory manager
        can see them for checkpointing without using a friend.
      */
     unsigned int frame_multiple ; /**< -- multiple of software_frame cycles to execute */
     unsigned int frame_offset ; /**< -- offset number of frames to execute */

    protected:
     bool debug ;  /**< ** prints debug messages about rti activities */

     pthread_mutex_t list_mutex ;  /**< ** mutex to protect the fire_list */
     std::vector < RtiList * > fire_list ;  /**< ** events to fire */

} ;

}

#endif


