/*
    PURPOSE:
        (Real Time Injector)
*/

#ifndef RTISTAGER_HH
#define RTISTAGER_HH

#include <map>
#include <vector>

#include "trick/RtiEvent.hh"
#include "trick/RtiList.hh"
#include "trick/RtiExec.hh"

namespace Trick {

/**
  The Real Time Injector provides a mechanism that variables may be set to values at a given
  time without affecting the real-time performance of a simulation any more than other real-time
  routines.

  Operation Scenario:
  @li On a lower priority thread, a request is made to change the value of an item at a certain time.
  @li This item (event) gets placed in a list
  @li A higher piority thread gets the information and changes the value of the data
  @li memory operation (creation and deletion) take place in the lower priority threads.

*/

class RtiStager {

    public:
     RtiStager() ;
     virtual ~RtiStager() ;

     //template <class T> Trick::RtiEventBase * createRtiEvent(char *variable, T value ) ;
     /**
        Creates an RtiEvent to do a single assignment using a double value
        @param var - variable name in the simulation
        @param value - what value should the variable be after execution as a double
        @return - always returns zero (should probably be void then)
     */
     virtual int Add(char *var, double value);

     /**
        Creates an RtiEvent to do a single assignment using a long long value
        @param var - variable name in the simulation
        @param value - what value should the variable be after execution as a long long
        @return - always returns zero (should probably be void then)
     */
     virtual int Add(char *var, long long value);

     /**
        Move stored up injections belonging to the current thread to the firing line.
        @param thread_id - the thread to use to fire the event, defaults to main thread.
        @return - always returns zero (should probably be void then)
     */
     virtual int Fire( unsigned int thread_id = 0 );

     /**
         List is a function that will display the contents of the list for situational awareness as to
         the events that are to be scheduled
     */
     virtual int List( void ) ;

     /**
         Sets the debug flag to the incoming parameter.
         @param on_off - true or false
     */
     int SetDebug( bool on_off ) ;

     /**
         Sets the execution to only execute at a multiple number of frames
         @param mult - the multiple to set
     */
     int SetFrameMultiple ( unsigned int thread_id , unsigned int mult ) ;

     /**
         Offsets execution a number of frames
         @param offset - the offset to set
         @return 0 if the offset is valid, -1 if offset >= frame_multiple
     */
     int SetFrameOffset ( unsigned int thread_id , unsigned int offset ) ;

     /**
         Adds an RTI executor.  Called from the S_define file, 1 for each thread.
         @param rtie - The executor to add
     */
     void AddInjectorExecutor( Trick::RtiExec * ) ;

     RtiExec * GetRtiExecutor( unsigned int thread_id ) ;

    protected:
     bool debug ;  /**< ** prints debug messages about rti activities */
     std::map < pthread_t, RtiList * > list_map ;  /**< ** map of event lists keyed by thread */
     std::vector < RtiExec * > executors ;  /**< ** map of event lists keyed by thread */

     /**
        Adds event created in Add to the event list.
        @param rei - new event to add to list
        @return - always returns zero (should probably be void then)
     */
     virtual int AddtoList (Trick::RtiEventBase *rei);

} ;

}

#endif


