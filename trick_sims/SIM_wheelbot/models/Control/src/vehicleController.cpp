#include "Control/include/vehicleController.hh"
#include <iostream>

VehicleController::VehicleController( std::vector<Point>* wayPoints,
                                      Navigator& theNavigator,
                                      DifferentialDriveController& theDriveController,
                                      double arrival_distance):
   navigator(theNavigator),
   driveController(theDriveController) {

    // Enforce minimum arrival distance.
    if (arrival_distance > 0.01) {
        arrivalDistance = arrival_distance;
    } else {
        arrivalDistance = 0.01;
    }
    waypointQueue = wayPoints;
    destination = waypointQueue->begin();
    homing = false;
    atend = false;
    printDestination();
}

void VehicleController::setWayPointQueue( std::vector<Point>* wayPoints ) {
    waypointQueue = wayPoints;
    destination = waypointQueue->begin();
    printDestination();
}

int VehicleController::getCurrentDestination(Point& currentDestination) {
    if (destination != waypointQueue->end()) {
        currentDestination = *destination;
        return 0;
    }
    return 1;
}

void VehicleController::printDestination() {
    if (destination != waypointQueue->end()) {
        std::cout << "Destination = (" << destination->x << "," << destination->y << ")." << std::endl;
    } else {
        std::cout << "No Destination." << std::endl;
    }
}

// Homing functions
bool VehicleController::get_homing() {
    return homing;
}

bool VehicleController::get_atend() {
    return atend;
}

void VehicleController::go_home(std::vector<Point>* homePoints) {
    waypointQueue = homePoints;
    destination = waypointQueue->begin();
    printDestination();
}

void VehicleController::update() {

    if (destination == waypointQueue->end() && homing == false) {
        driveController.update(0.0, 0.0);
        driveController.stop();

        // To control homing: comment out for no homing
        homing = true;

        if (homing == false) {
          atend = true;
        }
    } else if (destination == waypointQueue->end() && homing == true) {
        atend = true;
        driveController.stop();
    } else {
        double distance_err = navigator.distanceTo(*destination);
        if ( distance_err > arrivalDistance) {
            double heading_err = navigator.bearingTo(*destination);
            driveController.update(distance_err, heading_err);
        } else {
            std::cout << "Arrived at Destination." << std::endl;
            destination ++;
            printDestination();
        }
    }
}
