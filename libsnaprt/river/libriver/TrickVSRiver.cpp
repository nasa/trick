#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

// This is nasty. Need to include these here to make sure that Tcl/Tk defns
// are in place for dsp.h, which is in a nested include. Otherwise, we will
// get a conflict with doug.h later
#include "tcl.h"
#include "tk.h"

#include "../libtvs/vsclientimpl.hh"
#include "../libtc/tokenizer.hh"
#include "../libtvs/vsclientconnector.hh"
#include "../libutils/exception.hh"
#include "TrickVSRiver.hh"

#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

#ifdef USE_DOUG_H
#include "doug.h"
static DSS_DOUG* doug = 0 ;
static unsigned int nparamstoregister = 0;
static unsigned int nparamssofar = 0;
#endif


using std::cerr;
using std::stringstream;
using std::endl;
using std::vector;
using std::map;
using std::string;
static int totalpts = 0;

double TrickVSRiver::getStartTime()
{
    if (!_npoints) return 0;
    int TIME_IDX = _indices[_TIME_STR];
    return _params[TIME_IDX].vals[_buffer_start];
}

double TrickVSRiver::getStopTime()
{
    if (!_npoints) return 0;
    int TIME_IDX = _indices[_TIME_STR];
    return (_buffer_start) ? _params[TIME_IDX].vals[_buffer_start-1]
                           : _params[TIME_IDX].vals[_npoints-1];
}

int TrickVSRiver::getIndexAtTime( double* time )
{
   int TIME_IDX = _indices[_TIME_STR];

   double rawTime = *time;
   int index = _idxAtTimeBinarySearch(getTimeStamps(), _buffer_start, 
                                      _npoints-1, &rawTime);
   if (index < _buffer_start) index = _buffer_start;
   // If search time falls outside this part of the buffer, search the other
   // side
   if ((index == _npoints - 1) && (_buffer_start > 0)) {
       // If value falls between the two sides, pick the last point of the lower
       // side
       if (rawTime >= _params[TIME_IDX].vals[0]) {
           index = _idxAtTimeBinarySearch(getTimeStamps(), 0, 
                                          _buffer_start-1, &rawTime);
       }
   }

   return index;
}

void TrickVSRiver::_value_handler()
{
    int TIME_IDX = _indices[_TIME_STR];
    bool duplicate = false;

    // Don't interrupt during variable additions
    if (_addingVars) {
        if (trylock() != 0) {
            return;
        }
    } else {
        lock();
    }

    // If we've used up the whole buffer, the index to update will be the start
    // index. Otherwise, it will be at _npoints.
    int update_idx = (_npoints == _MAXVALS) ? (_buffer_start) : (_npoints);
    int prev_idx   = (update_idx) ? (update_idx-1) : (_MAXVALS-1);

    // Replace rather than add data point if duplicate time stamp
    if (_npoints > 0) {

        double currtime;
        double lasttime;
        lasttime = _params[TIME_IDX].vals[prev_idx];
        currtime = _params[TIME_IDX].incoming_val;

        if (lasttime == currtime) {
            // Duplicate timestamp -- use the previous value's index
            duplicate = true;
            update_idx = prev_idx;
        }
    }

    for (int ii = 0; ii < getNumParams(); ii++) {
        _params[ii].vals[update_idx] = _params[ii].incoming_val;
    }

    for (unsigned int ii = 0; ii < _transforms.size(); ii++) {
        try {
            _transforms[ii].callTransform(update_idx);
        } catch (Exception& e) {
            unlock();
            throw e;
        }
    }

    if (!duplicate) {
        totalpts++;
        if (_npoints < _MAXVALS) {
            _npoints++ ;
        } else {
            _buffer_start++;
            if (_buffer_start == _MAXVALS) _buffer_start = 0;
        }
    }

    unlock();
}

void TrickVSRiver::handler(VSMessage& msg)
{
    switch (msg.type) {
        case VSMessage::VSSTATUS_UPDATE:
            _update_callback(msg);
            break;
        case VSMessage::VSDATA_UPDATE:
            _value_handler();
            break;
        default:
            break;
    }
}

