/********************************* TRICK HEADER *******************************
PURPOSE: ()
LIBRARY DEPENDENCY:
    ((Guidance/src/navigator.cpp)
         )
PROGRAMMERS:
    (((John M. Penn) (L3 Communications) (March 2015) (Trick Refresher Project)))
*******************************************************************************/

#ifndef NAVIGATOR_HH
#define NAVIGATOR_HH

#include "point.hh"

    class Navigator {
        public:
        Navigator(double initial_heading, Point initial_location):
         heading(initial_heading), location(initial_location)
        { }
        void setHeading(double heading);
        void setLocation(double north, double west);

        double distanceTo(Point& mapPoint);
        double bearingTo(Point& mapPoint);

        Point convertMapToPlatform(Point& mapPoint);
        Point convertPlatformToMap(Point& platformPoint);
        Point convertPlatformToBody(Point& platformPoint);
        Point convertBodyToPlatform(Point& bodyPoint);

        private:
        double heading;
        Point  location;
    };
#endif
