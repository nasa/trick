ROS_HOME = /opt/ros/kinetic
TRICK_EXCLUDE += :${ROS_HOME}

TRICK_CFLAGS += -I../models -I${ROS_HOME}/include
TRICK_CXXFLAGS += -I../models -I${ROS_HOME}/include -Wno-shadow -Wno-unused-parameter

TRICK_EXEC_LINK_LIBS += -L${ROS_HOME}/lib -lroscpp -lrosconsole -lroscpp_serialization
