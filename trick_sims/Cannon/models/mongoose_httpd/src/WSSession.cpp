/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
LIBRARY DEPENDENCIES:
    ((simpleJSON.o))
**************************************************************************/
#include "trick/memorymanager_c_intf.h"
#include "trick/exec_proto.h"
#include "../include/WSSession.hh"
#include "../include/simpleJSON.hh"
#include <sstream>
#include <iomanip> // for setprecision

WSsession::WSsession( struct mg_connection *nc ) {
    connection = nc;
    valuesStaged = false;
    intervalTimeTics = exec_get_time_tic_value(); // Default time interval is one second.
    nextTime = LLONG_MAX;
    cyclicSendEnabled = false;
}

void WSsession::setTimeInterval(unsigned int milliseconds) {
    intervalTimeTics = exec_get_time_tic_value() * milliseconds / 1000;
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
    new_ref = ref_attributes(vname);
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
        // This REF2 object will "belong" to the WSsessionVariable, so it has
        // the right and responsibility to free() it in its destructor.
        WSsessionVariable *sessionVariable = new WSsessionVariable( new_ref ) ;
        sessionVariables.push_back( sessionVariable ) ;
    }
}

void WSsession::stageValues() {
    stageTime = (double)exec_get_time_tics() / exec_get_time_tic_value();
    std::vector<WSsessionVariable*>::iterator it;
    for (it = sessionVariables.begin(); it != sessionVariables.end(); it++ ) {
        (*it)->stageValue();
    }
    valuesStaged = true;
}

void WSsession::stageValuesSynchronously() {
    long long simulation_time_tics = exec_get_time_tics();
    if ( cyclicSendEnabled && ( simulation_time_tics >= nextTime )) {
        stageValues();
    }
    nextTime = (simulation_time_tics - (simulation_time_tics % intervalTimeTics) + intervalTimeTics);
}

void WSsession::sendValues() {
    if ( valuesStaged ) {
        std::vector<WSsessionVariable*>::iterator it;
        std::stringstream ss;

        ss << "{ \"msg_type\" : \"values\",\n";
        ss << "  \"time\" : " << std::setprecision(16) << stageTime << ",\n";
        ss << "  \"values\" : [\n";

        for (it = sessionVariables.begin(); it != sessionVariables.end(); it++ ) {
            if (it != sessionVariables.begin()) ss << ",\n";
            (*it)->writeValue(ss);
         }
         ss << "]}" << std::endl;
        std::string tmp = ss.str();
        const char * message = tmp.c_str();
        mg_send_websocket_frame(connection, WEBSOCKET_OP_TEXT, message, strlen(message));
        valuesStaged = false;
    }
}

void WSsession::pause()   { cyclicSendEnabled = false; }

void WSsession::unpause() { cyclicSendEnabled = true;  }

void WSsession::clear() {
        std::vector<WSsessionVariable*>::iterator it;
        it = sessionVariables.begin();
        while (it != sessionVariables.end()) {
            delete *it;
            it = sessionVariables.erase(it);
        }
}

void WSsession::exit() {}

int WSsession::handle_msg (const char* client_msg) {

     int status = 0;
     std::vector<Member*> members = parseJSON(client_msg);
     std::vector<Member*>::iterator it;
     const char *cmd;
     const char *var_name;
     int period;

     for (it = members.begin(); it != members.end(); it++ ) {
         if (strcmp((*it)->key, "cmd") == 0) {
             cmd = (*it)->valText;
         } else if (strcmp((*it)->key, "var_name") == 0) {
             var_name = (*it)->valText;
         } else if (strcmp((*it)->key, "period") == 0) {
             period = atoi((*it)->valText);
         }
     }

     if (cmd == NULL) {
         printf ("No \"cmd\" member found in client message.\n");
         status = 1;
     } else if (strcmp(cmd, "var_add") == 0) {
         addVariable( strdup(var_name) );
     } else if ( strcmp(cmd, "var_cycle") == 0 ) {
         setTimeInterval(period);
     } else if ( strcmp(cmd, "var_pause") == 0 ) {
         pause();
     } else if ( strcmp(cmd, "var_unpause") == 0 ) {
         unpause();
     } else if ( strcmp(cmd, "var_send") == 0 ) {
         stageValues();
         sendValues();
     } else if ( strcmp(cmd, "var_clear") == 0 ) {
         clear();
     } else if ( strcmp(cmd, "var_exit") == 0 ) {
         //TODO
     } else {
         emitError("Unknown Command: \"%s\".\n", cmd);
         status = 1;
     }
     return status;
}
