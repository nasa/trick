#ifndef PLANET_HH
#define PLANET_HH
/********************************* TRICK HEADER *******************************
PURPOSE:
      ()
      LIBRARY DEPENDENCY:
            ((../src/Planet.cpp))
PROGRAMMERS:
      (((John M. Penn) () () () ))
*******************************************************************************/

#define EARTH_MASS 5.9721986e24
#define EARTH_RADIUS 6367500.0

class Planet {
public:
    Planet();

    double pos[3];
    double mass;
    double radius;

};



#endif
