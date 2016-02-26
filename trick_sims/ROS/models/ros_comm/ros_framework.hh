
/*
PURPOSE: (To be)
LIBRARY_DEPENDENCIES: (ros_framework.cpp)
*/

#ifndef ROS_FRAMEWORK_HH
#define ROS_FRAMEWORK_HH

#include <string>

class RosFramework {
    public:
        RosFramework(std::string app_name) ;
        virtual ~RosFramework() ;
        virtual int shutdown() ;
} ;

#endif
