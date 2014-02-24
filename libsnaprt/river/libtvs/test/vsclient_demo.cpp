#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

using std::cerr;
using std::endl;
using std::cin;
using std::string;

#include "../vsclient.hh"
#include "../vsclientconnector.hh"
#include "../sniffer.hh"
#include "../transformation.hh"
#include "../../libutils/exception.hh"
#include "../../libutils/convutils.hh"

vector<TrickParam> values;
vector<Transformation> transforms;
bool exiting = false;

extern "C" {

// BEGIN Transform Functions
void square(Values& ins, Values& outs)
{
    double mode = ins["m"];
    double time = ins["t"];

    outs["m2"] = mode * mode;
    outs["mtt"] = mode * time;
}

void logsum(Values& ins, Values& outs)
{
    outs["ls"] = ::log(ins["m"] + ins["t"]);
}

}
// END of Transform Functions

unsigned int getIndex(const string& name)
{
    for (unsigned int ii = 0; ii < values.size(); ii++) {
        if (values[ii].name == name) return ii;
    }
    return 0;
}

void dogit(Values& ins, Values& outs)
{
    outs["dog"] = ins["i"] + ins["t"] ;
}

void handler(VSMessage& msg)
{
    static const int NUMVALUES = 100;
    static int numvalues = 0;
    cerr << "Message type is " << msg.type << endl;
    switch (msg.type) {
      case VSMessage::VSSTATUS_UPDATE:
             cerr << "Demo: Frozen is now " << msg.state.is_frozen << endl;
             cerr << "Demo: Connected is now " << msg.state.is_connected << endl;
             if (!msg.state.is_connected) exiting = true;;
             // process status change
             break;
      case VSMessage::VSDATA_UPDATE:

             // Update stored values -- in practice would use timestamp, check for dups etc.
             // Here we use a hard-coded index of 0 for vals

             values[0].vals[numvalues] = values[0].incoming_val;
             values[1].vals[numvalues] = values[1].incoming_val;

             // Perform transforms
             for (unsigned int ii = 0; ii < transforms.size(); ii++) {
                 transforms[ii].callTransform(numvalues);
             }
             cerr << "Time: " << values[0].vals[numvalues]
                  << "  Mode: " << values[1].vals[numvalues]
//                  << "  Mode^2: " << values[2].vals[numvalues]
//                  << "  Mode*Time: " << values[3].vals[numvalues]
//                  << "  Dog: " << values[getIndex("dog")].vals[numvalues]
//                  << "  Logsum: " << values[getIndex("logsumparam")].vals[numvalues]
                  << endl;
             if (++numvalues == NUMVALUES) exiting = true;
             break;
      default:
             cerr << "Demo: Got dog?" << endl;
             break;
    }
}

int main(int argc, char* argv[])
{
#ifdef WIN32
    WSADATA wsaData;

    /* Initiate use of the Windows socket DLL */
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        cerr << "Error constructing socket" << endl;
        WSACleanup();
        exit(-1);
    }
#endif
    char choice[256];
    Sniffer s;
    VSClientConnector* connector;
    s.add("localhost:9796");

    Utils::sleep(3000000); // Allow some time for sniffer to find sims
    while (1) {
        string sims = s.get_serialized_list();
//        string choice;
//        cerr << "Choices" << endl << sims;
        if (sims.length() == 0) { 
            cerr << "No sims available... looping" << endl;
            Utils::sleep(5000000);
            continue;
        }
cerr << "Compare: " << sims.find("\n") << " to " << sims.length()-1 << endl;
        if (sims.find("\n") != sims.length()-1) {
            cerr << "Choices" << endl << sims << endl;
            cin.getline( choice, 256 );
        } else {
            strcpy(choice, sims.c_str());
        }

        // Cheesy -- you need to enter the whole choice line. Improve this.
        connector = s.getConnector(choice);
        if (connector) {
            break;
        } else {
            cerr << "Can't get connector -- try again" << endl;
        }
    }
        
    try {
        values.reserve(2);
//        VSClient vsclient(handler, connector, 1, values);
        VSClient vsclient(handler, connector, 0.1, values);

        // Add non-transformed params
		vsclient.add(connector->getTimeParam());
        //vsclient.add(string(TIME_PARAM));
        values[0].allocate(1000);
		vsclient.add(connector->getModeParam());
        //vsclient.add(string(MODE_PARAM));
        values[1].allocate(1000);
/*
        // Add transformed params
        Transformation trans1(square);
        trans1.addInparam(values[getIndex(TIME_PARAM)], "t");
        trans1.addInparam(values[getIndex(MODE_PARAM)], "m");

        TrickParam tp("mode_squared", TrickParam::CALCULATED);
        tp.allocate(1000);
        values.push_back(TrickParam(tp));
        tp = TrickParam("mode_times_time", TrickParam::CALCULATED);
        tp.allocate(1000);
        values.push_back(TrickParam(tp));

        trans1.addOutparam(values[getIndex("mode_squared")], "m2");
        trans1.addOutparam(values[getIndex("mode_times_time")], "mtt");

        transforms.push_back(trans1);

        Transformation trans2("magnitude", "./square.so");
        trans2.addInparam(values[getIndex(TIME_PARAM)], "x");
        trans2.addInparam(values[getIndex(MODE_PARAM)], "y");
        tp = TrickParam("dog", TrickParam::CALCULATED);
        tp.allocate(1000);
        values.push_back(TrickParam(tp));
        trans2.addOutparam(values[getIndex("dog")], "m");
        transforms.push_back(trans2);

        Transformation trans3("logsum");
        trans3.addInparam(values[getIndex(TIME_PARAM)], "t");
        trans3.addInparam(values[getIndex(MODE_PARAM)], "m");
        tp = TrickParam("logsumparam", TrickParam::CALCULATED);
        tp.allocate(1000);
        values.push_back(TrickParam(tp));
        trans3.addOutparam(values[getIndex("logsumparam")], "ls");
        transforms.push_back(trans3);
*/


        vsclient.start();

        while (!exiting) {
            cerr << "Looping ... " << endl;
            Utils::sleep(4000000);
        }
        cerr << "All done! Terminating connections. " << endl;
    } catch (string& s) {
        cerr << "Exception: " << s << endl;
        exit(1);
    } catch (Exception& e) {
        cerr << "Exception!: " << e.what() << endl;
        exit(1);
    }
    delete connector;
    for (unsigned int ii = 0; ii < values.size(); ii++) {
        values[ii].deallocate();
    }
    for (unsigned int ii = 0; ii < transforms.size(); ii++) {
        transforms[ii].close();
    }
#ifdef WIN32
    /* Terminate use of the winsock DLL - matches WSAStartup call in constructor */
    if (WSACleanup() != 0) {
        cerr << "Error shutting down socket" << endl;
    }
#endif
}
