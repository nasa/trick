
#include "ros_subscribe.hh"
#include "Num.h"
#include "std_msgs/String.h"

void numchatterCallback(const trick_msgs::Num::ConstPtr & msg)
{
    std::cout << *msg << std::endl;
}

RosSubscribe::RosSubscribe() {}

int RosSubscribe::init()
{
    sub = n.subscribe("num_chatter", 1000, numchatterCallback);
    return 0;
}

int RosSubscribe::process()
{
    ros::spin();
    return 0;
}
