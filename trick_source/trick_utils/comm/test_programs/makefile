
#####################################################################
# Makefile:
#    This is a makefile for maintaining the TC test programs 
#
#####################################################################
# Creation:
#    Author: Keith Vetter 
#    Date:   6/19/00 
#####################################################################

-include ${TRICK_HOME}/makefiles/Makefile.common

ifndef TRICK_HOST_CPU
TRICK_HOST_CPU := $(shell trick-gte TRICK_HOST_CPU)
endif

ifndef TRICK_HOST_TYPE
TRICK_HOST_TYPE := $(shell trick-gte TRICK_HOST_TYPE)
endif


CC = cc 
FLAGS =
ifeq ($(TRICK_HOST_TYPE), Darwin)
 LIBS =
else
 LIBS = -lrt
endif

ifeq ($(TRICK_FORCE_32BIT), 1)
 FLAGS += -m32
endif

#####################################################################
##                       UTILITY DEFINITIONS                       ##
#####################################################################

LIB_DIR = ../object_$(shell uname -s)
COMM_LIB = ${LIB_DIR}/libtrick_comm.a
STUBS_LIB = ${LIB_DIR}/libtrick_stubs.a
LIBTC_LIB = $(COMM_LIB) $(STUBS_LIB) -lpthread ${LIBS}
   

#####################################################################
##                     FILE NAME DEFINITIONS                       ##
#####################################################################

OBJ_DIR = object_${TRICK_HOST_CPU}

TEST_PRGMS = \
 $(OBJ_DIR)/tc_server\
 $(OBJ_DIR)/tc_client\
 $(OBJ_DIR)/tc_multi_serv_example \
 $(OBJ_DIR)/tc_multi_client_example \
 $(OBJ_DIR)/dr_client

#####################################################################
##               TARGET & DEPENDENCY DEFINITIONS                   ##
#####################################################################

default:
	@ if [ "${TRICK_HOME}" != "" ] ; then \
	     if [ "`uname -s`" = "SunOS" ] ; then \
	        make -e test "FLAGS=-I${TRICK_HOME}/trick_source -I../include" "LIBS=-lsocket -lnsl" ; \
	     else \
	        make -e test "FLAGS=-I${TRICK_HOME}/trick_source -I../include" "LIBS=${LIBS}" ; \
	     fi \
	  else \
	     if [ "`uname -s`" = "SunOS" ] ; then \
	        make -e test "TRICK_HOST_CPU=`uname -s`" "FLAGS=-DSTAND_ALONE" "LIBS=-lsocket -lnsl" ; \
	     else \
	        make -e test "TRICK_HOST_CPU=`uname -s`" "FLAGS=-DSTAND_ALONE" "LIBS=${LIBS}" ; \
	     fi \
	  fi

clean:
	@ if [ "${TRICK_HOME}" != "" ] ; then \
	     /bin/rm -rf object_$${TRICK_HOST_CPU} ; \
	     echo "Removed object_$${TRICK_HOST_CPU}" ; \
	  else \
	     /bin/rm -rf object_`uname -s` ; \
	     echo "Removed object_`uname -s`" ; \
	  fi

test: $(OBJ_DIR) $(TEST_PRGMS)
	@ echo "[32mClient/Server test programs are up to date in $(OBJ_DIR).[00m"

$(OBJ_DIR):
	@ mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/tc_server: tc_server.c
	$(CC) $(FLAGS) -o $@ ${@F}.c $(LIBTC_LIB)

$(OBJ_DIR)/tc_client: tc_client.c
	$(CC) $(FLAGS) -o $@ ${@F}.c $(LIBTC_LIB)

$(OBJ_DIR)/tc_multi_serv_example: tc_multi_serv_example.c
	$(CC) $(FLAGS) -o $@ ${@F}.c $(LIBTC_LIB)

$(OBJ_DIR)/tc_multi_client_example: tc_multi_client_example.c
	$(CC) $(FLAGS) -o $@ ${@F}.c $(LIBTC_LIB)

$(OBJ_DIR)/dr_client: dr_client.c
	$(CC) $(FLAGS) -o $@ ${@F}.c $(LIBTC_LIB)

$(COMM_LIB):
	@ cd .. ; make STAND_ALONE=1 trick_comm

$(STUBS_LIB):
	@ cd .. ; make STAND_ALONE=1 trick_stubs_lib


$(OBJ_DIR)/dr_client: $(COMM_LIB)
$(OBJ_DIR)/dr_client: $(STUBS_LIB)
$(OBJ_DIR)/tc_server: $(COMM_LIB)
$(OBJ_DIR)/tc_server: $(STUBS_LIB)
$(OBJ_DIR)/tc_client: $(COMM_LIB)
$(OBJ_DIR)/tc_client: $(STUBS_LIB)
$(OBJ_DIR)/tc_multi_serv_example: $(COMM_LIB)
$(OBJ_DIR)/tc_multi_client_example: $(COMM_LIB)
$(OBJ_DIR)/tc_multi_serv_example: $(STUBS_LIB)
$(OBJ_DIR)/tc_multi_client_example: $(STUBS_LIB)
