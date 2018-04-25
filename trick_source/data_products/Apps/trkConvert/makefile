CPP      = g++
CC       = gcc

CXXFLAGS = 

MAIN     = trkConvert

OBJECTS = CSV_Formatter.o Varlist_Formatter.o trkConvert.o

.c.o:
	${CC} ${CFLAGS} ${INCDIRS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} ${INCDIRS} -c $<

all: install

trkConvert: $(OBJECTS)
	$(CPP) -o trkConvert $(OBJECTS) 

install: trkConvert
	cp trkConvert $${TRICK_HOME}/bin/trick-trkConvert

clean:
	rm -f *.o
	rm -f trkConvert
