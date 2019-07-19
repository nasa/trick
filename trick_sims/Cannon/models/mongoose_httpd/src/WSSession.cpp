#include "trick/memorymanager_c_intf.h"
#include "../include/WSSession.hh"
#include <sstream>






WSsession::WSsession( struct mg_connection *c ) {
    connection = c;
    clock_gettime(CLOCK_MONOTONIC_RAW, &lastTime);
    setTimeInterval(1000); // Default time interval is one second.
}
void WSsession::setTimeInterval(unsigned int milliseconds) {
    interval.tv_sec = milliseconds / 1000;
    interval.tv_nsec = (milliseconds % 1000) * 1000000L;
}

int WSsession::bad_ref_int = 0 ;

REF2* WSsession::make_error_ref(const char* in_name) {
    REF2* new_ref;
    new_ref = (REF2*)calloc(1, sizeof(REF2));
    new_ref->reference = strdup(in_name) ;
    new_ref->units = NULL ;
    new_ref->address = (char *)&bad_ref_int ;
    new_ref->attr = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES)) ;
    new_ref->attr->type = TRICK_NUMBER_OF_TYPES ;
    new_ref->attr->units = (char *)"--" ;
    new_ref->attr->size = sizeof(int) ;
    return new_ref;
}

//
// #include "trick/exec_proto.h"
// time = (double)exec_get_time_tics() / exec_get_time_tic_value() ;

//REF2* Trick::VariableServerThread::make_time_ref() {
//    REF2* new_ref;
//    new_ref = (REF2*)calloc(1, sizeof(REF2));
//    new_ref->reference = strdup("time") ;
//    new_ref->units = strdup("s") ;
//    new_ref->address = (char *)&time ;
//    new_ref->attr = (ATTRIBUTES*)calloc(1, sizeof(ATTRIBUTES)) ;
//    new_ref->attr->type = TRICK_DOUBLE ;
//    new_ref->attr->units = strdup("s") ;
//    new_ref->attr->size = sizeof(double) ;
//    return new_ref;
//}

#define MAX_MSG_SIZE 4096
int WSsession::emitError(const char* fmt, ... ) {
    char errText[MAX_MSG_SIZE];
    char msgText[MAX_MSG_SIZE];
    va_list args;

    errText[0]=0;
    msgText[0]=0;

    va_start(args, fmt);
    (void) vsnprintf(errText, MAX_MSG_SIZE, fmt, args);
    va_end(args);

    sprintf(msgText, "{ \"msg_type\" : \"error\",\n"
                     "  \"error\" : \"%s\"}\n", errText);

    mg_send_websocket_frame(connection, WEBSOCKET_OP_TEXT, msgText, strlen(msgText));
    return (0);
}

void WSsession::addVariable(char* vname){

    REF2 * new_ref ;
//    if ( strcmp(vname, "time") == 0 ) {
//        new_ref = make_time_ref();
//    } else {
        new_ref = ref_attributes(vname) ;
//    }

    if ( new_ref == NULL ) {
        emitError("Variable Server could not find variable %s.\n", vname);
        new_ref = make_error_ref(vname);
    } else if ( new_ref->attr ) {
        if ( new_ref->attr->type == TRICK_STRUCTURED ) {
            emitError("Variable Server: var_add cant add \"%s\" because its a composite variable.\n", vname);
            free(new_ref);
            new_ref = make_error_ref(vname);

        } else if ( new_ref->attr->type == TRICK_STL ) {
            emitError("Variable Server: var_add cant add \"%s\" because its an STL variable.\n", vname);
            free(new_ref);
            new_ref = make_error_ref(vname);
        }
    } else {
        emitError("Variable Server: BAD MOJO - Missing ATTRIBUTES.");
        free(new_ref);
        new_ref = make_error_ref(vname);
    }

    if ( new_ref != NULL ) {
        WSsessionVariable *sessionVariable = new WSsessionVariable( new_ref ) ;
        sessionVariables.push_back( sessionVariable ) ;
    }
}

void WSsession::sendValues() {
    std::vector<WSsessionVariable*>::iterator it;
    std::stringstream ss;
    ss << "{ \"msg_type\" : \"values\",\n";
    ss << "  \"values\" : [\n";
    for (it = sessionVariables.begin(); it != sessionVariables.end(); it++ ) {
        if (it != sessionVariables.begin()) ss << ",\n";
        (*it)->write_value(ss);
    }
    ss << "]}" << std::endl;
    std::string tmp = ss.str();
    const char * message = tmp.c_str();
    mg_send_websocket_frame(connection, WEBSOCKET_OP_TEXT, message, strlen(message));
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
