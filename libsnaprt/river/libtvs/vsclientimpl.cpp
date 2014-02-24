#include <float.h>
#include <cmath>
#include <iostream>
#include <sstream>
using std::cerr;
using std::endl;
using std::ostringstream;
using std::stringstream;

#include "vsuserif.hh"
#include "vsstructs.hh"
#include "vsclientimpl.hh"
#include "vsclientconnector.hh"
#include "../libtc/tokenizer.hh"
#include "sniffsrvcmds.hh"
#include "../libutils/exception.hh"
#include "../libutils/autolock.hh"
#include "../libutils/convutils.hh"

void VSClientImpl::setRate(double rate)
{
    _value_client->setRate(rate);
}

VSClientImpl::VSClientImpl(VSUserIF* user, VSClientConnector* connector,
                           double rate, vector<TrickParam>& params,
                           unsigned int buffsize):
                      _valueparams(params),
                      _rate(rate), _user(user),
                      _keep_running(true),
                      _shutdown(false),
                      _BUFFSIZE(buffsize),
                      _receivethreadspawned(false),
					  _timebase(1.0),
					  _connector(connector)
{
    _add_client    = connector->newClient();
    _status_client = connector->newClient();
    _value_client  = connector->newClient();

    if (!_add_client || !_status_client || !_value_client) {
        throw Exception("Unable to get good connection to SIM");
    }

    // Workaround a variable server bug -- pause state is uninitialized
    _status_client->pause();
    _status_client->unpause();
    _value_client->pause();

    _add_client->setBlockIO(TCClient::TC_COMM_TIMED_BLOCKIO);
    _add_client->setRate(0.0001); // Set rate small since this determines how
                                  // quickly add cmds are processed
    _add_client->pause();

    _status_client->setSynchronous(false);
    _status_client->setRate(1.0);
    _status_client->setBlockIO(TCClient::TC_COMM_TIMED_BLOCKIO);
    _receive_status_thread = new Thread();
    _receive_status_thread->Start(
                 (void* (*)(void*)) (&VSClientImpl::_static_receive_status), 
                 (void*) this);
    // Wait until either thread has established itself or found out that it couldn't

    while (!_receive_status_thread->isDead() && !_receive_status_thread->isRunning()) {
        Utils::sleep(10000);
    }

    if (_receive_status_thread->isDead()) {
        _add_client->exit();
        throw Exception("Unable to start status thread");
    }

//cerr << " * * * * * Got GOOD STATUS values from SIM" << endl;

    // For value client, sync initially false to get an initial set of values 
    // but will be reset after the initial data arrives
    setRate(0.0001);
    _value_client->setBlockIO(TCClient::TC_COMM_TIMED_BLOCKIO);
}

void VSClientImpl::startReceive()
{
    setRate(_rate);
    _receive_value_thread = new Thread();
    _receive_value_thread->Start(
                 (void* (*)(void*)) (&VSClientImpl::_static_receive_value), 
                 (void*) this);
}
    

VSClientImpl::~VSClientImpl() 
{
//cerr << "Shutting down now!" << endl;
    _shutdown = true;
    _keep_running = false;
    _add_client->exit();
    _receive_status_thread->Join();

//cerr << "Joined status thread!" << endl;
    if (_receivethreadspawned) {
        cerr << "Joining receive thread" << endl;
        _receive_value_thread->Join();
    }
//cerr << "Joined receive thread!" << endl;

    delete _add_client;
    delete _value_client;
    delete _status_client;

    for (unsigned int ii = 0; ii < _statusparams.size(); ii++) {
        _statusparams[ii].deallocate();
    }
}

unsigned int VSClientImpl::_countDirect(const vector<TrickParam>& params)
{
    unsigned int total = 0;
    for (unsigned int ii = 0; ii < params.size(); ii++) {
        if (params[ii].type == TrickParam::DIRECT) ++total;
    }
    return total;
}

int VSClientImpl::_add_data(vector<TrickParam>& params,
                            vector<string>& values)
{
    for (unsigned int ii = 0; ii < values.size(); ii++) {
        sscanf(values[ii].c_str(), "%lf" , &(params[ii].incoming_val));
        if (ii == _TIME_IDX) params[ii].incoming_val /= _timebase;
    }

    return 0;
}

void* VSClientImpl::_static_receive_status(void* arg)
{
    VSClientImpl* river = static_cast<VSClientImpl*>(arg);
    river->_receive_status();
    river->_receive_status_thread->setState(Thread::THREAD_DEAD);
    return NULL;
}

