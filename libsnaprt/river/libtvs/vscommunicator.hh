#ifndef VSCOMMUNICATOR_HH
#define VSCOMMUNICATOR_HH

#include "../libtc/tcclient.hh"
#include "vsstructs.hh"
#include "sniffsrvcmds.hh"
#include "vsversionif.hh"
#include <vector>
#include <string>
using std::vector;
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

/** \ingroup libtvs
 * Encapsulates a connection to a sim, and the variable server protocol.
 * VSCommunicator comes in direct and remote varieties. Direct communicators
 * are simply pass-thrus for commands. Remote communicators have to communicate
 * with sims indirectly via a sniff server. VSCommunicator hides the syntax
 * of variable server commands from vs clients.
 */
class VSCommunicator: public VSVersionIF {
  public:
    VSCommunicator(TCClient& client);
    virtual ~VSCommunicator();

    virtual int setRate(double rate) = 0;       ///< Send set rate
    virtual int setSynchronous(bool synch) = 0; ///< Send set synchronous
    virtual int pause() = 0;                    ///< Send pause
    virtual int unpause() = 0;                  ///< Send unpause
    virtual void exit() = 0;                    ///< Send exit and disconnect
    virtual int add(const string& name) = 0;    ///< Add parameter
    virtual int sendNow() = 0;                  ///< Get parameter values NOW
    virtual int sendText(const string& text) = 0; ///< Send text to VS

    /// Get units and initial value for a parameter
    virtual void getUnits(const string& name, string& units, double& val) = 0;

    /// Check parameter existence
    virtual vector<TrickParam> exists(int numparams, char** paramnames) = 0;

    /// Add multiple parameters
    virtual void addMultiple(const vector<TrickParam>& params) = 0;

    /// Check connection is valid
    virtual bool isValid() = 0;

    // Set blocking mode
    virtual void setBlockIO(TCComm::Blocking val) = 0;

    // Read a line from the variable server
    virtual string readline(string& remainder) = 0;

    // Read variable values from the variable server
    virtual int readvalues(string& remainder, 
                           unsigned int numValues,
                           vector<string>& values) = 0;
  
    // Whether there is data waiting on the receive socket
    virtual bool hasReceiveData();

    enum {
        INITIAL_VAL     = -999,
        INVALID_PARAM   = -4,
        COMM_ERR        = -3,
        TOO_MANY_VALUES = -2,
        TOO_FEW_VALUES  = -1,
        OK = 0,
    } StatusCodes;

  protected:
// TODO: See if this can be added here instead of in children
//    virtual int _send_cmd(const string& msg);
    TCClient _client;
};
#endif
