
# $Id: makefile 3159 2013-08-15 22:43:46Z lmlandry $

#
# This makefile is used to build trick utils standalone.
# That is, without Trick.
#
# It does NOT create lib or object dirs for each platform.
#
# Once ran, all libraries go into "lib" .

include $(TRICK_HOME)/makefiles/Makefile.common

ifndef TRICK_HOST_CPU
export TRICK_HOST_CPU := $(shell gte TRICK_HOST_CPU)
endif

ifndef TRICK_HOST_TYPE
export TRICK_HOST_TYPE := $(shell gte TRICK_HOST_TYPE)
endif


LIB_DIR = lib_${TRICK_HOST_CPU}

DIRS = comm \
    shm \
    interpolator \
    trick_adt \
    units \
	math            # build last becuase of gsl dependency issues

trick: all

all:
	@ for i in $(DIRS) ; do \
         echo ; echo "==: Compile trick_utils/$$i" ; \
         echo -n "==Start "; date "+%H:%M:%S" ;\
         cd $$i ; \
         $(MAKE) trick ; \
         if [ $$? -ne 0 ] ; then \
            exit 1 ; \
         fi ; \
         cd .. ; \
         echo -n "==Stop "; date "+%H:%M:%S" ;\
      done
	@ if [ "$(STAND_ALONE)" = "1" ] ; then \
          echo "[32m== Trick utils (stand alone) make complete ==[00m" ; \
      else \
          echo "[32m== Trick utils make complete ==[00m" ; \
      fi

$(LIB_DIR):
	@ mkdir -p $(LIB_DIR)
	@ echo "$(LIB_DIR) created"

stand_alone:
	@ $(MAKE) STAND_ALONE=1 all

clean_stand_alone:
	@ $(MAKE) STAND_ALONE=1 clean

real_clean: clean clean_stand_alone

clean:
	@ /bin/rm -rf  $(LIB_DIR)
	@ for i in $(DIRS) ; do \
         $(MAKE) -C $$i real_clean ; \
      done
	@ echo " "
	@ if [ "$(STAND_ALONE)" = "1" ] ; then \
          echo "[32mCleaned all of trick_utils (stand alone) !!![00m" ; \
      else \
          echo "[32mCleaned all of trick_utils !!![00m" ; \
      fi
	@ echo " "
