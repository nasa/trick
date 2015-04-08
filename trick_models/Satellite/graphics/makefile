include ${TRICK_HOME}/makefiles/Makefile.common
CPP      = g++
CC       = gcc

ifeq ($(TRICK_HOST_TYPE), Darwin)
       GL_LIBS  = -framework GLUT -framework OpenGL
       TC_LIBS = -L${TRICK_HOME}/lib_${TRICK_HOST_CPU} -ltrick_comm 
       LDFLAGS = 
else
       GL_LIBS  = -lGL -lGLU -lglut
       TC_LIBS = -L${TRICK_HOME}/lib_${TRICK_HOST_CPU} -ltrick_comm -lrt
       LDFLAGS = -Wl,--export-dynamic
endif

CFLAGS = -I${TRICK_HOME}/trick_source

MAIN     = Scene

OBJECTS = Camera.o Scene.o SceneElement.o SatelliteModel.o Ball.o

.c.o:
	${CC} ${CFLAGS} ${INCDIRS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} ${INCDIRS} -c $<

all: Scene

Scene: $(OBJECTS)
	$(CPP) -o Scene $(OBJECTS) $(GL_LIBS) $(TC_LIBS) ${LDFLAGS} -lm

clean:
	rm -f *.o
	rm -f Scene
