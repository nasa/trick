#include <string>
#include <vector>
using std::string;
using std::vector;

#include "vspassthru.hh"
#include "sniffsrvcmds.hh"
#include "../libtc/tokenizer.hh"
#include "../libutils/autolock.hh"

VSPassThru::VSPassThru(TCServer& simclient): 
                             _sim(NULL),
                             _simclient(simclient)
{}

void VSPassThru::setSim(TCClient* sim)
{
    // If we're already connected to something, disconnect
    disconnectSim();
    _sim = sim;
    _listenthread.Start(_listen_static, this);
}

void VSPassThru::disconnectSim()
{
    if (_sim) {
        { // Block for AutoLock
            AutoLock locker(&_writemutex);
            _sim->disconnect();
        }

        // wait for listen thread to end
        _listenthread.Join();
        _sim = NULL;
    }
}

void VSPassThru::disconnect()
{
    disconnectSim();
    _simclient.disconnect();
}

int VSPassThru::writeSim(const string& msg)
{
    return writeSim(msg.c_str(), msg.size());
}

int VSPassThru::writeSim(const char* msg, const int size)
{
    // Should it be an error if we are asked to write and _sim is NULL? 
    // It's convenient to allow writeSim to do nothing if the sim is not yet
    // connected.
    int ret = size;
    AutoLock locker(&_writemutex);
    if (_sim && _sim->isValid()) ret = _sim->write(msg, size);
    return ret;
}

int VSPassThru::writeSimClient(const string& msg)
{
    int ret = msg.size();
    AutoLock locker(&_writemutex);
    if (_simclient.isValid()) ret = _simclient.write(msg);
    return ret;
}

bool VSPassThru::isValid()
{
    bool result = true;
    AutoLock locker(&_writemutex);
    if (!_sim) {
        result = false;
    } else {
        result = _sim->isValid() && _simclient.isValid();
    }
    return result;
}

void* VSPassThru::_listen_static(void* tcpassthru)
{
    VSPassThru* tp = (VSPassThru*) tcpassthru;
    tp->_listen();
    return NULL;
}

// Messages from the sim are immediately forwarded to the sim client
void VSPassThru::_listen(void)
{
    string buffer;
    string remainder;

    while (_sim->isValid()) {
        buffer = _sim->readline(remainder);
        if (!_sim || !_sim->isValid()) break;

        if (buffer != "") {
            // If we got a disconnect command, give up
            if (SniffSrvCmds::isCmd(buffer) 
                && buffer == SniffSrvCmds::disconnect()) break;
            int byteswritten = writeSimClient(buffer);
            if (byteswritten != buffer.size()) {
                cerr << "* * * * * WARNING: Partial Packet Written: "
                     << byteswritten << " bytes of " << buffer.size() << endl;
            }
        }
    }

    // Write remaining packets
    if (remainder != "") {
        vector<string> packets = Tokenizer::tokenize(remainder, "\n");
        for (unsigned int ii = 0; ii < packets.size(); ii++) {
            writeSimClient(packets[ii]);
        }
    }
    { // Block for AutoLock
        AutoLock locker(&_writemutex);
        if (_sim) _sim->disconnect();
        _sim = NULL;
    }
}
