#ifndef _DATARIVER_H_
#define _DATARIVER_H_

#include <iostream>
using namespace std;

/** \defgroup libdp Data River Library
 *  Provides source-agnostic access to Trick variable values.
 */

/** \ingroup libdp
    Abstract parent class for all data rivers.
    A data river encapsulates the process of providing data in a stream form
    for playback. A river consists of a set of parameter names mapped to a 
    values array (an array of double) and a units string. Each parameter is
    assumed to have an equal number of values, so a river is essentially a
    set of parallel arrays. Each river has a special parameter which represents
    the sim time. It is assumed that a river slice (i.e. the set of all 
    parameter values with a given index, i) contains parameters sampled at 
    the time given by the time parameter value at index i. For a synchronized 
    sim, the values will be time homogeneous, and the assumption is correct.
    For an unsynchronized sim, the assumption will be approximately correct.

    In addition to parameters available directly from the data source, data 
    rivers support parameters derived from source parameters via transforms 
    specified at the time of river construction.

    Data rivers are not constructed directly. Rather, they constructed through
    factory functions. See DataRiverFactory.

    Typical application river usage works like this:
    - Application uses factory method to construct the river
    - Application enters processing loop
      - Application figures out the current time
      - Application calls getIndexAtTime() to retrieve the corresponding slice index
      - Application accesses values for the parameters it needs using getVals()
    - End of processing loop
    - Application destroys the river
 */
class DataRiver {

       public:
               DataRiver() : _rate(0),_line(0) {}

               virtual ~DataRiver() ;

               /// Returns the number of points in the river
               virtual int           getNumPoints()       = 0 ;

               /// Returns the array of values for param
               virtual double*       getVals(const char* param) = 0 ;

               /// Returns the arrays of timestamp values
               virtual double*       getTimeStamps()      = 0 ;

               /// Returns the units string for param
               virtual const char*   getUnit(const char* param) = 0 ;

               /** Returns the filename containing the data points or a 
                   remote file spec for live data.
                 */
               virtual const char*   getFileName()        = 0 ;

               /// Returns the name of the time parameter
               virtual const char*   getTimeUnit()        = 0 ;

               /// Returns the number of parameters in the river
               virtual int           getNumParams()       = 0 ;

               /// Sets data rate -- only applicable to live rivers
               virtual void          setDataRate(double rate) {_rate = rate;}

               /// Gets data rate -- only applicable to live rivers
               virtual double        getDataRate() { return _rate; }

               /// Returns the first timestamp in the river
               virtual double        getStartTime();

               /// Returns the last timestamp in the river
               virtual double        getStopTime();

               /// Returns the index value corresponding to the given time
               virtual int           getIndexAtTime( double* time ) ;

               /// Returns the number of transforms applied to the data
               virtual int           getNumTransforms() = 0;

               /// Return initial value and units for a given variable
               virtual void          getOneShotValue(const string& name, double& value, string& units);

               /// Send text command -- only applicable to live rivers
               virtual int           sendText(const char* line)
               {
                   _line = line;
                   return 0;
               }

               /// Locks the river to prevent updates which processing
               virtual void          lock() {};

               /// Unlocks the river to resume updates
               virtual void          unlock() {};

       protected:

               /// Compute the river index for a given timestamp
               virtual int _idxAtTimeBinarySearch(double* timestamps,
                                                  int low, int high,
                                                  double* time ) ;
       private:
             double _rate;
             const char* _line;
} ;

#endif
