#include <iostream>
#include "vsclient.hh"
#include "vsclientimpl.hh"
#include "vsclientconnector.hh"

VSClient::VSClient( void (*handler)(VSMessage& msg),
                    VSClientConnector* connector,
                    double rate,
                    vector<TrickParam>& params):
                      _handler(handler),
                      _vsclient(this, connector, rate, params, 1)
{

}

VSClientImpl::IOStatus VSClient::add(string name)
{
    return _vsclient.add(name);
}

void VSClient::handler(VSMessage& msg)
{
    _handler(msg);
}
