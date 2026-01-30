#ifndef TCP_CONNECTION_HH
#define TCP_CONNECTION_HH

/*
    PURPOSE: ( Encapsulate a TCP connection. )
*/

#include "trick/ClientConnection.hh"
#include "trick/SystemInterface.hh"

namespace Trick
{

class TCPConnection : public ClientConnection
{
public:
    TCPConnection();
    TCPConnection(int listen_socket);
    TCPConnection(SystemInterface * system_interface);
    TCPConnection(int listen_socket, SystemInterface * system_interface);

    virtual int start() override;

    virtual int write(const std::string & message) override;
    virtual int write(char * message, int size) override;

    virtual int read(std::string & message, int max_len = MAX_CMD_LEN) override;

    virtual int disconnect() override;
    virtual bool isInitialized() override;

    virtual int setBlockMode(bool blocking) override;

    virtual int restart() override;

    virtual std::string getClientTag() override;
    virtual int setClientTag(std::string tag) override;

    virtual std::string getClientHostname() override;
    virtual int getClientPort() override;

private:
    int _socket;
    bool _connected;

    // This is needed to be able to accept an incoming connection, after being set up by the listener
    int _listen_socket;

    SystemInterface * _system_interface; /* ** */
};
} // namespace Trick

#endif
