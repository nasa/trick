/************************************************************************
PURPOSE: (Handles the waypoints.)
**************************************************************************/
#ifndef Waypoint_HH
#define Waypoint_HH

#ifndef ARRAY_SIZE
#define ARRAY_SIZE  10
#endif

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class WaypointList {
    public:
    double north[ARRAY_SIZE];
    double west[ARRAY_SIZE];
    std::string img[ARRAY_SIZE];
    
    int length;

    WaypointList();
    WaypointList(std::string path);

    int add(double n, double w, std::string i);
    void remove(int index);
    void clear();
    
    void load(std::string path);
    void append(std::string path);
    void save(std::string path);
};

#endif