/*
 * PURPOSE: (VehicleController class)
 */
#ifndef VEHICLE_CONTROLLER_HH
#define VEHICLE_CONTROLLER_HH

#include <vector>
#include "Guidance/include/point.hh"
#include "Guidance/include/navigator.hh"
#include "Control/include/differentialDriveController.hh"

#ifndef PI
#define PI 3.141592653589793
#endif

class VehicleController {
    public:
    VehicleController(std::vector<Point>* waypointQueue,
                      Navigator& navigator,
                      DifferentialDriveController& driveController,
                      double arrival_distance);

    int getCurrentDestination(Point& currentDestination);
    void setWayPointQueue( std::vector<Point>* waypointQueue );
    void printDestination();
    void update();

    private:
    // Do not allow the default constructor to be used.
    VehicleController();

    std::vector<Point>* waypointQueue;
    std::vector<Point>::iterator destination;
    Point departure;
    Navigator& navigator;
    DifferentialDriveController& driveController;

    double arrivalDistance;
};
#endif
