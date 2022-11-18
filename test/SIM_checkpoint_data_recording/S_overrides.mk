TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

clean_logs:
	rm RUN_test*/log_foo*

clean: clean_logs