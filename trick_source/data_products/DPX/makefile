
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
	@ echo -e "\e[34m[---Making Documentation---]\e[0m"
	doxygen DPX.doxygen

testing: $(LIBDIRS)
	@ echo -e "\e[34m[---Building TESTING---]\e[0m"
	$(MAKE) -C TESTING
	@ echo ""

$(LIBDIRS):
	@ echo -e "\e[34m[---Building $@---]\e[0m"
	@ $(MAKE) -C $@
	@ echo ""

$(APPDIRS): $(LIBDIRS)
	@ echo -e "\e[34m[---Building $@---]\e[0m"
	@ $(MAKE) -C $@
	@ echo ""

clean:
	@ for i in $(LIBDIRS) ; do \
	       $(MAKE) -C $$i clean ; \
          done
	@ for i in $(APPDIRS) ; do \
	       $(MAKE) -C $$i clean ; \
          done

	@ $(MAKE) -C TESTING spotless
	@ $(MAKE) -C APPS spotless
	
	${RM} DOCS
	${RM} ${LIBDIR}
	${RM} *~
	${RM} *.o

real_clean: clean

