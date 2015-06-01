
ifdef TRICK_HOME
ifneq ($(STAND_ALONE), 1)

-include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

OBJECT_FILES =  \
       $(OBJ_DIR)/tc_accept.o \
       $(OBJ_DIR)/tc_blockio.o \
       $(OBJ_DIR)/tc_broadcast_conninfo.o \
       $(OBJ_DIR)/tc_clock_init.o \
       $(OBJ_DIR)/tc_clock_time.o \
       $(OBJ_DIR)/tc_connect.o \
       $(OBJ_DIR)/tc_dev_copy.o \
       $(OBJ_DIR)/tc_disconnect.o \
       $(OBJ_DIR)/tc_error.o \
       $(OBJ_DIR)/tc_init.o \
       $(OBJ_DIR)/tc_isValid.o \
       $(OBJ_DIR)/tc_listen.o \
       $(OBJ_DIR)/tc_multiconnect.o \
       $(OBJ_DIR)/tc_pending.o \
       $(OBJ_DIR)/tc_read.o \
       $(OBJ_DIR)/tc_read_byteswap.o \
       $(OBJ_DIR)/tc_set_blockio.o \
       $(OBJ_DIR)/tc_write_byteswap.o \
       $(OBJ_DIR)/tc_write.o \
       $(OBJ_DIR)/tc_init_mcast_client.o \
       $(OBJ_DIR)/tc_init_mcast_server.o \
       $(OBJ_DIR)/tc_init_udp_client.o \
       $(OBJ_DIR)/tc_init_udp_server.o \
       $(OBJ_DIR)/trick_bswap_buffer.o \
       $(OBJ_DIR)/trick_byteswap.o \
       $(OBJ_DIR)/trick_error_hndlr.o

-include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

ifneq ($(DMTCP),)
TRICK_CFLAGS += -D_DMTCP -I$(DMTCP)/dmtcpaware
endif

SEPARATE_LIBS = $(TRICK_LIB_DIR)/libtrick_comm.a

default: $(SEPARATE_LIBS)
trick: $(SEPARATE_LIBS)

stand_alone:
	@ $(MAKE) STAND_ALONE=1

clean_stand_alone:
	@ $(MAKE) STAND_ALONE=1 clean

$(TRICK_LIB_DIR)/libtrick_comm.a : $(OBJECT_FILES) $(IO_OBJS) $(OBJ_DIR)_sa/stubs.o | $(TRICK_LIB_DIR)
	ar cr $@ $(OBJECT_FILES) $(IO_OBJS) $(OBJ_DIR)_sa/stubs.o

$(OBJ_DIR)_sa/stubs.o: $(SRC_DIR)stubs.c | $(OBJ_DIR)_sa
	$(CD_CMD) $(TRICK_CC) $(TRICK_CFLAGS) -DSTAND_ALONE -c ${<F} -o $(UP_DIR)$@

$(OBJ_DIR)_sa:
	@ mkdir -p $@

clean: clean_sa

clean_sa:
	${RM} -rf $(OBJ_DIR)_sa

else

include makefile.stand_alone

endif
else

include makefile.stand_alone

endif