void VSClientImpl::_receive_status()
{
    int  datasize = 16384;
    vector<char> data(datasize*2);

    string buffer;
    string remainder;

	string modeparam = _connector->getModeParam();
	string timeparam = _connector->getTimeParam();
	string timebaseparam = _connector->getTimeBaseParam();
    bool needTimebase = (timebaseparam != "");

    // Add time as param 0
    try {
        if (_add(false, _statusparams, timeparam) != SUCCESS) {
            _keep_running = false;
cerr << "Cannot add time" << endl;
        }
    } catch (Exception& e) {
        _keep_running = false;
cerr << "Problem trying to add time: " << e.what() << endl;
        return;
    }

    // Add mode as param 1
    try {
        if (_add(false, _statusparams, modeparam) != SUCCESS) {
cerr << "Add failed for " << modeparam << endl;
            _keep_running = false;
            return;
        }
    } catch (Exception& e) {
        _keep_running = false;
cerr << "Problem trying to add mode: " << e.what() << endl;
        return;
    }

    // Add time base as param 2
    try {
        if (needTimebase
            && (_add(false, _statusparams, timebaseparam) != SUCCESS)) {
            _keep_running = false;
cerr << "Cannot add time base" << endl;
        }
    } catch (Exception& e) {
        _keep_running = false;
cerr << "Problem trying to add time: " << e.what() << endl;
        return;
    }

    // Initially, assume frozen and set value client to non-synchronized
    // If this assumption is wrong, we'll know as soon as we get our first
    // packet.
    _vsstate.is_frozen = true;
    _vsstate.need_update = true;
    _value_client->setSynchronous(false);

    bool first_token;
    vector<string> values;
    int numDirectParams = _countDirect(_statusparams);

    /* While the client is good... */
    while ( _keep_running && _status_client->isValid()) {
        first_token = true;
        
        int status = _status_client->readvalues(remainder, numDirectParams,
                                                values);
        if (status == 0 && values.size() > 0) {
            _add_data(_statusparams, values);
            double& timestamp = _statusparams[0].incoming_val;
            double& simmode = _statusparams[1].incoming_val;
            if (needTimebase) _timebase = _statusparams[2].incoming_val;

            // Call user to let them know data updated
               // Simmode 5 = RUNNING: go to Synchronous mode
            if (::fabs(simmode - 5) < 0.1 && _vsstate.is_frozen) {
                // Need to lock here since we are sending on the
                // value client's thread
                AutoLock locker(&_handler_mutex);
                _value_client->setSynchronous(true);
                _vsstate.is_frozen = false;
                _vsstate.need_update = true;

               // Simmode 1 = FROZEN: go to Asynchronous mode
            } else if (::fabs(simmode - 1) < 0.1 && !_vsstate.is_frozen) {
                // Need to lock here since we are sending on the
                // value client's thread
                AutoLock locker(&_handler_mutex);
                _value_client->setSynchronous(false);
                _vsstate.is_frozen = true;
                _vsstate.need_update = true;
            }
            _vsstate.is_connected = true;
            if (_vsstate.need_update) {
                _vsstate.timestamp = timestamp;
                VSMessage msg(_vsstate);
                msg.type = VSMessage::VSSTATUS_UPDATE;

                AutoLock locker(&_handler_mutex);
                _user->handler(msg); 
                _vsstate.need_update = false;
            }
            // Now that we have good data, signal successful connection
            if (!_receive_status_thread->isRunning()) {
//cerr << "Signalling status connected" << endl;
                _receive_status_thread->setState(Thread::THREAD_RUNNING);
            }
        }
        Utils::sleep(500000); // Half-second sleep
    }

    if (!_shutdown) {
//cerr << "Not shutdown" << endl;
    } else {
//cerr << "Shutdown" << endl;
}

    // Process disconnection
    _status_client->exit();

    // Generally, we'll attempt to supply status back to the client to indicate
    // that we've disconnected. However, in the case of a hard shutdown (where
    // the destructor has been called), assume we're going down, and don't
    // try to pass back status since we don't know whether the client still
    // exists.
    if (!_shutdown) {
        _vsstate.is_connected = false;
        VSMessage msg(_vsstate);
        msg.type = VSMessage::VSSTATUS_UPDATE;
        AutoLock locker(&_handler_mutex);
        _user->handler(msg);
    } else {
        /* Signal value client to go down, if it hasn't already */
        _keep_running = false;
    }
}

void* VSClientImpl::_static_receive_value(void* arg)
{
    VSClientImpl* river = static_cast<VSClientImpl*>(arg);
    river->_receive_value();
    river->_receive_value_thread->setState(Thread::THREAD_DEAD);
    return NULL;
}

