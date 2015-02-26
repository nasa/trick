/******************************TRICK HEADER*************************************
PURPOSE:                     ( Test variable server )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( VS.o )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) )
*******************************************************************************/
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include "../include/VS.hh"
#include "sim_services/VariableServer/include/variable_server_proto.h"
#include "sim_services/VariableServer/include/VariableServer.hh"

// internal function: parse tc_read output
int VSTest::get_line(char* thing) {
    int ii;
    int count = 0;

    // take one full line of tc device output
    for (ii = 0; ii < strlen(thing); ii++) {
        if (thing[ii] != '\n') {
            got_read[ii] = thing[ii];
        } else {
            got_read[ii] = '\0';
            break;
        }
    }

    return(0);
}

void VSTest::printLine() {
    std::cout << got_read << std::endl;
}

int VSTest::vs_write(char* cmd_buffer) {
    int nbytes;

    nbytes = strlen(cmd_buffer);
    tc_write(&bob, cmd_buffer, nbytes);

    return(0);
}

int VSTest::vs_read() {
    int num;
    double cpu_t, cpu_st;
    char read_buffer[256];

    struct rusage cpu_usg;
    getrusage(RUSAGE_SELF, &cpu_usg);
    cpu_st = ((double) cpu_usg.ru_utime.tv_sec) + ((double) cpu_usg.ru_utime.tv_usec/1000000.0);

    while (true) {
        getrusage(RUSAGE_SELF, &cpu_usg);
        cpu_t = ((double) cpu_usg.ru_utime.tv_sec) + ((double) cpu_usg.ru_utime.tv_usec/1000000.0);

        // Check for empty read.
        if (cpu_t - cpu_st > 5.0*cycle_rate) {
            memset(got_read, '\0', strlen(got_read));
            break;
        }

        num = tc_pending(&bob);
        if (num) {
            tc_read(&bob, read_buffer, num);
                     get_line(read_buffer);
            break;
        }
    }
    //std::cout << (cpu_t - cpu_st) << std::endl;
    //std::cout << read_buffer << std::endl;
    //printLine();

    return(0);
}
