#ifndef MOCK_CLIENT_CONNECTION_HH
#define MOCK_CLIENT_CONNECTION_HH

#include "trick/ClientConnection.hh"
#include <gmock/gmock.h>

class MockClientConnection : public Trick::ClientConnection {
    public: 
        MOCK_METHOD0(start, int());
        MOCK_METHOD1(write, int(const std::string& message));
        MOCK_METHOD2(write, int(char * message, int size));
        MOCK_METHOD2(read, int(std::string& message, int max_len)); 
        MOCK_METHOD0(disconnect, int()); 
        MOCK_METHOD1(setBlockMode, int(bool blocking)); 
        MOCK_METHOD0(isInitialized, bool()); 
        MOCK_METHOD0(restart, int());
        MOCK_METHOD0(getClientTag, std::string());
        MOCK_METHOD1(setClientTag, int(std::string tag));
        MOCK_METHOD0(getClientHostname, std::string());
        MOCK_METHOD0(getClientPort, int());

};

#endif