#include <iostream>
#include "vsclientimpl.hh"
#include "vsstructs.hh"
class VSClientConnector;

/** \ingroup libtvs
 * Minimal variable server client, useful for testing and as a basis
 * for standalone implementations. 
 */
class VSClient: VSUserIF {
  public:
    /** Constructor */
    VSClient( void (*handler)(VSMessage& s), ///< Handler for value updates
              VSClientConnector* connector,  ///< Connector to sim
              double rate,                   ///< Data rate
              vector<TrickParam>& values     ///< Storage for parameter data
            );

    /// Add a parameter to the list
    VSClientImpl::IOStatus add(string name); 

    /// Start receiving data
    void start() { _vsclient.startReceive(); } 

    /// Handler for update values
    void handler(VSMessage& s);

  private:
    void (*_handler)(VSMessage&); ///< Stores the client handler to call
    VSClientImpl _vsclient;       /**< Handles underlying connection and
                                       data processing */
};
