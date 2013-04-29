#ifndef WIN32
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#endif
#include "../libtc/tccomm.hh"
#include "../libutils/convutils.hh"
#include <cstring>
#include <iostream>
#include <cerrno>
#include "sniffer.hh"
#include "../libtc/tcclient.hh"
#include "sniffsrvcmds.hh"
#include "../libtc/tokenizer.hh"
#include "../libutils/autolock.hh"
#include "../libutils/convutils.hh"
#include "sniffline.hh"
#include <sstream>
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;
using std::list;

// direct sniffing only
Sniffer::Sniffer(): _keep_running(true)
{
    _init();
}

// direct and remote sniffing
Sniffer::Sniffer(list<string>& hostnports): _keep_running(true)
{
    _init();
    add(hostnports);
}

VSClientConnector* Sniffer::getConnector(const std::string& line)
{
    HostSpec spec(line);

    if (spec.isValid()) {
        return VSClientFactory().getConnector(spec);
    } else {
        cerr << "Bad spec" << endl;
        return NULL;
    }
}

void Sniffer::add(const HostSpec& hostspec)
{
    Thread new_thread;
    AutoLock locker(&_mutex);

    Sniffer::ThreadInit* params = new Sniffer::ThreadInit;
    params->sniffer = this;
    params->spec = hostspec;
    new_thread.Start( (void* (*)(void*)) (&Sniffer::_static_sniffremote),
                      (void*) params);
    _remote_threads.push_back(new_thread);
}

void Sniffer::add(const string& hostnport)
{
    add(HostSpec(hostnport));
}

void Sniffer::add(list<string>& hostnports)
{
    list<string>::iterator iter = hostnports.begin();
    while (iter != hostnports.end()) {
        add(*iter);
        iter++;
    }
}

void Sniffer::_init()
{
#ifdef WIN32
    WSADATA wsaData;

    /* Initiate use of the Windows socket DLL */
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        MessageBox(NULL, "Error constructing socket", "Sniffer", MB_OK);
        WSACleanup();
        throw Exception("Unable to initialize socket API");
    }
    fprintf(stderr, "Increment on %p\n", this);
#endif
    _direct_thread.Start( (void* (*)(void*)) (&Sniffer::_static_sniffdirect),
                          (void*) this);
}

Sniffer::~Sniffer()
{
    _keep_running = false;
    { // Block for AutoLock
        AutoLock locker(&_mutex);
        _direct_thread.Join();
    }
    list<Thread>::iterator iter = _remote_threads.begin();
    while (iter != _remote_threads.end()) {
        (*iter).Join();
        iter++;
    }
#ifdef WIN32
    fprintf(stderr, "Decrement on %p\n", this);
    /* Terminate use of the winsock DLL - matches WSAStartup call in constructor */
    if (WSACleanup() != 0) {
        char msg[1024];
        sprintf(msg, "Error shutting down socket: %d\n", tc_errno);
        MessageBox(NULL, msg, "Sniffer::~Sniffer", MB_OK);
    }
#endif
}

// Return a newline-delimited string of items in the right format for display
// on the GUI
string Sniffer::get_serialized_list()
{
    vector<string> sims = get_list();
    string result;
    
    for (unsigned int ii = 0; ii < sims.size(); ii++) {
        // Pull out all the words
        SniffLine sniffline(sims[ii]);
        if (! sniffline.isValid()) continue;
        result += sniffline.serialize() + "\n";

    }
    return result;
}

vector<string> Sniffer::get_list()
{
    double thetime = Utils::get_Time();
    vector<string> thelist;
    AutoLock locker(&_mutex);
    list<Item>::iterator iter = _list.begin();
    while (iter != _list.end()) {
        if (!_too_old(*iter, thetime)) {
            thelist.push_back(iter->content);
        }
        iter++;
    }
    return thelist;
}

bool Sniffer::_too_old(Item& item, double time)
{
    return (time - item.age > TOOOLD);
}

