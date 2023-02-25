/********************************* TRICK HEADER *******************************
PURPOSE: ( Handles the waypoints. )
*******************************************************************************/
#include "Aircraft/include/Waypoint.hh"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Waypoint.hh"

Waypoint::Waypoint(double n, double w, std::string i) {
    pos[0] = n;
    pos[1] = w;
    img = i;
    data = std::to_string(pos[0]) + "," + std::to_string(pos[1]) + "," + img;
}

WaypointList::WaypointList() {
    queue = std::vector<Waypoint>();
    current = queue.begin();
}

void WaypointList::reset() {
    current = queue.begin();
}

void WaypointList::add(Waypoint w) {
    queue.push_back(w);
}

bool WaypointList::next() {
    if(current != queue.end()) {
        current++;
        return true;
    } else {
        current = queue.begin();
        return false;
    }
}

std::string WaypointList::exportData() {
    std::vector<Waypoint>::iterator i = queue.begin();
    std::string list = "";
    while (i != queue.end())
    {
        list += "|" + i->data + "|";
        i++;
    }
    return list;
}

std::string WaypointList::operator[](int i)
{
    return queue[i].data;
}
