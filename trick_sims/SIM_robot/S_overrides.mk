TRICK_CFLAGS += -I./models -I${TRICK_HOME} -g -O0 -Wall -Wextra -Wshadow
TRICK_CXXFLAGS += -I./models -I${TRICK_HOME} -g -O0 -Wall -Wextra -Wshadow

.PHONY: clean_graphics

all: models/graphics/build/RobotDisplay.jar

spotless: clean_graphics

models/graphics/build/RobotDisplay.jar:
	${MAKE} -C ./models/graphics

clean_graphics:
	${MAKE} -C ./models/graphics clean
