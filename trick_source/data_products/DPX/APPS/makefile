
# $Id: makefile 640 2010-04-14 16:25:51Z wwoodard $

SHELL = /bin/sh

all: apps

apps: gxflot

ifneq ($(FERMI_WARE_HOME),)
apps: fxplot
endif

fxplot:
	$(MAKE) -C FXPLOT

gxflot:
	$(MAKE) -C GXPLOT

clean:
	$(MAKE) -C FXPLOT clean
	$(MAKE) -C GXPLOT clean

spotless: clean
	$(MAKE) -C FXPLOT spotless
	$(MAKE) -C GXPLOT spotless
