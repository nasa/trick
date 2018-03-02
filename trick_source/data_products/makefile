#
# Makefile for Data Products Libs And Apps
#

TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)

include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

LIB_DIR = lib_${TRICK_HOST_CPU}

LIBDIRS = Var \
    Log \
    EQParse \
    units

ifneq ("$(wildcard fermi-ware)","")
LIBDIRS += fermi-ware
endif

APPDIRS = DPX \
    Apps/trkConvert \
    Apps/Trk2csv \
    Apps/ExternalPrograms

all: $(LIBDIRS) $(APPDIRS)
	@ echo "[32m== Trick data_products make complete ==[00m"

.PHONY: $(LIBDIRS) $(APPDIRS)

$(LIBDIRS):
	@ echo ; echo "==: Compile data_products/$@"
	@ $(MAKE) -C $@ 

$(APPDIRS): $(LIBDIRS) 
	@ echo ; echo "==: Compile data_products/$@"
	@ $(MAKE) -C $@

real_clean: clean

clean:
	@ for i in $(LIBDIRS) ; do \
	       $(MAKE) -C $$i real_clean ; \
          done
	@ for i in $(APPDIRS) ; do \
	       $(MAKE) -C $$i real_clean ; \
          done
	@ rm -rf $(LIB_DIR)

