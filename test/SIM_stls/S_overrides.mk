
TRICK_CFLAGS += -I./models -DTRICK_UNIT_TEST
TRICK_CXXFLAGS += -I./models

TRICK_CXXFLAGS += -DTRICK_UNIT_TEST

clean: checkpoint_clean

checkpoint_clean:
	rm -f RUN_test/chkpnt_in
