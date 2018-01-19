CPP      = g++
CC       = gcc

CXXFLAGS = 

MAIN     = trkConvert

OBJECTS = CSV_Formatter.o Varlist_Formatter.o trkConvert.o

.c.o:
	${CC} ${CFLAGS} ${INCDIRS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} ${INCDIRS} -c $<

all: trkConvert

trkConvert: $(OBJECTS)
	$(CPP) -o trkConvert $(OBJECTS) 

clean:
	rm -f *.o
	rm -f trkConvert
