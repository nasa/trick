
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>
#include <vector>
#include <cstring>

#if _DMTCP
#include "dmtcpaware.h"
#endif

#include "trick/ExternalApplication.hh"
#include "trick/ExternalApplicationManager.hh"
#include "trick/variable_server_proto.h"
#include "trick/env_proto.h"
#include "trick/command_line_protos.h"

Trick::ExternalApplication::ExternalApplication() :
    command(std::string(env_get_var("TRICK_HOME") + std::string("/bin/"))) {
    host_source = port_source = AUTO;
    cycle_period_set = minimum_cycle_period_set = disconnect_behavior_set = height_set =
      width_set = x_set = y_set = auto_reconnect_set = false;
}

void Trick::ExternalApplication::set_startup_command(std::string in_command) {
    command = in_command;
}

std::string Trick::ExternalApplication::get_startup_command() {
    return command;
}

void Trick::ExternalApplication::add_arguments(std::string args) {
    arguments << " " << args;
}

void Trick::ExternalApplication::set_arguments(std::string args) {
    arguments.str(args);
    host_source = port_source = NONE;
    cycle_period_set = disconnect_behavior_set = height_set = width_set = x_set = y_set = false;
}

std::string Trick::ExternalApplication::get_arguments() {
    return arguments.str() + create_arguments_string();
}

void Trick::ExternalApplication::set_host(std::string in_host) {
    host = in_host;
    host_source = USER;
}

std::string Trick::ExternalApplication::get_host() {
    return host;
}

void Trick::ExternalApplication::set_port(unsigned short in_port) {
    port = in_port;
    port_source = USER;
}

unsigned short Trick::ExternalApplication::get_port() {
    return port;
}

void Trick::ExternalApplication::set_auto_reconnect(bool in_auto_reconnect) {
    auto_reconnect = in_auto_reconnect;
    auto_reconnect_set = true;
}

bool Trick::ExternalApplication::get_auto_reconnect() {
    return auto_reconnect;
}

void Trick::ExternalApplication::set_cycle_period(double in_cycle_period) {
    cycle_period = in_cycle_period;
    cycle_period_set = true;
}

double Trick::ExternalApplication::get_cycle_period() {
    return cycle_period;
}

void Trick::ExternalApplication::set_minimum_cycle_period(double in_minimum_cycle_period) {
    minimum_cycle_period = in_minimum_cycle_period;
    minimum_cycle_period_set = true;
}

double Trick::ExternalApplication::get_minimum_cycle_period() {
    return minimum_cycle_period;
}

void Trick::ExternalApplication::set_disconnect_behavior(DisconnectBehavior in_disconnect_behavior) {
    disconnect_behavior = in_disconnect_behavior;
    disconnect_behavior_set = true;
}

Trick::ExternalApplication::DisconnectBehavior Trick::ExternalApplication::get_disconnect_behavior() {
    return disconnect_behavior;
}

void Trick::ExternalApplication::set_height(int in_height) {
    height = in_height;
    height_set = true;
}

int Trick::ExternalApplication::get_height() {
    return height;
}

void Trick::ExternalApplication::set_width(int in_width) {
    width = in_width;
    width_set = true;
}

int Trick::ExternalApplication::get_width() {
    return width;
}

void Trick::ExternalApplication::set_x(int in_x) {
    x = in_x;
    x_set = true;
}

int Trick::ExternalApplication::get_x() {
    return x;
}

void Trick::ExternalApplication::set_y(int in_y) {
    y = in_y;
    y_set = true;
}

int Trick::ExternalApplication::get_y() {
    return y;
}

/** @par Detailed Design */
void Trick::ExternalApplication::launch() {
    /** <ul><li> If the application is enabled: */
    if (var_server_get_enabled()) {
        /** <ul><li> Execute the command and pass it any arguments. */
        std::ostringstream oss;

        int argc;
        char **argv;

        argc = command_line_args_get_argc() ;
        argv = command_line_args_get_argv() ;

        oss << command << " " << arguments.str() << " " << create_arguments_string() ;
        if (argc > 2) {
            for (int i=0;i<argc;i++) {
                if (!strcmp(argv[i], "dmtcp")) {
                    oss << " -dmtcp" ;
                    break ;
            }
        }
        }
        oss << " &";

#ifdef _DMTCP
        if( dmtcpIsEnabled() ) {
            std::string real_system_name ;

            void* dlhandle ;
            void (*real_system_ptr)(const char *) = NULL ;

            dlhandle = dlopen( NULL, RTLD_LAZY) ;

            real_system_name = "_real_system" ;
            real_system_ptr = (void (*)(const char *))dlsym( dlhandle , real_system_name.c_str()) ;


            if ( real_system_ptr != NULL ) {
                std::cout << "\nExternalApplication::launch() calling DMTCP _real_system \"" << oss.str() << "\n";
                (*real_system_ptr)(oss.str().c_str()) ;
            } else {
                std::cout << "calling DMTCP  \"" << oss.str().c_str() << "\"" << std::endl;
                system(oss.str().c_str());
            }

            dlclose(dlhandle) ;
        } else {
            system(oss.str().c_str());
        }
#else
        std::cout << oss.str() << std::endl;
        system(oss.str().c_str());
#endif
    }
}

std::string Trick::ExternalApplication::create_arguments_string() {
    std::ostringstream oss;

    switch (host_source) {
        case AUTO:
            host = var_server_get_hostname();
        case USER:
            oss << " --host " << host;
        case NONE:
            break;
    }

    switch (port_source) {
        case AUTO:
            port = var_server_get_port();
        case USER:
            oss << " --port " << port;
        case NONE:
            break;
    }

    if (minimum_cycle_period_set) {
        oss << " --minCycle " << minimum_cycle_period;
    }

    if (cycle_period_set) {
        oss << " --cycle " << cycle_period;
    }

    if (disconnect_behavior_set) {
        switch(disconnect_behavior) {
            case CLOSE:
                oss << " --disconnectBehavior Close";
                break;
            case NOTIFY:
                oss << " --disconnectBehavior Notify";
                break;
            case NOTHING:
                oss << " --disconnectBehavior Nothing";
                break;
            default:
                std::cout << __FILE__ << ":" << __LINE__ << ": [33;1mwarning:[0m "
                          << "Ignoring invalid value (" << disconnect_behavior
                          << ") for disconnect_behavior." << std::endl;
                break;
        }
    }

    if (height_set) {
        oss << " --height " << height;
    }

    if (width_set) {
        oss << " --width " << width;
    }

    if (x_set) {
        oss << " --x " << x;
    }

    if (y_set) {
        oss << " --y " << y;
    }

    if (auto_reconnect_set) {
        oss << " --autoReconnect " << (auto_reconnect ? "true" : "false");
    }

    return oss.str();
}
