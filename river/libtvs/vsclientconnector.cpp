#include <string>
#include <sstream>
#include <iostream>
using std::stringstream;
using std::string;
using std::cerr;
using std::endl;

#include "vsclientconnector.hh"
#include "sniffsrvcmds.hh"
#include "vscommunicator.hh"
#include "vscommunicator07.hh"
#include "vscommunicator10.hh"
#include "vsclientimpl.hh"
#include "../libutils/exception.hh"
#include "../libtc/tcclient.hh"

VSClientConnector::VSClientConnector(const HostSpec& spec): 
                                                    VSVersionIF(spec.version()),
                                                    _spec(spec)
{}

VSClientConnector::~VSClientConnector()
{}

string VSClientConnector::toString()
{
//    stringstream s;
//    s << _host << ":" << _port;
//    return s.str();
    return _spec.spec();
}

VSTrick07Connector::VSTrick07Connector(const HostSpec& spec):
                                           VSClientConnector(spec)
{}

VSTrick10Connector::VSTrick10Connector(const HostSpec& spec):
                                           VSClientConnector(spec)
{}

VSCommunicator* VSTrick07Connector::newClient()
{
    TCClient client(_spec);
	client.setHandshakeDisabled(getHandshakeDisabled());
    if (client.connect() != TC_SUCCESS) {
        throw Exception("Unable to make a connection to the sim");
    }
    return new VSCommunicator07(client);
}

VSCommunicator* VSTrick10Connector::newClient()
{
    TCClient client(_spec);
	client.setHandshakeDisabled(getHandshakeDisabled());
    if (client.connect() != TC_SUCCESS) {
        throw Exception("Unable to make a connection to the sim");
    }
    return new VSCommunicator10(client);
}

VSClientImpl* VSTrick07Connector::getVSClient( VSUserIF* user,
                                                double rate,
                                                vector<TrickParam>& params,
                                                unsigned int buffsize )
{
    return new VSClientImpl(user, this, rate, params, buffsize);
}

VSClientImpl* VSTrick10Connector::getVSClient( VSUserIF* user,
                                                double rate,
                                                vector<TrickParam>& params,
                                                unsigned int buffsize )
{
    return new VSClientImpl(user, this, rate, params, buffsize);
}


string VSTrick07Connector::getTimeBaseParam()
{
	return "";
}

string VSTrick10Connector::getTimeBaseParam()
{
	return "trick_sys.sched.time_tic_value";
}

string VSTrick07Connector::getTimeParam()
{
	return "sys.exec.out.time";
}

string VSTrick10Connector::getTimeParam()
{
	return "trick_sys.sched.time_tics";
}

string VSTrick07Connector::getModeParam()
{
	return "sys.exec.work.mode";
}

string VSTrick10Connector::getModeParam()
{
	return "trick_sys.sched.mode";
}

TCComm::Flag VSTrick07Connector::getHandshakeDisabled()
{
	return TCComm::TC_COMM_FALSE;
}

TCComm::Flag VSTrick10Connector::getHandshakeDisabled()
{
	return TCComm::TC_COMM_TRUE;
}


VSClientConnector* VSClientFactory::getConnector(const HostSpec& spec) 
{
    if (spec.isValid()) {
		switch (spec.version()) {
		  case HostSpec::TRICK07:
			return new VSTrick07Connector(spec);
		  case HostSpec::TRICK1X:
			return new VSTrick10Connector(spec);
          default:
            return NULL;
        }
    } else {
        return NULL;
    }
}
