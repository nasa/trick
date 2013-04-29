#include "sniffsrvcmds.hh"
#include <string>
#include <sstream>
#include <iostream>
using std::string;
using std::stringstream;
using std::cerr;
using std::endl;

const string SniffSrvCmds::CMDDELIM(":::");
const string SniffSrvCmds::FAILUREDELIM("#");

// Detach command
string SniffSrvCmds::detach()
{
    return CMDDELIM + "detach\n";
}

// Route name only
string SniffSrvCmds::route()
{
    return CMDDELIM + "route";
}

// Exists name only
string SniffSrvCmds::exists()
{
    return CMDDELIM + "exists";
}

// Add multiple name only
string SniffSrvCmds::addmultiple()
{
    return CMDDELIM + "addmultiple";
}

// Route instantiated command
string SniffSrvCmds::route(const string& host, const int port)
{
    stringstream s;
    s << CMDDELIM << "route " << host << " " << port << endl;
    return s.str();
}

// Connect name only
string SniffSrvCmds::connect()
{
    return CMDDELIM + "connect";
}

// Connect instantiated command
string SniffSrvCmds::connect(const string& host, const int port)
{
    stringstream s;
    s << CMDDELIM << "connect " << host << " " << port << endl;
    return s.str();
}

string SniffSrvCmds::disconnect()
{
    return CMDDELIM + "disconnect\n";
}

string SniffSrvCmds::listonce()
{
    return CMDDELIM + "listonce\n";
}

string SniffSrvCmds::listperiodic()
{
    return CMDDELIM + "listperiodic\n";
}

string SniffSrvCmds::listremove()
{
    return CMDDELIM + "listremove\n";
}

string SniffSrvCmds::success()
{
    return CMDDELIM + "success\n";
}

string SniffSrvCmds::failure(const string& msg)
{
    return FAILUREDELIM + msg + "\n";
}

bool SniffSrvCmds::isCmd(const std::string& buffer)
{
    return (buffer.substr(0, CMDDELIM.size()) == SniffSrvCmds::CMDDELIM);
}
