/************************************************************************
PURPOSE: (Handles the waypoints.)
**************************************************************************/
#ifndef Waypoint_HH
#define Waypoint_HH

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Waypoint {
    public:
    double pos[2];
    std::string img;

    Waypoint(double n, double w, std::string i);
    std::string dataToString();
};

class WaypointList {
    public:
    std::vector<Waypoint> queue;
    std::vector<Waypoint>::iterator current;

    WaypointList();

    void reset();
    void add(Waypoint w);
    bool next();
    std::string dataToString();
};

#endif