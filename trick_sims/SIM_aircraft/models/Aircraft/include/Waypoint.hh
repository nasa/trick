/************************************************************************
PURPOSE: (Handles the waypoints.)
**************************************************************************/
#ifndef Waypoint_HH
#define Waypoint_HH

#ifndef ARRAY_SIZE
#define ARRAY_SIZE 10
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class WaypointList
{
public:
    WaypointList();
    WaypointList(std::string path);

    int add(double n, double w, std::string i);
    void remove(int index);
    void clear();

    void load(std::string path);
    void append(std::string path);
    void save(std::string path);

    void getPosition(int index, double (&pos)[2]);
    std::string getImage(int index);
    int size();

private:
    double north[ARRAY_SIZE];
    double west[ARRAY_SIZE];
    std::string img[ARRAY_SIZE];

    int length;
};

#endif