// Utility function for convenience
// Returns an enum with value:
//    SUCCESS, if successful
//    FAILURE, if variable doesn't exist in the sim
//    DONTKNOW, if connection is not working
VSClientImpl::IOStatus TrickVSRiver::_add_variable(const char* name)
{

    string units;
    VSClientImpl::IOStatus ret;

    ret = _vsclient->add(name);

    // If successful, add to index
    if (ret == VSClientImpl::SUCCESS) {
        _indices[name] = _params.size()-1 ;
    }
    return ret;
}

// Utility function for convenience
// Returns an enum with value:
//    SUCCESS, if successful
//    FAILURE, if variable doesn't exist in the sim
//    DONTKNOW, if connection is not working
VSClientImpl::IOStatus TrickVSRiver::_get_initial_value(const string& name,
                                                        double& value,
                                                        string& units)
{
    return _vsclient->get_initial_value(name, value, units);
}

// Externally visible function to get an immediate value for a variable without
// adding it to the list of variable to retrieve from the VS
void TrickVSRiver::getOneShotValue(const string& name, double &value, string &units)
{
    _get_initial_value(name, value, units);
}

int TrickVSRiver::sendText(const char* line)
{
    return _vsclient->sendText(line);
}

VSClientImpl::IOStatus TrickVSRiver::_addmultiple(unsigned int numnames, char** names)
{
    // numnames could be 0 if our variables are all transformed
    if (numnames == 0) return VSClientImpl::SUCCESS;

    unsigned int startsize = _params.size();
    _vsclient->addmultiple(numnames, names);

    // Couldn't add any parameters
    if (_params.size() == startsize) {
        return VSClientImpl::FAILURE;
    }

    for (unsigned int ii = startsize; ii < _params.size(); ii++) {
        _indices[_params[ii].name] = ii;
    }
    return VSClientImpl::SUCCESS;
}

void TrickVSRiver::_addtransforms(vector<Transformation>& transforms)
{
    lock();
    int newparams = 0;

    for (unsigned int ii = 0; ii < transforms.size(); ii++) {        
        const vector<string>& names = transforms[ii].inparams();
        bool valid = true;
        for (unsigned int jj = 0; jj < names.size() && valid; jj++) {
            // See if the param is constant. Should not throw an exception
            // since we're loading the list from param names!
            TrickParam& tp = transforms[ii].getInparam(names[jj]);
            try {
                if (tp.type == TrickParam::CONSTANT) continue;
            } catch (Exception& e) {
                valid = false;
                continue;
            }

            // See if we already have a stream for this param
            int pos = -1;
            for (unsigned int kk = 0; kk < _params.size(); kk++) {
                if (_params[kk].name == names[jj]) {
                    pos = kk;
                    break;
                }
            }
            if (pos == -1) { // New param -- need to allocate
                bool addedVar = false;
                // If param is oneshot, grab value and store
                if (tp.type == TrickParam::ONESHOT) {
                    double value;
                    string units;
                    if (_get_initial_value(names[jj], value, units) == VSClientImpl::SUCCESS) {
                        addedVar = true;
                        tp.incoming_val = value;
                        tp.units = units;
                    }
                } else { // Direct var
                    if (_add_variable(names[jj].c_str()) == VSClientImpl::SUCCESS) {
                        addedVar = true;
                    }
                }

                if (!addedVar) {
                    // Uh oh --- variable server doesn't know the param
                    string errmsg("Can't add param ");
                    errmsg += names[jj] + " for transform "
                              + transforms[ii].funcname()
                              + " -- transform will be disabled";
                    DSF_LogWarning(const_cast<char*>(errmsg.c_str()));
                    transforms.erase(transforms.begin() + ii);
                    --ii;
                    valid = false;
                    continue;
                }

                // Oneshot var don't get added to the param liist
                if (tp.type == TrickParam::ONESHOT) continue;

                transforms[ii].getInparam(names[jj]).vals =
                                                _params[_params.size()-1].vals;
            } else {         // Existing variable -- join to existing stream
                transforms[ii].getInparam(names[jj]).vals = _params[pos].vals;
            }
        }
        if (!valid) continue;

        newparams += transforms[ii].outparams().size();

#ifdef USE_DOUG_H
        // Update progress bar
        nparamssofar += transforms[ii].inparams().size();
        doug = (DSS_DOUG*) DSF_GetDataFromProcess( const_cast<char*>("DSS_DOUG"));
        double percent = ((double) nparamssofar) / (double) nparamstoregister;
        stringstream strm;
        strm << "simdata_p::popup_fileload_progressbar_update " << percent*100;
        char cmd[1024];
        strcpy(cmd, strm.str().c_str());
        if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
#endif
    }

    unsigned int startsize = _params.size();
    _params.resize(startsize + newparams);

    for (unsigned int ii = 0; ii < transforms.size(); ii++) {
        const vector<string>& pnames = transforms[ii].outparams();
        for (unsigned int jj = 0; jj < pnames.size(); jj++) {
            unsigned int index = startsize + jj;
            _indices[pnames[jj]] = index;
            TrickParam& tp = transforms[ii].getOutparam(pnames[jj]);
            tp.allocate(_MAXVALS);  // Allocate space for values
            _params[index] = tp;
        }
        _transforms.push_back(transforms[ii]);
        startsize += pnames.size();
    }

    unlock();
}
                     
