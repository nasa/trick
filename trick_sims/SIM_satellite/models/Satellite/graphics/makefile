CPP      = g++
CC       = gcc

ifeq ($(shell uname), Darwin)
       GL_LIBS  = -framework GLUT -framework OpenGL
       TC_LIBS = -L${TRICK_HOME}/lib -ltrick_comm
       LDFLAGS =
else
       GL_LIBS  = -lGL -lGLU -lglut
       TC_LIBS = -L${TRICK_HOME}/lib -ltrick_comm -lrt
       LDFLAGS = -Wl,--export-dynamic
endif

CFLAGS = -I${TRICK_HOME}/include

MAIN     = Scene

OBJECTS = Camera.o Scene.o SceneElement.o SatelliteModel.o Ball.o

.c.o:
	${CC} ${CFLAGS} ${INCDIRS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} ${INCDIRS} -c $<
	echo $(NARF)

all: Scene

Scene: $(OBJECTS)
	$(CPP) -o Scene $(OBJECTS) $(GL_LIBS) $(TC_LIBS) ${LDFLAGS} -lm

clean:
	rm -f *.o
	rm -f Scene
