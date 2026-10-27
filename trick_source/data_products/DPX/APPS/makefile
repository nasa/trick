
SHELL = /bin/sh

all: apps

apps: gxflot


gxflot:
	$(MAKE) -C GXPLOT

clean:
	$(MAKE) -C GXPLOT clean

spotless: clean
	$(MAKE) -C GXPLOT spotless
