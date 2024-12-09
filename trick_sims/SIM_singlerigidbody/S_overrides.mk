TRICK_CFLAGS += -Imodels
TRICK_CXXFLAGS += -Imodels

.PHONY: clean_graphics

all: models/singlerigidbodygraphics/build/SingleRigidBodyDisplay.jar

spotless: clean_graphics

models/singlerigidbodygraphics/build/SingleRigidBodyDisplay.jar:
	${MAKE} -C ./models/singlerigidbodygraphics

clean_graphics:
	${MAKE} -C ./models/singlerigidbodygraphics clean
