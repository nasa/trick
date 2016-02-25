
TRICK_CFLAGS += -I.
TRICK_CXXFLAGS += -I.

S_main: tc_server
tc_server: tc_server.c
	${TRICK_CC} -o $@ $<

clean: clean_tc_apps
spotless: clean_tc_apps

clean_tc_apps:
	${RM} -f tc_server

