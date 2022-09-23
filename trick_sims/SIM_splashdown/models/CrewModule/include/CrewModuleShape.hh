/************************************************************************
PURPOSE: (Simulate a ... .)
LIBRARY DEPENDENCIES:
    ((CrewModule/src/CrewModuleShape.o))
**************************************************************************/
#ifndef CREW_MODULE_SHAPE_HH
#define CREW_MODULE_SHAPE_HH

class CrewModuleShape {
public:
    double sphere_radius;
    double cone_angle;

    double sphere_center_base[3];
    double cone_point_base[3];
    double plane_point_base[3];

    double sphere_center_trans[3];
    double cone_point_trans[3];
    double plane_point_trans[3];

    // Work variables
    double cone_vector_trans[3];
    double plane_vector_trans[3];

    CrewModuleShape ();
    bool containsPoint(double (&test_point)[3] );
    void transformCoordinates( double (&rotation)[3][3], double (&position)[3]);
    void transformPoint( double (&rotation)[3][3], double (&translation)[3], double (&in_point)[3], double (&out_point)[3]);

};

#endif
