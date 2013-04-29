#ifndef VSCOMMUNICATOR07_HH
#define VSCOMMUNICATOR07_HH

#include "../libtc/tcclient.hh"
#include "vsstructs.hh"
#include "sniffsrvcmds.hh"
#include "vscommunicator.hh"
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
class VSCommunicator07: public VSCommunicator {
  public:
    /// Build a communicator from a connection
    VSCommunicator07(TCClient& client);
    virtual ~VSCommunicator07() {}
    int setRate(double rate);             ///< Send set rate
    int setSynchronous(bool synchronous); ///< Send set synchronous
    int pause();                          ///< Send pause
    int unpause();                        ///< Send unpause
    void exit();                          ///< Send exit and disconnect
    int add(const string& name);          ///< Add parameter
    int sendNow();                        ///< Get parameter values update NOW
    int sendText(const string& text);     ///< Send text to VS

    /// Get units and initial value for a parameter
    void getUnits(const string& name, string& units, double& val);

    /// Check parameter existence
    virtual vector<TrickParam> exists(int numparams, char** paramnames) //= 0;
    { cerr << "PURE EXISTS" << endl; return vector<TrickParam>();}
    /// Add multiple parameters
    virtual void addMultiple(const vector<TrickParam>& params) //= 0;
    { cerr << "PURE ADDMULTIPLE" << endl; }

    /// Check connection is valid
    bool isValid() { return _client.isValid(); }

    // Set blocking mode
    void setBlockIO(TCComm::Blocking val) { _client.setBlockIO(val); }

    // Read a line from the variable server
    string readline(string& remainder) { return _client.readline(remainder); }

    // Read variable values from the variable server
    virtual int readvalues(string& remainder, 
                           unsigned int numValues,
                           vector<string>& values);
  
  protected:
    int _send_cmd(const string& cmd); ///< Send a command to the variable server
};

/** \ingroup libtvs
 * Implements VSCommunicator for direct access variable servers */
class DirectVSCommunicator07 : public VSCommunicator07 {
  public:
    // Communicator interface
    DirectVSCommunicator07(TCClient& client): VSCommunicator07(client) {}

    /// Check parameter existence
    virtual vector<TrickParam> exists(int numparams, char** paramnames);

    /// Add multiple parameters
    virtual void addMultiple(const vector<TrickParam>& params);
};

/** \ingroup libtvs
 * Implements VSCommunicator for access to variable servers via sniff srv */
class SniffVSCommunicator07 : public VSCommunicator07 {
  public:
    // Communicator interface
    SniffVSCommunicator07(TCClient& client): VSCommunicator07(client) {}

    /// Check parameter existence
    virtual vector<TrickParam> exists(int numparams, char** paramnames);

    /// Add multiple parameters
    virtual void addMultiple(const vector<TrickParam>& params);
};

#endif
