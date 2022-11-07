
TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

TRICK_CXXFLAGS += -std=c++11

test: setup

# This is a workaround so that the sim runs with RUN_test/setup.py before the actual unit test run.
# setup.py adds hardcoded data to the sim and dumps a checkpoint, and unit_test.py loads that checkpoint and checks all the values.

setup: T_main_${TRICK_HOST_CPU}.exe 
	echo "Running Setup Job"
	./T_main_${TRICK_HOST_CPU}.exe RUN_test/setup.py

clean: checkpoint_clean

checkpoint_clean:
	rm -f RUN_test/chkpnt_in
