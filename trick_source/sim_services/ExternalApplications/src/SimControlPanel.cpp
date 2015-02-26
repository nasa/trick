/*
 * $Id: SimControlPanel.cpp 3592 2014-07-01 16:01:00Z hchen3 $
 */

#include <string>

#include "sim_services/ExternalApplications/include/SimControlPanel.hh"

Trick::SimControlPanel::SimControlPanel() {
    command += std::string("sim_control");
    auto_exit = false;
}

std::string Trick::SimControlPanel::create_arguments_string() {
    ExternalApplication::create_arguments_string();

    std::ostringstream oss;

    if (host_source != NONE) {
        oss << " " << host;
    }

    if (port_source != NONE) {
        oss << " " << port;
    }

    if (auto_exit) {
    	oss << " -auto_exit";
    }

    return oss.str();
}
