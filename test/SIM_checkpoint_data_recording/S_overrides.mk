TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

clean_logs:
	rm -f RUN_test*/log_foo* RUN_test*/log_enums*

clean: clean_logs