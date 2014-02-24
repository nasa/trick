#ifndef tcpassthru_hh
#define tcpassthru_hh

#include <string>
#include "../libtc/tcclient.hh"
#include "../libtc/tcserver.hh"
#include "../libutils/mutex.hh"
#include "../libutils/thread.hh"

/** \ingroup libtvs
 * This class allows communication between a SIM and a client, and provides
 * mutex-protected writes to allow other threads to write to the SIM or 
 * client safely.
 * Initially, it is assumed that we already have a client up and running, but
 * not necessarily a SIM that we're connected to.
 * The SIM is, for us, a TCClient (actually, we're a client of the SIM).
 * The client is on the other end of a TCServer socket, and so appears to us
 * as a TCServer instance.
 */

class VSPassThru {
  public:
    VSPassThru(TCServer& simclient);
    void setSim(TCClient* sim);  /**< Set SIM. It is assumed that the SIM
                                      is already connected. */
    void disconnectSim();        ///< Disconnect sim only
    void disconnect();           ///< Disconnect sim and client
    int writeSim(const std::string& msg); ///< Write msg to sim
    int writeSim(const char* msg, const int size); ///< Write msg to sim
    int writeSimClient(const std::string& msg); ///< Write msg to client
    bool isValid();              ///< Checks sim and client connection are ok
    static void* _listen_static(void*); ///< Listen for msgs from the sim
    void _listen(void);                 ///< Listen for msgs from the sim

  private:
    TCClient* _sim;               ///< Connection to the sim
    TCServer&_simclient;          ///< Connection to the client
    Thread _listenthread;         ///< Listener thread
    Mutex _writemutex;            ///< Mutex to prevent conflicting writes
};
#endif
