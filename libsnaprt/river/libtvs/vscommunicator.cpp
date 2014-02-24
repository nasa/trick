#include "vscommunicator.hh"
#include "../libtc/tokenizer.hh"

VSCommunicator::VSCommunicator(TCClient& client): _client(client),
                                                  VSVersionIF(client.version())
{}

bool VSCommunicator::hasReceiveData()
{
    /*
     * Client will be in one of three states:
     * 1) Invalid, in which case isPending returns -1
     * 2) Valid, but with no data, in which case isPending returns 0
     * 3) Valid, with data, in which case isPending returns the number of bytes
     *    available on the socket.
     */
    return (_client.isPending() > 0);
}

VSCommunicator::~VSCommunicator() 
{}

// Raw send of message
/*
int VSCommunicator::_send_cmd(const string& msg)
{
    string cmd = msg + "\n";
    if ( _client.write(cmd.c_str(), cmd.size()) <= 0 ) {
        return -1;
    }
    return 0;
}
*/
