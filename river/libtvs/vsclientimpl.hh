#ifndef _VSCLIENT_HH_
#define _VSCLIENT_HH_

#include <sys/types.h>
#if 0
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "vsuserif.hh"
#include "vsstructs.hh"
#include "vscommunicator.hh"
#include "../libutils/mutex.hh"
#include "../libutils/thread.hh"

using namespace std;
class VSClientConnector;

/** \defgroup libtvs Trick Variable Server Library 
 * The Trick Variable Server Library contains code to interact with a Trick
 * Variable Server. Features include:
 * - Requesting variable parameters from the variable server
 * - Checking for variable existence on the variable server
 * - Handling sim freezes
 * - Parsing messages from the variable server
 * - Controlling data rate
 * - Monitoring connection status
 * - Maintaining a list of available sims
 * - Supporting indirect connection to sims via a sniff server
 */

/** \ingroup libtvs
 * VSClientImpl is used by a data storage mechanism (such as a DataRiver)
 * to handle the underlying connection to the Trick variable server and do
 * variable value parsing. The connection between the data store and
 * VSClientImpl is an intimate one -- in order to add a variable, the data store
 * passes in a list of TrickParams, and VSClientImpl is expected to add to the
 * list when an addition is successful, and maintain a reference to the list
 * so that as values arrive from the variable server, they may be placed 
 * directly into the corresponding TrickParam storage spaces.
 * 
 * In addition to the list of parameters passed in from the data storage
 * client (the "value" parameters), VSClientImpl has another list of "status"
 * parameters that it maintains internally in order to keep its client up to
 * date on the connection status. In fact, VSClientImpl actually supports three 
 * connections to the variable server:
 * - A "value" connection, used to request and receive values
 * - A "status" connection, used to receive status updates
 * - An "add" connection, used only to check whether a parameter is valid
 *   (i.e. whether it exists on the variable server).
 * The rationale for the add connection is that it is important not to slow 
 * down data retrieval and processing on the other two threads, and checking
 * for existence requires four messages to be sent over the network.
 *
 * The value and status connections are each associated with their own 
 * processing thread. Adds, on the other hand, are usually done on the main
 * thread. Connections to the variable server may be either synchronous or
 * asynchronous (see the Trick manual for more information). For TrickVSRiver,
 * the status and add connections are always asynchronous, while the value
 * thread attempts to be synchronous in order to get all the data points. During
 * sim freezes, however, the values thread is set to asynchronous by the status
 * thread in order that user changes to values during freeze may be reflected
 * (synchronous threads do not receive updates during freeze).
 *
 * TrickVSRiver may be connected to a local sim, via a socket, or to a remote
 * sim via TCP/IP, or to a remote sim through a sniff server. Actually, this 
 * information is kept hidden in the VSClientConnector, and TrickVSRiver has
 * no idea whether the connection is local or remote.
 * 
 */
class VSClientImpl {

       public:
               /// Status of an attempted command to the variable server
               enum IOStatus {
                   FAILURE = 0,   ///< Operation failed
                   SUCCESS,       ///< Operation succeeded
                   DONTKNOW       ///< Unknown -- usually due to timeout
               };

               /// Main constructor
               VSClientImpl(VSUserIF* user,  ///< Client to call on updates
                            VSClientConnector* connector, ///< Connector to sim
                            double rate,                  ///< Data rate
                            vector<TrickParam>& params,   ///< Value parameters
                            unsigned int buffsize /**< Amount of storage space
                                                       to reserve per param */
                           );

               /** update handler called when whole packet processed
                * at exit must have ensured that TrickParam addresses 
                * can be overwritten without losing data
                */
               virtual ~VSClientImpl();

               /// Add a parameter to the list
               IOStatus add(string name);

               /// Get initial values for a parameter
               IOStatus get_initial_value(const string& name, double& value, string& units);
 
               /// Add a list of parameters to the list
               void addmultiple(unsigned int numnames, char** names);

               /// Set the data rate
               void setRate(double rate);

               /// Get the data rate
               double getRate() { return _rate; }

               /// Lock data processing
               void lock()   { _handler_mutex.Lock(); }

               /// Unlock data processing
               void unlock() { _handler_mutex.Unlock(); }

               /// Start the data receiving thread
               void startReceive();

               /// Send text to the VS
               int sendText(const char* line);
       private:
               unsigned int _countDirect(const vector<TrickParam>& params);

               /// Get units for a parameter name
               IOStatus _var_units(string name, string& units);

               /// Check if a parameter name exists on the variable server
               IOStatus _var_exists(string name);

               /// Add a parameter to a list of parameters
               IOStatus _add(bool forValue, vector<TrickParam>& params, 
                             const string& name);

               /// List of value parameters
               vector<TrickParam>& _valueparams ;

               /// List of status parameters
               vector<TrickParam>  _statusparams ;

               /// Data rate -- time between data frame in seconds
               double _rate;

               /// Parse incoming data into params
               virtual int _add_data(vector<TrickParam>& params, 
                                     vector<string>& data);

               VSUserIF* _user;   ///< Pointer to our client
               VSState _vsstate;  ///< Current connection state

               bool _keep_running; ///< Synchronizes threads on exit
               bool _shutdown;     /**< True on a ordered shutdown; false on
                                        a shutdown due to disconnection  */
 
               const unsigned int _BUFFSIZE; ///< Space reserved per param
               static const int _TIME_IDX = 0; ///< Index for time param
               
               /// Entry point to value thread
               static void* _static_receive_value(void*);
               /// Entry point to value thread
               void _receive_value();                    
               Thread* _receive_value_thread;   ///< Value thread
               VSCommunicator* _value_client;   ///< Value client connection
               
               /// Entry point to status thread
               static void* _static_receive_status(void*);
               /// Entry point to status thread
               void _receive_status();                    
               Thread* _receive_status_thread;    ///< Status thread
               VSCommunicator* _status_client;   ///< Status client connection
               Mutex _handler_mutex;   ///< Data handler mutex
               Mutex _add_mutex;       ///< Param adding mutex

               VSCommunicator* _add_client;      ///< Add client connection
               bool _receivethreadspawned;       /**< True if value thread has
                                                      been spawned */
               double _timebase;                 /**< Divisor for converting
                                                      raw time to seconds */
			   VSClientConnector* _connector;     ///< Access to connections, params
} ;

#endif

