#include "trick_publisher/include/PubNode.hh"


void PubNode::defaults()
{
    nodeName        = "trick_publisher_node";
    publicationName = "Trick_says";
}

void PubNode::init_ros()
{

    whatToYell = "Trick is screaming.";

    int argc=0;
    char **argv = 0x0;

    rclcpp::init(argc,argv);
    node = std::make_shared<rclcpp::Node>(nodeName);
    pub = node->create_publisher<std_msgs::msg::String>(publicationName,10);

}


void PubNode::yellIntoTheVoid()
{

    if(!pub)
    {
        std::cout << "No publisher detected.\n" << std::endl;
        return;
    }
    else if(!node)
    {
        std::cout << "No node detected.\n" << std::endl;
        return;
    }
    else
    {
        std_msgs::msg::String msg;
        msg.data = whatToYell;

        pub->publish(msg);
    }

    if(node)
    {
        rclcpp::spin_some(node);
    }
}

void PubNode::shutItDown()
{

    pub.reset();
    node.reset();
    rclcpp::shutdown();
}
