TRICK_CFLAGS += -I${JEOD_HOME}/models
TRICK_CFLAGS += -I${ER7_UTILS_HOME}
TRICK_CXXFLAGS += -I${JEOD_HOME}/models
TRICK_CXXFLAGS += -I${ER7_UTILS_HOME}


all: build_ephem_files

build_ephem_files:
	@ echo "Building JPL DE405 ephemeris files" ;\
	cd ${JEOD_HOME}/models/environment/ephemerides/de4xx_ephem/data ;\
	make

