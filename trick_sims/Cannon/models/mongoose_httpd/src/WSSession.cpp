#include "../include/WSSession.hh"

WSsession::WSsession( struct mg_connection *c ) {
    connection = c;
    clock_gettime(CLOCK_MONOTONIC_RAW, &lastTime);
    setTimeInterval(1000); // Default time interval is one second.
}
void WSsession::setTimeInterval(unsigned int milliseconds) {
    interval.tv_sec = milliseconds / 1000;
    interval.tv_nsec = (milliseconds % 1000) * 1000000L;
}
void WSsession::addVariable(const char* vname){
    varNames.push_back(vname);
}
void WSsession::sendValues() {
    std::vector<const char*>::iterator it;
    for (it = varNames.begin(); it != varNames.end(); it++ ) {
        // ==================================================================
        // FIXME: Get the values of the variables and send them to the client.
        // Currently we just send the names for testing.
        // ==================================================================
        mg_send_websocket_frame(connection, WEBSOCKET_OP_TEXT, *it, strlen(*it));
    }
}
void WSsession::synchSend() { // This must be called at a frequency greater than or equal to the interval.
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    if (to_nanoseconds(&now) - to_nanoseconds(&lastTime) >= to_nanoseconds(&interval)) {
        if (enabled) {
            sendValues();
        }
        lastTime = now;
    }
}
void WSsession::pause()   { enabled = false;}
void WSsession::unpause() { enabled = true; }

// int main ( int argc, char* argv[]) {
//     WSsessionMap wsmap;
//     struct mg_connection *c
//     WSsession* wsess = new WSsession(c);
//     struct timespec nap, t2;

//     nap.tv_sec  = 0;
//     nap.tv_nsec = 10000000L;
//
//     wsess->setTimeInterval(500);
//     wsess->addVariable("dyn.cannon.pos[0]");
//     wsess->addVariable("dyn.cannon.pos[1]");
//     wsess->unpause();
//
//     while(1) {
//         wsess->synchSend();
//         if ( nanosleep(&nap, &t2) < 0) { return -1; };
//     }
//     return 0;
// }
