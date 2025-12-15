#ifndef __SUBNODE_HH_
#define __SUBNODE_HH_
/*******************************************
PURPOSE: Trick model for ROS2 subscriber node
LIBRARY DEPENDENCY:
(
    (trick_subscriber/src/SubNode.cc)
)
*******************************************/

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <iostream>

class SubNode
{
    public:
        std::string darkEchoes;

        std::string nodeName;
        std::string subscriptionName;

        rclcpp::Node::SharedPtr  node;
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub;

        void defaults();
        void init_ros();
        void earToTheVoid();
        void shutItDown();

};


#endif
