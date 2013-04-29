#include "../../libtc/tcclient.hh"
#include "../../libtc/hostspec.hh"
#include "../../libutils/thread.hh"
#include "../../libutils/convutils.hh"
#include "../sniffsrvcmds.hh"
#include "dsp.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
using std::stringstream;
using std::ifstream;
using std::cerr;
using std::endl;
using std::cin;

int send_cmd(TCClient& client, string cmd)
{
    int cmd_size = htonl(cmd.size()) ;

    if (client.write((char *)&cmd_size, sizeof(int)) <= 0 ) {
        fprintf(stderr, "send_cmd(): Failed writing size information.\n");
        return -1;
    }

    if (client.write(cmd.c_str(), cmd.size()) <= 0 ) {
        fprintf(stderr, "send_cmd(): Failed writing primary data.\n");
        return -1;
    }
    return 0;
}

void* listen(void* clientptr)
{
  TCClient* client = (TCClient*) clientptr;

  double ref_time = client->clock_init();
  int receptions = 0;

  /* Round trip to server */
  client->setBlockIO(TCComm::TC_COMM_BLOCKIO);
  string remainder;
  while (client->isValid()) {
      string s = client->readline(remainder);
      if (s != "") {
          if (SniffSrvCmds::isCmd(s) || s.size() < 100) {
              cerr << s;
          } else {
              if (s.size() != 2682) {
                  cerr << endl << "Received " << s.size() << " bytes" << endl;
              } else {
                  cerr << ".";
              }
          }
      }
      receptions++;
      if (client->clock_time(ref_time) > 10) {
          cerr << endl << "Receptions/sec = " << receptions / client->clock_time(ref_time) << endl;
          receptions = 0;
          ref_time = client->clock_init();
      }

      Utils::sleep(10);
  }
  cerr << "Exited because connection went down" << endl;
  return NULL;
}

int main( int narg, char** args ) 
{
  Thread listener;
  string s;
  stringstream strm;

  if ( narg < 3 ) {
    fprintf(stderr, 
       "USAGE: sniffclient <host> <port>\n");
    exit(-1);
  }
  strm << args[2];
  int port;
  strm >> port;

  TCClient client(HostSpec(args[1], port,"07")) ;
  client.connect();
  if (!client.isValid()) {
      cerr << "Couldn't get a connection to server" << endl;
      exit(-1);
  }

  listener.Start(listen, &client);
  cerr << "What is your bidding, master?" << endl;

  while (client.isValid()) {
      getline(cin, s);
      if (SniffSrvCmds::isCmd(s)) {
          client.write(s+"\n");
      } else if (s[0] == '@') {
          string fname = s.substr(1);
          cerr << "Got " << fname << endl;
          ifstream strm(fname.c_str());
          string line;
          while (strm.good()) {
              getline(strm, line);
              cerr << "Sending " << line << endl;
              send_cmd(client, line + "\n");
          }
      } else {
          send_cmd(client, s + "\n");
      }
  }

  return 0 ;
}


