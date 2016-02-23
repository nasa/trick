
TRICK_CFLAGS += -I.
TRICK_CXXFLAGS += -I.

S_main: tc_server
tc_server: tc_server.c
	${TRICK_CC} ${TRICK_CFLAGS} ${TRICK_SYSTEM_CFLAGS} -I${TRICK_HOME}/trick_source -o $@ $< -L${TRICK_LIB_DIR} -ltrick_comm ${TRICK_EXEC_LINK_LIBS}

clean: clean_tc_apps
spotless: clean_tc_apps

clean_tc_apps:
	${RM} -f tc_server