void Sniffer::_update_list(const string& item)
{
    double thetime = Utils::get_Time();
    bool found = false;
    
    list<Item>::iterator iter = _list.begin();
    while (iter != _list.end()) {
        if (iter->content == item) {
            iter->age = thetime;
            found = true;
            break;
        }
        iter++;
    }
    if (!found) {
//cerr << "Adding item " << item << endl;
        _list.push_back(Item(item, thetime));
    }

    // Check for outdated entries
    iter = _list.begin();
    while (iter != _list.end()) {
        if (_too_old(*iter, thetime)) {
            iter = _list.erase(iter);
        } else {
            iter++;
        }
    }
}

void* Sniffer::_static_sniffdirect(void* thisptr)
{
    Sniffer* sniffer = (Sniffer*) thisptr;
    sniffer->_sniffdirect();
    return NULL;
}

void Sniffer::_sniffdirect(void)
{
   int sock;
   int status;
   const int bufferSize = 4096;
   char buffer[bufferSize+1];

   sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

   sockaddr_in serverAddr;
   memset((char*) &serverAddr, 0, sizeof(serverAddr));
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(9265);
   serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

   struct ip_mreq imreq;
   memset(&imreq, 0, sizeof(struct ip_mreq));
   imreq.imr_multiaddr.s_addr = inet_addr("224.3.14.15");
   imreq.imr_interface.s_addr = INADDR_ANY;
#ifdef WIN32
   u_long on;
#else
   int on;
#endif
   on = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0) {
       fprintf(stderr, "Error setting sockopt: %d\n", tc_errno);
       perror("setsockopt: reuseport");
   }
#ifdef SO_REUSEPORT
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char *) &on, sizeof(on)) < 0) {
       perror("setsockopt: reuseport");
   }
#endif
#ifdef WIN32
   if ((status = bind(sock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in))) == SOCKET_ERROR) {
       fprintf(stderr, "Error binding sniffer to socket: %d\n", tc_errno);
   }
#else
   status = bind(sock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in));
#endif

   IOCTL_SOCKET(sock, (unsigned long)FIONBIO, &on);

   status = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
#ifndef WIN32
                      (const void*)&imreq, sizeof(struct ip_mreq));
#else
                      (const char*)&imreq, sizeof(struct ip_mreq));
#endif

//cerr << "Set sock status = " << status << endl;
   socklen_t size = sizeof(serverAddr);
   int nbytes;
   
   while (_keep_running) {
       nbytes = recvfrom(sock, buffer, bufferSize, 0, 
                      (struct sockaddr*) &serverAddr, &size);

       if (nbytes > 0) {
           buffer[nbytes] = '\0';

           { // Block for AutoLock
               AutoLock locker(&_mutex);
               _update_list(string(buffer));
           }
       } else if (nbytes == -1 && tc_errno == TRICKCOMM_EWOULDBLOCK) {
           Utils::sleep(1000000);
       } else {
           AutoLock locker(&_mutex);
           cerr << "Sniffer giving up with code " << nbytes 
                << " errno " << tc_errno << endl;
           _list.clear();

           break;
       }
   }
}

// Thread entry function
// params is allocated via new, and so must be deleted at exit
void* Sniffer::_static_sniffremote(void* params)
{
    ThreadInit* init = (ThreadInit*) params;
    Sniffer* s = init->sniffer;
    HostSpec spec = init->spec;
    s->_sniffremote(spec);
    delete (ThreadInit*) params;
    return NULL;
}

void Sniffer::_sniffremote(HostSpec& spec)
{
    string buffer;

    TCClient client(spec);
    // Problem with this in Trick 10?
    //client.setBlockIO(TCComm::TC_COMM_TIMED_BLOCKIO);
    client.connect();
    if (! client.isValid()) {
        return;
    }
    cerr << "Connected to Trick sim sniffer on " << spec.host() 
         << ":" << spec.port() << endl;
    client.write(SniffSrvCmds::listperiodic());
    string remainder;
    buffer = client.readline(remainder);
    
    if (buffer != SniffSrvCmds::success()) {
        cerr << "Couldn't register for list of sims; server said: " 
             << buffer << endl;
    }

    while (client.isValid() && _keep_running) {
        // Get some data
        buffer = client.readline(remainder, 2);
        if (buffer == "") continue;

        AutoLock locker(&_mutex);
        // TODO: Update this to use HostSpec
        _update_list(spec.spec() + " " + buffer);
    }
    cerr << "Disconnecting from Trick sim sniffer" << endl;
}
