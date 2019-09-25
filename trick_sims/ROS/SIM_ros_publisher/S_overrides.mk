
ROS_HOME = /opt/ros/kinetic
TRICK_EXCLUDE += :${ROS_HOME}

TRICK_CFLAGS += -I../models -I${ROS_HOME}/include
TRICK_CXXFLAGS += -I../models -I${ROS_HOME}/include -Wno-shadow -Wno-unused-parameter

TRICK_EXEC_LINK_LIBS += -L${ROS_HOME}/lib -lroscpp -lrosconsole -lroscpp_serialization

$(MODEL_cpp_OBJ) : | ../models/ros_comm/Num.h

MESSAGE_FILES = ../models/ros_comm/Num.h

${MESSAGE_FILES} : %.h : %.msg
	cd ${<D} && ${ROS_HOME}/lib/gencpp/gen_cpp.py ${<F} -Itrick_msgs:. -Istd_msgs:${ROS_HOME}/share/std_msgs/cmake/../msg -p trick_msgs -o . -e /opt/ros/jade/share/gencpp

clean: clean_msg_headers

clean_msg_headers:
	rm -f ${HOME}/trick_models/ros_comm/Num.h
