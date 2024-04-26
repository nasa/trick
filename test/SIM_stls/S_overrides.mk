
TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

TRICK_CXXFLAGS +=

clean: checkpoint_clean

checkpoint_clean:
	rm -f RUN_test/chkpnt_in
