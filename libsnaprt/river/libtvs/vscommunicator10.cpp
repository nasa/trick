#include "vscommunicator10.hh"
#include "../libtc/tokenizer.hh"
#include "../libutils/exception.hh"
#include <iostream>
using std::endl;
using std::cerr;
using std::hex;
using std::dec;

VSCommunicator10::VSCommunicator10(TCClient& client): VSCommunicator(client)
{}

int VSCommunicator10::_send_cmd(const string& msg)
{
    string cmd = msg + "\n";
    if ( _client.write(cmd.c_str(), cmd.size()) <= 0 ) {
        return VSCommunicator::COMM_ERR;
    }
    return VSCommunicator::OK;
}

int VSCommunicator10::setSynchronous(bool synchronous)
{
    char cmd_str[256];
    sprintf (cmd_str, "trick.sim_services.var_sync(%d)", 
               synchronous ? VSCommunicator10::VSCOMM_LOCKSTEP 
                           : VSCommunicator10::VSCOMM_ASYNC);
    return _send_cmd(cmd_str);
}

int VSCommunicator10::pause()
{
    return _send_cmd("trick.sim_services.var_pause()");
}

int VSCommunicator10::unpause()
{
    return _send_cmd("trick.sim_services.var_unpause()");
}

void VSCommunicator10::exit()
{
    _send_cmd("trick.sim_services.var_exit()");
    _client.disconnect();
}

int VSCommunicator10::setRate(double rate)
{
    char msg[1024];
    sprintf(msg, "trick.sim_services.var_cycle(%lf)", rate);
    return _send_cmd(msg);
}

int VSCommunicator10::sendNow()
{
    return _send_cmd("trick.sim_services.var_pause()\n"
                     "trick.sim_services.var_send()\n"
                     "trick.sim_services.var_unpause");
}

int VSCommunicator10::add(const string& name)
{
    return _send_cmd(string("trick.sim_services.var_add(\"") + name + "\")");
}

// Existence and addition in one go! 
// Assumes: _client is connected and paused
void VSCommunicator10::getUnits(const string& name, string& units, double& val)
{
    string cmd;
    char tmpunits[255];

    cmd = string("trick.sim_services.var_clear()\n")
          + string("trick.sim_services.var_add(\"") + name + string("\")\n")
          + string("trick.sim_services.var_units(\"") + name + "\", \"xx\")\n"
          + string("trick.sim_services.var_send()\n")
          + string("trick.sim_services.var_clear()");

    units = ""; // Empty by default until we receive something
    if (_send_cmd(cmd) < 0) return;

    string remainder;
    string buffer = _client.readline(remainder);

    if (buffer.size() > 0) { // Received data
        vector<string> words = Tokenizer::tokenize(buffer, "\t\n");
        double myval;
        if (words.size() >= 1 && words[0] == "0") {
            // A non-existent name will return "BADREF" in the value field
            int scanned = sscanf(words[1].c_str(), "%lf {%[^}]s", 
                                 &myval, tmpunits);
            if (scanned == 2) {
                val = myval;
                units = tmpunits;
            }
        }
    }
/*
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
             int scanned = sscanf(buffer.c_str(), "%lf", &myval);
             if (scanned == 1) {
                  val = myval;
//                  throw Exception("Sim does not return unit information");
units = "--";
             }

            break;
        }
    }
*/
}

vector<TrickParam> VSCommunicator10::exists(int numparams, char** paramnames)
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

void VSCommunicator10::addMultiple(const vector<TrickParam>& params)
{
    for (unsigned int ii = 0; ii < params.size(); ii++) {
        add(params[ii].name);
    }
}

int VSCommunicator10::readvalues(string& remainder,
                                       unsigned int numValues,
                                       vector<string>& values)
{
    string data = _client.readline(remainder);
    vector<string> words = Tokenizer::tokenize(data, "\t");

    if (words.size()-1 > numValues) {
        return VSCommunicator::TOO_MANY_VALUES;
    }

    if (words.size()-1 < numValues) {
        cerr << "Received " << words.size()-1 << " values; expected " 
             << numValues << endl;
        return VSCommunicator::TOO_FEW_VALUES;
    }

    values.erase(values.begin(), values.end());
    for (unsigned int ii = 1; ii < words.size(); ii++) {
        values.push_back(words[ii]);
    }
    return VSCommunicator::OK;
}

int VSCommunicator10::sendText(const string &text)
{
    // Sending var_send or var_exists is not supported
    if (text.find("trick.sim_services.var_send") != string::npos
            || text.find("trick.sim_services.var_exists") != string::npos ) {
        return VSCommunicator::INVALID_PARAM;
    }

    // Trick 10 does not provide any status on commands through the
    // variable server, so the best we can do is send and return the
    // comm status.
    return _send_cmd(text);
}
