#ifndef VSCOMMUNICATOR10_HH
#define VSCOMMUNICATOR10_HH

#include "../libtc/tcclient.hh"
#include "vsstructs.hh"
#include "sniffsrvcmds.hh"
#include "vscommunicator.hh"
#include <vector>
#include <string>
using std::vector;
using std::string;

/** \ingroup libtvs
 * Encapsulates a connection to a sim, and the variable server protocol.
 * VSCommunicator comes in direct and remote varieties. Direct communicators
 * are simply pass-thrus for commands. Remote communicators have to communicate
 * with sims indirectly via a sniff server. VSCommunicator hides the syntax
 * of variable server commands from vs clients.
 */
class VSCommunicator10: public VSCommunicator {
  public:
    /// Build a communicator from a connection
    VSCommunicator10(TCClient& client);
    virtual ~VSCommunicator10() {}
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
	virtual vector<TrickParam> exists(int numparams, char** paramnames);

    /// Add multiple parameters
	virtual void addMultiple(const vector<TrickParam>& params);

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

    enum Trick_Sync {
      VSCOMM_ASYNC = 0,   /* Tell the Trick VS not to synchronize variables
                             before sending updates to simdata. This results in
                             non-homogeneous data, but is necessary during 
                             freezes when synchronized data is not sent. */
      VSCOMM_BESTEFF = 1, /* Tell the Trick VS to synchronize variables and send
                             data as it finds time. Not recommended for simdata,
                             since this can result in data not being received in
                             a timely manner, and even disconnects due to 
                             partial sends or network timeouts. */
      VSCOMM_LOCKSTEP = 2 /* Tell the Trick VS to synchronize variables and 
                             force the Trick main thread to wait until the data 
                             has been sent. This places extra work on the sim 
                             main thread, but keeps the sim and simdata in sync.
                           */
    };
};

#endif
