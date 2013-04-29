#include "../../libtc/tcclient.hh"
#include "../../libtc/tcserver.hh"
#include "../../libtc/tcservermgr.hh"
#include "../../libutils/exception.hh"
#include "../../libtc/tokenizer.hh"
#include "../../libutils/autolock.hh"
#include "../../libutils/thread.hh"
#include "../../libutils/convutils.hh"
#include "../vspassthru.hh"
#include "../sniffsrvcmds.hh"
#include "../sniffer.hh"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <cstdlib>
#include <cstring>
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::list;
using std::stringstream;

static Sniffer _sniffer;
static list<TCServer*> _servers;
static Mutex srvmutex;

int send_cmd(TCClient& client, string cmd)
{
    cmd += "\n";
    int cmd_size = htonl(cmd.size()) ;

    if ( client.write((char *)&cmd_size, sizeof(int)) <= 0 ) {
        fprintf(stderr, "send_cmd(): Failed writing size information.\n");
        return -1;
    }

    if ( client.write(cmd.c_str(), cmd.size()) <= 0 ) {
        fprintf(stderr, "send_cmd(): Failed writing primary data.\n");
        return -1;
    }
    return 0;
}

// Add multiple variables. Nothing is returned here. It is assumed that any errors have 
// already been found by using exists()
void addmultiple(TCClient& client, vector<string> vars)
{
    // Now actually do the adds
    string cmd;
    for (unsigned int ii = 1; ii < vars.size(); ii++) {
        cmd = string("var_add ") + vars[ii] + ";" ;
        send_cmd(client, cmd.c_str());
    }
}

string exists(TCClient& client, vector<string> vars)
{
    string remainder;
    string buffer;
    string ret;

    string querycmd;
    send_cmd(client, string("var_pause;"));
    // Set rate low since this determines how long it takes to process
    // commands
    send_cmd(client, string("var_cycle = 0.0001;"));

    // Skip the first entry -- it's the command name
    for (unsigned int ii = 1; ii < vars.size(); ii++) {
        querycmd = "var_clear; var_add " + vars[ii] + " {xx}; var_send; ";

        send_cmd(client, querycmd.c_str());
        buffer = client.readline(remainder);

        if (buffer.size() > 0) { // Received data
            switch (buffer[0]) {
              case '#':
                ret += "#\t";
                while (buffer.size() > 0 && buffer.substr(0,14) != "#### Last read") {
                    // Skip over error lines
                    buffer = client.readline(remainder);
                }
                // Skip final line of error report
                if (buffer.size() > 0) {
                    buffer = client.readline(remainder);
                }
                break;

              default:
                double val;
                char myunits[256];
                int scanned = sscanf(buffer.c_str(), "%lf {%[^}]s", &val, myunits);
                if (scanned != 2) {
                    ret += "#\t";
                } else {
                    stringstream strm(buffer);
                    string tmpvalue;
                    strm >> tmpvalue;
                    ret += tmpvalue + " {" + myunits + "}\t";
                }
                break;
            }
        } else {
            ret += "#\t";
        }
    }
    ret += "\n";
    return ret;
}

string getservers(Sniffer& sniffer)
{
     stringstream msgstr;
//     msgstr << sniffer.size() << endl << _sniffer.get_serialized_list();
     msgstr << _sniffer.get_serialized_list();
     return msgstr.str();
}

void* sendsims(void*)
{
     static unsigned int numclients = 0;
     while (1) {
         { // Block for autolock
             AutoLock locker(&srvmutex);
             list<TCServer*>::iterator iter = _servers.begin();

             while (iter != _servers.end()) {
                 if (! (*iter)->isValid()) {
                     iter = _servers.erase(iter);
                     continue;
                 }
                 (*iter)->write(getservers(_sniffer));
                 iter++;
             }

             // Output message if num clients changes
             if (_servers.size() != numclients) {
                 numclients = _servers.size();
                 cerr << "Sent data on " << _sniffer.size() << " sims" 
                      << " to " << _servers.size() << " clients" << endl;
             }
         }
         Utils::sleep(5000000); // 5 second sleep
     }
     return NULL;
}

