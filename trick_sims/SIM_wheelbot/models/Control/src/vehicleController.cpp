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
    printDestination();
    exit = false;
    home = false;
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

void VehicleController::gohome() {
    destination = waypointQueue->end()-1;
    home = true;
}

void VehicleController::printDestination() {
    if (destination != waypointQueue->end()) {
        std::cout << "Destination = (" << destination->x << "," << destination->y << ")." << std::endl;
    } else {
        std::cout << "No Destination." << std::endl;
    }
}

bool VehicleController::getexit() {
    return exit;
}

void VehicleController::update() {

    if (destination == waypointQueue->end() && exit == false) {
        if (home == false) {
            driveController.update(0.0, 0.0);
        }
        exit = true;
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
