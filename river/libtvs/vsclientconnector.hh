#ifndef vsclientconnector_hh
#define vsclientconnector_hh

#include <string>
#include "vsversionif.hh"
#include "vsstructs.hh"
#include "../libtc/hostspec.hh"
#include "../libtc/tcclient.hh"

#include <vector>
#include <iostream>
using std::cerr;
using std::endl;

class VSClientImpl;
class VSCommunicator;
class VSUserIF;

/** \ingroup libtvs
 * Provides communication clients to talk to a Trick variable server.
 * This class is an abstract parent class from which is derived various
 * different types of connections. The primary purpose of a VSClientConnector
 * is to provide VSCommunicator instances already set up to talk to the 
 * variable server. VSClientConnector is thus a factory of sorts.
 */
class VSClientConnector: public VSVersionIF {
  public:
    VSClientConnector(const HostSpec& spec);
	virtual ~VSClientConnector();
	virtual VSCommunicator* newClient() = 0;      ///< Get a new comm client
	virtual VSClientImpl* getVSClient( VSUserIF* user,
									   double rate,
									   std::vector<TrickParam>& params,
									   unsigned int buffsize ) = 0; ///< Get a variable server client
    virtual std::string toString();           ///< Return the current hostspec
	virtual std::string getTimeParam() = 0;
	virtual std::string getTimeBaseParam() = 0;
	virtual std::string getModeParam() = 0;
	virtual TCComm::Flag getHandshakeDisabled() = 0;

  protected:
    HostSpec _spec; ///< Host/port spec
};

/** \ingroup libtvs
 * Creates communication clients for a Trick variable server that is 
 * directly accessible via a host and port.
 */ 
class VSTrick07Connector: public VSClientConnector {
  public:
    VSTrick07Connector(const HostSpec& spec);
	~VSTrick07Connector() {}
    virtual VSCommunicator* newClient();  ///< Get a new comm client
	virtual VSClientImpl* getVSClient( VSUserIF* user,
									   double rate,
									   std::vector<TrickParam>& params,
									   unsigned int buffsize ); ///< Get a variable server client
	virtual std::string getTimeParam();
	virtual std::string getTimeBaseParam();
	virtual std::string getModeParam();
	virtual TCComm::Flag getHandshakeDisabled();
};

/** \ingroup libtvs
 * Creates communication clients for a Trick variable server that is 
 * directly accessible via a host and port.
 */ 
class VSTrick10Connector: public VSClientConnector {
  public:
    VSTrick10Connector(const HostSpec& spec);
	~VSTrick10Connector() {}
    virtual VSCommunicator* newClient();  ///< Get a new comm client
	virtual VSClientImpl* getVSClient( VSUserIF* user,
									   double rate,
									   std::vector<TrickParam>& params,
									   unsigned int buffsize ); ///< Get a variable server client
	virtual std::string getTimeParam();
	virtual std::string getTimeBaseParam();
	virtual std::string getModeParam();
	virtual TCComm::Flag getHandshakeDisabled();
};

/** Factory class for building VSClientConnectors */
class VSClientFactory {
  public:
    VSClientFactory() {}
    /// Build from a HostSpec (could be direct, or remote)
    static VSClientConnector* getConnector(const HostSpec& spec); 
};
#endif
