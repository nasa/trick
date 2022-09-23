
/*
PURPOSE: (To be)
LIBRARY_DEPENDENCIES: (ros_publish.cpp)
*/

#ifndef ROS_PUBLISH_HH
#define ROS_PUBLISH_HH

#include "ros/ros.h"

class RosPublish {
    public:
        ros::NodeHandle n ;             // ** ros nodehandle
        ros::Publisher msg_pub ;        // ** ros publisher

        int count ;

        RosPublish() ;
        int init() ;
        int publish() ;
} ;

#endif
