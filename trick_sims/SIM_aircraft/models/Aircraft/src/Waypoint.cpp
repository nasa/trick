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
}

// Returns the data in the Waypoint as a String
std::string Waypoint::dataToString() {
    return std::to_string(pos[0]) + "," + std::to_string(pos[1]) + "," + img;
}

WaypointList::WaypointList() {
    queue = std::vector<Waypoint>();
    current = queue.begin();
}

// Brings the iterator back to the beginning of the queue.
void WaypointList::reset() {
    current = queue.begin();
}

void WaypointList::add(Waypoint w) {
    queue.push_back(w);
}

// Have the 'current' iterator point to the next waypoint in the queue or loop back to the beginning
// Returns true if there are still waypoints left before the end of the list.
bool WaypointList::next() {
    if(current != queue.end()) {
        current++;
        return true;
    } else {
        reset();
        return false;
    }
}

// Transcribe the list of waypoints into a String that is returned
std::string WaypointList::dataToString() {
    std::vector<Waypoint>::iterator i = queue.begin();
    std::string list = "";
    while (i != queue.end())
    {
        list += "|" + i->dataToString() + "|";
        i++;
    }
    return list;
}
