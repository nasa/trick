TRICK_CXXFLAGS += -g -I./ -std=c++17
TRICK_CXXFLAGS += -I/opt/ros/humble/include/
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rclcpp
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rcl
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rcutils
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rmw
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rcl_yaml_param_parser
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rosidl_runtime_c
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rosidl_typesupport_interface
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rcpputils
TRICK_CXXFLAGS += -I/opt/ros/humble/include/builtin_interfaces
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rosidl_runtime_cpp
TRICK_CXXFLAGS += -I/opt/ros/humble/include/tracetools
TRICK_CXXFLAGS += -I/opt/ros/humble/include/rcl_interfaces
TRICK_CXXFLAGS += -I/opt/ros/humble/include/libstatistics_collector
TRICK_CXXFLAGS += -I/opt/ros/humble/include/statistics_msgs
TRICK_CXXFLAGS += -I/opt/ros/humble/include/std_msgs

TRICK_CFLAGS=${TRICK_CXXFLAGS}
TRICK_USER_LINK_LIBS += -L/opt/ros/humble/lib -L/opt/ros/humble/share
TRICK_USER_LINK_LIBS += -lrclcpp
TRICK_USER_LINK_LIBS += -lrcl
TRICK_USER_LINK_LIBS += -lrmw
TRICK_USER_LINK_LIBS += -lrcutils
TRICK_USER_LINK_LIBS += -lrcpputils
TRICK_USER_LINK_LIBS += -lstd_msgs__rosidl_typesupport_cpp
TRICK_USER_LINK_LIBS += -lrmw_fastrtps_cpp
TRICK_USER_LINK_LIBS += -ltracetools
TRICK_USER_LINK_LIBS += -llibstatistics_collector

TRICK_EXCLUDE += /opt/ros/humble/include/