TrickVSRiver::TrickVSRiver(VSClientConnector* connector, double rate,
               unsigned int numparams, char** param_names,
               vector<Transformation>& transforms,
               void (*update_callback)(VSMessage&)): 
                          _buffer_start(0), 
                          _npoints(0), _TIME_STR(0), _vsclient(0),
                          _update_callback(update_callback),
                          _addingVars(true)
{ 
    _TIME_STR = strdup(connector->getTimeParam().c_str());

    unsigned int totalparams = numparams;
    for (unsigned int ii = 0; ii < transforms.size(); ii++) {
        totalparams += transforms[ii].inparams().size();
    }
    if (totalparams == 0) throw Exception("No parameters requested from river");

    _MAXVALS = _MEMORYSPACE / (sizeof(double)*totalparams);
    _params.reserve(totalparams); // Reserve enough space to hold all 
                                   // parameters without changing size

    ostringstream strm;
    strm << "Variable Server@" << connector->toString() << endl;
    _filename = strm.str();

    try {
        VSClientImpl::IOStatus ret;

        _vsclient = new VSClientImpl(this, connector, rate, _params, _MAXVALS);

        ret = _add_variable(_TIME_STR);
        switch (ret) {
          case VSClientImpl::SUCCESS:
            break;
          default:
            // Deep dog do
            throw Exception("Unable to communicate with the SIM");
            break;
        }

        if (_addmultiple(numparams, param_names) != VSClientImpl::SUCCESS) {
            throw Exception("No variables in the API file found in the SIM");
        }
#ifdef USE_DOUG_H
        nparamstoregister = totalparams;
        nparamssofar = numparams;
        double percent = ((double) nparamssofar) / (double) nparamstoregister;
        stringstream strm;
        strm << "simdata_p::popup_fileload_progressbar_update " << percent*100;
        // Update progress bar 
        doug = (DSS_DOUG*) DSF_GetDataFromProcess( const_cast<char*>("DSS_DOUG"));
        char cmd[1024];
        strcpy(cmd, strm.str().c_str());
        if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
#endif

        // Add transformed variables
        _addtransforms(transforms);

#ifdef USE_DOUG_H
        // Update progress bar to 100%
        doug = (DSS_DOUG*) DSF_GetDataFromProcess( const_cast<char*>("DSS_DOUG"));
        strcpy(cmd, "simdata_p::popup_fileload_progressbar_update 100");
        if (doug) Tcl_EvalEx(doug->interp, cmd, -1, TCL_EVAL_GLOBAL );
#endif
        _addingVars = false;
        _vsclient->startReceive();

    } catch (Exception& E) {
        _cleanup();
        throw E;
    }
}

const int   TrickVSRiver::_MEMORYSPACE  = 100000000;

void TrickVSRiver::_cleanup()
{
    if (_vsclient) {
        delete _vsclient;
        _vsclient = 0;
    }
    if (_TIME_STR) {
        free(_TIME_STR); // Allocated by strdup, must be free'd
        _TIME_STR = 0;
    }
    for (unsigned int ii = 0; ii < _params.size(); ii++) {
        _params[ii].deallocate();
    }
    for (unsigned int ii = 0; ii < _transforms.size(); ii++) {
        _transforms[ii].close();
    }
}

TrickVSRiver::~TrickVSRiver()
{
    _cleanup();
//    cerr << "* * * * * * Received " << totalpts << " points " << endl;
}