void* theserver(void* server)
{
    string buffer;
    string remainder;
    TCServer* srv = (TCServer*) server;
    TCClient* sim = NULL;
    VSPassThru passthru(*srv);
    bool routing = false;

    while (srv->isValid()) {
        buffer = srv->readline(remainder);
        if (buffer == "") continue;

        // Routing -- will be a pass-thru provided the connection is still good
        if (routing) {
            if (passthru.isValid()) {
                passthru.writeSim(buffer);
                continue;
            // Connection went down -- clear up
            } else {
                delete sim;
                sim = NULL;
                routing = false;
            }
        }

        if (!SniffSrvCmds::isCmd(buffer)) {
            if (sim) {
                // Pass buffer to var server/router
                passthru.writeSim(buffer);
                continue;
            } else {
                passthru.writeSimClient( SniffSrvCmds::failure("Not connected to a sim"));
                continue;
            }
        }

        // One shot list of sims
        if (buffer == SniffSrvCmds::listonce()) {
            passthru.writeSimClient(getservers(_sniffer));
            continue;
        }

        // Periodcally updated list of sims
        if (buffer == SniffSrvCmds::listperiodic()) {
            { // Block for AutoLock
                AutoLock locker(&srvmutex);
                _servers.push_back(srv);
            }
            passthru.writeSimClient(SniffSrvCmds::success());
            continue;
        }

        // Stop receiving updates
        if (buffer == SniffSrvCmds::listremove()) {
            { // Block for AutoLock
                AutoLock locker(&srvmutex);
                _servers.remove(srv);
            }
            passthru.writeSimClient(SniffSrvCmds::success());
            continue;
        }

        // Route to host
        string routecmd = SniffSrvCmds::route();
        if (buffer.substr(0, routecmd.size()) == routecmd) {
            // Parse out host and port, initialize connection
            string host;
            int port;
            vector<string> words = Tokenizer::tokenize(buffer, " \n");
            if (words.size() == 3) {
                stringstream strm;
                host = words[1];
                strm << words[2];
                strm >> port;
            } else {
                passthru.writeSimClient( SniffSrvCmds::failure(
                         "Invalid number of parameters in routing request"));
                continue;
            }

            if (sim) delete sim;
// TODO: Fix this up better
            sim = new TCClient(HostSpec(host, port, "07"));
            int ret = sim->connect();
            if (ret == TC_SUCCESS) {
                passthru.setSim(sim);
                routing = true;
                passthru.writeSimClient(SniffSrvCmds::success());
            } else {
                passthru.writeSimClient(
                          SniffSrvCmds::failure("Cannot connect to router"));
                delete sim;
                sim = NULL;
            }
            continue;
        }

        // Detach from routing -- shutdown connection
        if (buffer == SniffSrvCmds::detach()) {
            // Detach is always succesful ... I hope
            passthru.writeSimClient(SniffSrvCmds::success());
            passthru.writeSimClient(SniffSrvCmds::disconnect());
            break;
        }

        // Connect to host
        string connectcmd = SniffSrvCmds::connect();
        if (buffer.substr(0, connectcmd.size()) == connectcmd) {
            // Parse out host and port, initialize connection
            string host;
            int port;
            vector<string> words = Tokenizer::tokenize(buffer, " \n");
            if (words.size() == 3) {
                stringstream strm;
                host = words[1];
                strm << words[2];
                strm >> port;
            } else {
                passthru.writeSimClient( SniffSrvCmds::failure(
                         "Invalid number of parameters in connection request"));
                continue;
            }

            if (sim) delete sim;
// TODO: Fix this better
            sim = new TCClient(HostSpec(host, port, "07"));

            int ret = sim->connect();
            if (ret == TC_SUCCESS) {
                passthru.setSim(sim);
                passthru.writeSimClient(SniffSrvCmds::success());
            } else {
                passthru.writeSimClient(
                          SniffSrvCmds::failure("Cannot connect"));
                delete sim;
                sim = NULL;
            }
            continue;
        }

        // Disconnect from sim
        if (buffer == SniffSrvCmds::disconnect()) {
            if (sim) {
                passthru.disconnectSim();
                delete sim;
                sim = NULL;
            }
            // If we were never connected to a sim, still consider disconnect
            // successful
            passthru.writeSimClient(SniffSrvCmds::success());
            continue;
        }

        // Check for existence of multiple vars
        string existscmd = SniffSrvCmds::exists();
        if (buffer.substr(0, existscmd.size()) == existscmd) {
            if (sim) {
                vector<string> vars = Tokenizer::tokenize(buffer, " \n");
                // Need a new client here because we're going to do call and response
                // with the sim
//                TCClient addclient(sim->getHost(), sim->getPort());
                TCClient addclient(*sim);
                int ret = addclient.connect();
                if (ret == TC_SUCCESS) {
                    passthru.writeSimClient(exists(addclient, vars));
                    send_cmd(addclient, "var_exit;");
                    addclient.disconnect();
                } else {
                    passthru.writeSimClient(
                          SniffSrvCmds::failure("Cannot connect"));
                }
            } else {
                passthru.writeSimClient(
                          SniffSrvCmds::failure("Not connected to a sim"));
            }
            continue;
        }

        // Add multiple variables
        string addmultiplecmd = SniffSrvCmds::addmultiple();
        if (buffer.substr(0, addmultiplecmd.size()) == addmultiplecmd) {
            vector<string> vars = Tokenizer::tokenize(buffer, " \n");
            // Any results will be sent back to the client via the passthru
            // However, no results are expected provided all the variables exist on the
            // variable server. The intended course of action is to use the exists command
            // before sending addmultiple.
            addmultiple(*sim, vars);
            continue;
        }

        // Unknown command
        passthru.writeSimClient(SniffSrvCmds::failure("Unknown command"));
    }

    // Shut down sim connection (if there is one)
    passthru.disconnect();
    if (sim) {
        delete sim;
        sim = NULL;
    }
    cerr << "Exiting server thread for " << srv->getSocket() << endl;
    return NULL;
}

int main(int argc, char* argv[]) 
{
    Thread sendthread;
    list<Thread> serverthreads;
    int port;

    if (argc < 2) {
        port = 9796;
    } else {
        stringstream strm;
        strm << argv[1];
        strm >> port;
    }

    try {
        cerr << "Starting manager on " << port << endl;
        TCServerMgr mgr(port);
        
        try {
            sendthread.Start(sendsims, NULL);
        } catch (Exception& e) {
            cerr << "Unable to create send thread" << endl;
            exit(1);
        }
        while (1) {
            TCServer* srv = mgr.accept();
            Thread serverthread(&theserver, (void*) srv, "SniffSrv");
            serverthreads.push_back(serverthread);
        }
    } catch (string& exc) {
        cerr << "Got exception: " << exc << endl;
    } catch (Exception& exc) {
        cerr << "Got exception: " << exc.what() << endl;
    } catch (...) {
        cerr << "Got unknown exception: " << endl;
    }
}
