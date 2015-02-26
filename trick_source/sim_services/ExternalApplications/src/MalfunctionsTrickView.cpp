/*
 * $Id: MalfunctionsTrickView.cpp 2126 2012-01-17 22:08:56Z dbankie $
 */

#include <string>

#include "sim_services/ExternalApplications/include/MalfunctionsTrickView.hh"

Trick::MalfunctionsTrickView::MalfunctionsTrickView() {
    command += std::string("mtv");
}

std::string Trick::MalfunctionsTrickView::create_arguments_string() {
    ExternalApplication::create_arguments_string();

    std::ostringstream oss;

    if (host_source != NONE) {
        oss << " " << host;
    }

    if (port_source != NONE) {
        oss << " " << port;
    }

    return oss.str();
}
