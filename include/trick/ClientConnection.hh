/************************************************************************
PURPOSE: (Abstract base class for a connection to a client. Should
be inherited by variable server and web server connections. )
LIBRARY DEPENDENCIES:
    () )
**************************************************************************/

#ifndef CLIENT_CONNECTION_HH
#define CLIENT_CONNECTION_HH

#include <string>

namespace Trick
{
class ClientConnection
{
public:
    virtual ~ClientConnection() {}

    static const unsigned int MAX_CMD_LEN = 200000;

    enum ConnectionType
    {
        TCP,
        UDP,
        MCAST,
        WS
    };

    // Pure virtual methods
    virtual int start() = 0;

    virtual int write(const std::string & message) = 0;
    virtual int write(char * message, int size) = 0;

    virtual int read(std::string & message, int max_len = MAX_CMD_LEN) = 0;

    virtual int setBlockMode(bool blocking) = 0;
    virtual int disconnect() = 0;

    virtual bool isInitialized() = 0;

    virtual std::string getClientTag() = 0;
    virtual int setClientTag(std::string tag) = 0;

    virtual int restart() = 0;

    virtual std::string getClientHostname() = 0;
    virtual int getClientPort() = 0;

protected:
    ConnectionType _connection_type;
    std::string _client_tag;
};
} // namespace Trick

#endif
