#include "vscommunicator07.hh"
#include "../libtc/tokenizer.hh"

#include <iostream>
#include <iomanip>
using std::cerr;
using std::endl;

VSCommunicator07::VSCommunicator07(TCClient& client): VSCommunicator(client)
{}

int VSCommunicator07::_send_cmd(const string& msg)
{
    string cmd = msg + "\n";
    int cmd_size = htonl(cmd.size()) ;

    if ( _client.write((char *)&cmd_size, sizeof(int)) <= 0 ) {
//cerr << "Error writing size" << endl;
        return VSCommunicator::COMM_ERR;
    }

    if ( _client.write(cmd.c_str(), cmd.size()) <= 0 ) {
//cerr << "Error writing " << cmd << endl;
        return VSCommunicator::COMM_ERR;
    }
    return VSCommunicator::OK;
}

int VSCommunicator07::setSynchronous(bool synchronous)
{
    char cmd_str[256];
    sprintf (cmd_str, "var_sync = %d ;", synchronous ? 1 : 0);
    return _send_cmd(cmd_str);
}

int VSCommunicator07::pause()
{
    return _send_cmd("var_pause; ");
}

int VSCommunicator07::unpause()
{
    return _send_cmd("var_unpause; ");
}

void VSCommunicator07::exit()
{
    _send_cmd("var_exit; ");
    _client.disconnect();
}

int VSCommunicator07::setRate(double rate)
{
    char msg[1024];
    sprintf(msg, "var_cycle = %lf ; ", rate);
    return _send_cmd(msg);
}

int VSCommunicator07::sendNow()
{
    return _send_cmd("var_pause; var_send; var_unpause;");
}

int VSCommunicator07::add(const string& name)
{
    return _send_cmd(string("var_add ") + name + " ; ");
}

// Existence and addition in one go! 
// Assumes: _client is connected and paused
void VSCommunicator07::getUnits(const string& name, string& units, double& val)
{
    string cmd;

    cmd = string("var_clear; var_add ") + name 
              + " {xx} ; var_send ; var_clear ; ";

    units = ""; // Empty by default until we receive something
    if (_send_cmd(cmd) < 0) return;

    string remainder;
    string buffer = _client.readline(remainder);
    if (buffer.size() > 0) { // Received data
        switch (buffer[0]) {
          case '#':
            for (int ii = 1; ii < 4 && _client.isValid(); ii++) {
                // Skip over error lines
                buffer = _client.readline(remainder);
                if (buffer.size() <= 0) {
                    break;
                }
            }
            break;
          default:
             double myval;
             char myunits[256];
             int scanned = sscanf(buffer.c_str(), "%lf {%[^}]s", &myval, myunits);
             if (scanned == 2) {
                  val = myval;
                  units = myunits;
             }

            break;
        }
    }
}

vector<TrickParam> DirectVSCommunicator07::exists(int numparams, char** paramnames)
{
    string units;
    double value;
    vector<TrickParam> params;

    for (int ii = 0; ii < numparams; ii++) {
        getUnits(paramnames[ii], units, value);
        if (units != "") {
            params.push_back(TrickParam(paramnames[ii], TrickParam::DIRECT, units, value));
        }
    }
    return params;
}

void DirectVSCommunicator07::addMultiple(const vector<TrickParam>& params)
{
    for (unsigned int ii = 0; ii < params.size(); ii++) {
        add(params[ii].name);
    }
}

vector<TrickParam> SniffVSCommunicator07::exists(int numparams, 
                                               char** paramnames)
{
    vector<TrickParam> params;

    string existcmd = SniffSrvCmds::exists();
    for (int ii = 0; ii < numparams; ii++) {
        existcmd += string(" ") + paramnames[ii];
    }
    existcmd += "\n";
    _client.write(existcmd);
    string remainder;
    string valueline = _client.readline(remainder, 60);

    // Should be one token for each var
    vector<string> values = Tokenizer::tokenize(valueline, "\t\n");
    if (values.size() != numparams) {
        return params;  // TODO: figure out something better to return -- maybe
    }

    for (unsigned int ii = 0; ii < values.size(); ii++) {
        double val;
        char myunits[256];
        int scanned = sscanf(values[ii].c_str(), "%lf {%[^}]s", &val, myunits);
        if (values[ii][0] == '#' || scanned != 2) {
            continue;
        } else {
            params.push_back(TrickParam(paramnames[ii], TrickParam::DIRECT, myunits, val));
        }
    }
    return params;
}

void SniffVSCommunicator07::addMultiple(const vector<TrickParam>& params)
{
    string addcmd = SniffSrvCmds::addmultiple();
    if (params.size() == 0) return;

    for (unsigned int ii = 0; ii < params.size(); ii++) {
        addcmd += string(" ") + params[ii].name;
    }

    // Send multiple add cmd
    addcmd += "\n";
    _client.write(addcmd);
}

int VSCommunicator07::readvalues(string& remainder, 
                           unsigned int numValues,
                           vector<string>& values)
{
    string data = _client.readline(remainder);
    vector<string> words = Tokenizer::tokenize(data, "\t");

    if (words.size() > numValues) {
        return VSCommunicator::TOO_MANY_VALUES;
    }

    if (words.size() < numValues) {
        return VSCommunicator::TOO_FEW_VALUES;
    }

    values = words;
    return VSCommunicator::OK;
}

int VSCommunicator07::sendText(const string &text)
{
    // Not supported for Trick 07
    return VSCommunicator::INVALID_PARAM;
}
