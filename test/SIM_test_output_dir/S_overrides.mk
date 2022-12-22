
TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

clean: output_clean

output_clean:
	rm -rf sim_output