void VSClientImpl::_receive_value()
{
    _receivethreadspawned = true;

    string buffer;
    string remainder;
    vector<string> values;
    int status;
    int numDirectParams;

    static VSMessage msg(_vsstate);
    msg.type = VSMessage::VSDATA_UPDATE;

    // Trick should not yet be sending data, but there's a bug which causes data
    // to be sent with some Trick versions. At this point, Trick should have all
    // the parameters, and we can check this by requesting a send while we are 
    // paused. If Trick has the bug and is already sending, there's no point in
    // asking. We just have to hope that at some point Trick sends all the
    // parameters.
    if (!_value_client->hasReceiveData()) {
        _value_client->sendNow();
        numDirectParams = _countDirect(_valueparams);
        status = _value_client->readvalues(remainder, numDirectParams, values);
        if (status < 0) {
            cerr << "Trick is not sending all values -- shutting down" << endl;
            _keep_running = false; // Set to shut down connection
        }
    }

    if (_keep_running) {
        _value_client->unpause();
    }

    _vsstate.is_receiving = false;
    _receive_value_thread->setState(Thread::THREAD_RUNNING);
    while ( _keep_running && _value_client->isValid()) {

        // Process packets
        // Count number of direct params here in case params are added
        // after receive is started.
        numDirectParams = _countDirect(_valueparams);
        status = _value_client->readvalues(remainder, numDirectParams, values);
        if (status < 0) {
            cerr << "Wrong number of data values from Trick -- ignoring" << endl;
            continue;
        }

        if (!_keep_running) break;
        if (status == 0 && values.size() > 0) {
            if (_add_data(_valueparams, values) == 0) { 
                // Successfully parsed the data and updated data vals
                // Call user to let them know data updated
                _vsstate.is_receiving = true;
                _vsstate.need_update = true;
                AutoLock locker(&_handler_mutex);
                try {
                    _user->handler(msg); 
                } catch (std::exception& e) {
                    _handler_mutex.Unlock();
                    break;
                }
            }
        } else {
            _vsstate.is_receiving = false;
            _vsstate.need_update = true;
            if ( tc_errno == TC_EWOULDBLOCK ) {

                // No data read
                Utils::sleep(_rate*1000000/2);
            } else {
                // Timed out
                if (tc_errno == TC_READWRITE_TIMEOUT) {
//cerr << "Timeout on value" << endl;
                }

                // Error: TODO: Send status
                break ;
            }
        }
    }

//cerr << "Shutting down value thread" << endl;
    if (_keep_running) {
        // Process leftover packets sitting in remainder
        vector<string> packets = Tokenizer::tokenize(remainder, "\n");
        for (unsigned int ii = 0 ; ii < packets.size(); ii++) {
            int numDirectParams = _countDirect(_valueparams);
            _status_client->readvalues(packets[ii], numDirectParams, values);
            if (_add_data(_valueparams, values) == 0) { 
                // Successfully parsed the data and updated data vals
                // Call user to let them know data updated
                AutoLock locker(&_handler_mutex);
                try {
                    _user->handler(msg); 
                } catch (std::exception& e) {
                    // TODO: what to put here? Is ignoring ok?
                    break;
                }
            }
        }
    }

    // Send final status
    _vsstate.is_receiving = false;

    msg.type = VSMessage::VSSTATUS_UPDATE;
    {
        AutoLock locker(&_handler_mutex);
        VSState state;
        state.is_receiving = false;
        state.is_connected = false;
        VSMessage lastmsg(state);
        _user->handler(lastmsg); 
    }

    _value_client->exit();

    /* Socket descriptor was invalid... cleanup */
    _keep_running = false;
}

// Get initial values for a parameter
VSClientImpl::IOStatus
VSClientImpl::get_initial_value(const string& name,
                                double& value,
                                string& units)
{
    VSClientImpl::IOStatus ret = DONTKNOW;
    if (_add_client->isValid()) {
        _add_client->getUnits(name, units, value);
        if (units != "") {
            ret = SUCCESS;
        }
    }
    return ret;
}

// Internal add -- used by external add and others
VSClientImpl::IOStatus VSClientImpl::_add(bool forValue, 
                                          vector<TrickParam>& params, 
                                          const string& name)
{

    string units;
    double value;

    AutoLock locker(&_add_mutex);
    VSClientImpl::IOStatus ret = DONTKNOW;

    for (unsigned int ii = 0; ii < params.size(); ii++) {
        if (params[ii].name == name) {
            return FAILURE;  // Duplicate name - don't add
        }
    }

// TODO: Do we need a check on units coming back that don't match sourceunits
// when we're on Trick 10?
    if (_add_client->isValid()) {
        _add_client->getUnits(name, units, value);
        if (units != "") {
            ret = SUCCESS;
            TrickParam tp(name, TrickParam::DIRECT, units, value);
            tp.allocate(_BUFFSIZE);
            params.push_back(tp);
            if (forValue) {
                _value_client->add(name);
            } else {
                _status_client->add(name);
            }
        }
        else { 
            cerr << "Got back empty units" << endl;
        }
    }

    return ret;
}

void VSClientImpl::addmultiple(unsigned int numnames, char** names)
{
    string units;
    string successvars;
    vector<string> status;
    vector<TrickParam> tmpparams;
    if (numnames == 0) return;

    // Tmpparams will contain TrickParam entries for all param names that exist
    { // Block for AutoLock
        AutoLock locker(&_add_mutex);
        tmpparams = _add_client->exists(numnames, names);
    }

    if (tmpparams.size() > 0) {
        _value_client->addMultiple(tmpparams);
        for (unsigned int ii = 0; ii < tmpparams.size(); ii++) { 
            tmpparams[ii].allocate(_BUFFSIZE);
            _valueparams.push_back(tmpparams[ii]);
        }
    }
}

// External add -- for variables
// Units will be updated on success.
VSClientImpl::IOStatus VSClientImpl::add(string name)
{
    return _add(true, _valueparams, name);
}

int VSClientImpl::sendText(const char* line)
{
    return _add_client->sendText(line);
}
