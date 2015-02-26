
TRICK_CFLAGS += -I${TRICK_HOME}/trick_models
TRICK_CXXFLAGS += -I${TRICK_HOME}/trick_models

S_main: tc_server tc_client
tc_server: tc_server.c
	${TRICK_CC} ${TRICK_CFLAGS} -I${TRICK_HOME}/trick_source -o $@ $< -L${TRICK_LIB_DIR} -ltrick_comm ${TRICK_EXEC_LINK_LIBS}

tc_client: tc_client.c
	${TRICK_CC} ${TRICK_CFLAGS} -I${TRICK_HOME}/trick_source -o $@ $< -L${TRICK_LIB_DIR} -ltrick_comm ${TRICK_EXEC_LINK_LIBS}

clean: clean_tc_apps
spotless: clean_tc_apps

clean_tc_apps:
	${RM} -rf tc_server tc_client

