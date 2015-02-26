#
# Makefile for Data Products Libs And Apps
#

#
# $Id: makefile 1899 2011-08-29 21:55:23Z wwoodard $
#

include ${TRICK_HOME}/makefiles/Makefile.common

LIB_DIR = lib_${TRICK_HOST_CPU}

LIBDIRS = Var \
    Log \
    EQParse

ifneq ($(FERMI_WARE_HOME),)
LIBDIRS += $(FERMI_WARE_HOME)
endif

APPDIRS = DPX \
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

