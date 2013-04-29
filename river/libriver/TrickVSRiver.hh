#ifndef _TRICKVSRIVER_H_
#define _TRICKVSRIVER_H_

#include "../libtvs/vsuserif.hh"
#include "../libtvs/vsclientimpl.hh"
#include "../libtvs/vsclientconnector.hh"
#include "../libtvs/transformation.hh"
#include "../libtvs/vsstructs.hh"
#include "../libutils/mutex.hh"
#include "DataRiver.hh"

#include <vector>
#include <map>
#include <string>

/** \ingroup libdp 
 * Provides a data river implementation that connectes to a Trick variable
 * server.
 * This river is constructed by the factory from a valid connector, and in 
 * addition to the usual set of parameter names and transforms, takes a rate
 * parameter that specifies how fast data ought to be requested from the
 * variable server, and an update callback which is used to update the river
 * client on STATUS changes only, NOT on value updates.
 *
 * Parameters and connection information are passed to the VSClientImpl, which
 * is responsible for gathering the data from the variable server and storing
 * values back in the river. Values are stored as they arrive, then 
 * TrickVSRiver is notified, since it has to calcuate the values of any 
 * transformed variables. The intent is that the owner of the river will ask 
 * the river for the values it needs via the normal river mechanism (see the 
 * DataRiver class for details). In addition to reducing processing, this
 * approach hides the fact that the TrickVSRiver is receiving live updates 
 * while file-based rivers already have all their data at construction.
 */
class TrickVSRiver: public DataRiver, VSUserIF {
  public:
    /// Callback handlers from connection
    virtual void handler(VSMessage& msg);

    /// Constructor
    TrickVSRiver(VSClientConnector* connector, double rate,
           unsigned int numparams, char** param_names,
           vector<Transformation>& transforms,
           void (*update_callback)(VSMessage& msg)
          );
    ~TrickVSRiver();

    double*      getVals(const char* param) {
        if (_indices.count(param)) {
            return _params[_indices[param]].vals;
        } else {
            return NULL;
        }
    }
    const char*  getUnit(const char* param) 
                                    { return _params[_indices[param]].units.c_str(); }
    int          getNumPoints()     { return _npoints; }
    const char*  getFileName()      { return _filename.c_str(); }
    const char*  getTimeUnit()      { return _params[_indices[_TIME_STR]].units.c_str(); }
    double*      getTimeStamps()    { return _params[_indices[_TIME_STR]].vals; }
    double       getStartTime();
    double       getStopTime();
    int          getNumParams()     { return _params.size(); }
    int          getNumTransforms() { return _transforms.size(); }
    void         setDataRate(double rate) { _vsclient->setRate(rate); }
    double       getDataRate() { return _vsclient->getRate(); }
    virtual int  getIndexAtTime( double* time ) ;
    virtual void getOneShotValue(const string& name, double &value, string &units);
    virtual int  sendText(const char* line);

    /// Lock modifications/access to parameters
    virtual void lock()             { _datalock.Lock(); }

    /// Non-blocking lock on modifications/access to parameters
    virtual int  trylock()          { return _datalock.Trylock(); }
 
    /// Unlock lock mutex
    virtual void unlock()           { _datalock.Unlock(); }

  private:
    /// Add multiple variables to the list of parameters
    VSClientImpl::IOStatus _addmultiple(unsigned int numnames, char** names);

    /// Add transformed variables
    void _addtransforms(vector<Transformation>& transforms);

    /// Time conversion from raw time to processed time
    double _time(double time);

    /// Time conversion from processed time to raw time
    double _rawTime(double time);

    void _cleanup(); ///< Deallocate data
    void _value_handler(); ///< Handle incoming values from the variable server
    int _buffer_start; ///< Start index of circular buffer
    Mutex _datalock; ///< Data locking mutex

    std::map<string, int> _indices;  // Name-to-index map

    // By convention, the 0th entry will be the time entry
    std::vector<TrickParam> _params;  ///< Storage for data parameters
    std::vector<Transformation> _transforms; ///< Storage for data transforms

    int _npoints ;        // Convenience value for current number of points
    unsigned int _MAXVALS ;       // Maximum number of points supported per param
    static const int _MEMORYSPACE ;   // Total memory space for values
    char* _TIME_STR;                  // Name of the time variable

    std::string _filename;     // Psuedo-filename required by DataRiver interface

    VSClientImpl* _vsclient;  // Connection client

    /// Callback to client on status update
    void (*_update_callback)(VSMessage&); 

    /// Add a variable to the list of variables
    VSClientImpl::IOStatus _add_variable(const char* name);
    VSClientImpl::IOStatus _get_initial_value(const std::string& name,
                                              double& value,
                                              std::string& units);

    /// True if we are in the process of adding a variable to the list
    bool _addingVars;
} ;

#endif
