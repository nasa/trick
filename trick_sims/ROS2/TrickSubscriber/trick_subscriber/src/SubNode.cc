#include "trick_subscriber/include/SubNode.hh"

void SubNode::defaults()
{
    nodeName         = "trick_subscriber_node";
    subscriptionName = "Trick_says";
}

void SubNode::init_ros()
{

    int argc=0;
    char **argv = 0x0;

    rclcpp::init(argc,argv);
    node = std::make_shared<rclcpp::Node>(nodeName);
    sub = node->create_subscription<std_msgs::msg::String>(
            subscriptionName, 10,
            [this](std_msgs::msg::String::ConstSharedPtr msg) {
                std::cout << "The void speaks: " << msg->data << std::endl;
            }
        );
}


void SubNode::earToTheVoid()
{

    if(!sub)
    {
        std::cout << "No subscriber detected.\n" << std::endl;
        return;
    }
    else if(!node)
    {
        std::cout << "No subscription node detected.\n" << std::endl;
        return;
    }
    else
    {
        rclcpp::spin_some(node);
    }

}

void SubNode::shutItDown()
{

    sub.reset();
    node.reset();
    rclcpp::shutdown();
}
