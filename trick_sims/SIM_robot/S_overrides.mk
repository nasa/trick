TRICK_CFLAGS += -I./models -I${TRICK_HOME} -g -O0
TRICK_CXXFLAGS += -I./models -I${TRICK_HOME} -g -O0

all: build_graphics_client

spotless: clean_graphics

build_graphics_client:
	${MAKE} -C ./models/graphics

clean_graphics:
	${MAKE} -C ./models/graphics clean
