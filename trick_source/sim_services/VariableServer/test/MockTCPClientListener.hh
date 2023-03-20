#ifndef MOCK_CLIENT_LISTENER_HH
#define MOCK_CLIENT_LISTENER_HH

#include "trick/TCPClientListener.hh"

#include <gmock/gmock.h>

class MockTCPClientListener : public Trick::TCPClientListener {
    public:
    MOCK_METHOD2(initialize, int(std::string, int));
    MOCK_METHOD0(initialize, int()); 
    MOCK_METHOD1(setBlockMode, int(bool)); 
    MOCK_METHOD0(checkForNewConnections, bool()); 
    MOCK_METHOD0(setUpNewConnection, Trick::TCPConnection*()); 
    MOCK_METHOD0(disconnect, int()); 
    MOCK_METHOD0(checkSocket, int()); 
    MOCK_METHOD1(validateSourceAddress, bool(std::string)); 
    MOCK_METHOD0(isInitialized, bool()); 
    MOCK_METHOD0(restart, int()); 
    MOCK_METHOD0(getHostname, std::string()); 
    MOCK_METHOD0(getPort, int()); 
};

#endif