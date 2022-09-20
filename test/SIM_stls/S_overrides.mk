
TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

TRICK_CXXFLAGS += -std=c++11


# /Users/jndeans/trick/trick_test/SIM_stls.xml: setup
test: setup

setup: T_main_${TRICK_HOST_CPU}.exe 
	echo "Running Setup Job"
	./T_main_${TRICK_HOST_CPU}.exe RUN_test/setup.py

clean: checkpoint_clean

checkpoint_clean:
	rm -f RUN_test/chkpnt_in1234