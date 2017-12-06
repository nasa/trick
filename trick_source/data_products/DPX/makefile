
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf

DPX_DIR = `pwd`
LIBDIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}

LIBDIRS = DPM \
          DPC \
          DPV/UTILS

APPDIRS = APPS/GXPLOT

ifneq ("$(wildcard ${TRICK_HOME}/trick_source/data_products/fermi-ware)","")
APPDIRS += APPS/FXPLOT
endif

all: $(LIBDIRS) $(APPDIRS)

.PHONY: $(LIBDIRS) $(APPDIRS)

always_make:

docs: always_make
	@echo "===== Making DOCS ====="
	doxygen DPX.doxygen

testing: $(LIBDIRS)
	@echo "===== Making TESTING ====="
	$(MAKE) -C TESTING

$(LIBDIRS):
	@ echo ; echo "==: Compile $@"
	@ $(MAKE) -C $@

$(APPDIRS): $(LIBDIRS)
	@echo "===== Making APPS ====="
	@ echo ; echo "==: Compile $@"
	@ $(MAKE) -C $@

clean:
	@ for i in $(LIBDIRS) ; do \
	       $(MAKE) -C $$i clean ; \
          done
	@ for i in $(APPDIRS) ; do \
	       $(MAKE) -C $$i clean ; \
          done

	@ $(MAKE) -C TESTING spotless
	@ $(MAKE) -C APPS spotless
	
	@ echo -e "\e[31m\c"
	${RM} DOCS
	${RM} ${LIBDIR}
	${RM} *~
	${RM} *.o
	@ echo -e "\e[32mDirectory cleaned.\e[0m"

real_clean: clean

