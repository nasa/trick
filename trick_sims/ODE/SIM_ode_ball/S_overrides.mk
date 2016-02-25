
ODE_HOME = ${HOME}/ode-0.14

TRICK_ICG_EXCLUDE = ${ODE_HOME}
TRICK_ICG_NOCOMMENT = ${ODE_HOME}
TRICK_SWIG_EXCLUDE = ${ODE_HOME}

TRICK_CFLAGS += -I./models -I../models -I${ODE_HOME}/include -I${ODE_HOME}
TRICK_CXXFLAGS += -I./models -I../models -I${ODE_HOME}/include -I${ODE_HOME}

TRICK_EXEC_LINK_LIBS += -L${ODE_HOME}/drawstuff/src/.libs -ldrawstuff -L${ODE_HOME}/ode/src/.libs -lode -lGLU -lGL -L/usr/X11R6/lib -lXext -lX11
