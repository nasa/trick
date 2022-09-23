
/*
PURPOSE: (To be)
LIBRARY_DEPENDENCIES: (ros_subscribe.cpp)
*/

#ifndef ROS_SUBSCRIBE_HH
#define ROS_SUBSCRIBE_HH

#include "ros/ros.h"

class RosSubscribe {
    public:
        ros::NodeHandle n ;      // ** ros nodehandle
        ros::Subscriber sub ;    // ** ros subscriber

        RosSubscribe() ;
        int init() ;
        int process() ;
} ;

#endif
