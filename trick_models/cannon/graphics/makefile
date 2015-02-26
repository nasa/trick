CC       = cc

ifeq ($(TRICK_HOST_TYPE), Darwin)
	GL_LIBS  = -framework GLUT -framework OpenGL
else
	GL_LIBS  = -lGL -lGLU -lglut
endif
TC_LIBS  = -L${TRICK_HOME}/lib_${TRICK_HOST_CPU} -ltrick_comm -ltrick_stubs
CFLAGS   = -I${TRICK_HOME}/trick_source
MAIN     = cannon

SRC        = $(wildcard *.c)
OBJECTS    = $(subst .c,.o,$(SRC))

all: $(OBJECTS) $(MAIN)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(MAIN): $(OBJECTS)
	$(CC) -o cannon $(OBJECTS) $(GL_LIBS) $(TC_LIBS) -lm

clean:
	rm -f *.o
	rm -f cannon
