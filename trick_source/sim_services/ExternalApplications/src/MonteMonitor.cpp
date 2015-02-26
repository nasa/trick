/*
 * $Id: MonteMonitor.cpp 2126 2012-01-17 22:08:56Z dbankie $
 */

#include <string>

#include "sim_services/ExternalApplications/include/MonteMonitor.hh"

Trick::MonteMonitor::MonteMonitor() {
    command += std::string("mm");
}
