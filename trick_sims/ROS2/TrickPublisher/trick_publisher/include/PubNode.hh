#ifndef __PUBNODE_HH_
#define __PUBNODE_HH_
/*******************************************
PURPOSE: Trick model for ROS2 publisher node
LIBRARY DEPENDENCY:
(
    (trick_publisher/src/PubNode.cc)
)
*******************************************/

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <iostream>

class PubNode
{
public:
    std::string whatToYell;

    std::string nodeName;
    std::string publicationName;

    rclcpp::Node::SharedPtr node;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub;

    void defaults();
    void init_ros();
    void yellIntoTheVoid();
    void shutItDown();
};

#endif
