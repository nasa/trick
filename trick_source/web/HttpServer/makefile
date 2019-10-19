RM = rm -rf
CC = cc
CPP = c++
CURL = curl
MV = mv
CP = cp
MKDIR = mkdir

CFLAGS = -g -Wall
INCLUDE_DIRS = -Iinclude -I${TRICK_HOME}/include

OBJDIR = obj
LIBDIR = lib
INCDIR = include

TRICK_LIBDIR = ${TRICK_HOME}/lib
TRICK_INCDIR = ${TRICK_HOME}/include

MONGOOSE_OBJS = ${OBJDIR}/mongoose.o
LIB_MONGOOSE = libmongoose.a 
MONGOOSE_INCDIR = ${TRICK_INCDIR}/mongoose

LIB_TRICK_HTTP = libtrickHTTP.a
TRICK_HTTP_OBJS = \
       ${OBJDIR}/VariableServerSession.o \
       ${OBJDIR}/VariableServerVariable.o \
       ${OBJDIR}/http_GET_handlers.o \
       ${OBJDIR}/WebServer.o \
       ${OBJDIR}/simpleJSON.o

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: install

install: build_libs | ${MONGOOSE_INCDIR}
	$(MV) ${LIBDIR}/libmongoose.a     ${TRICK_LIBDIR}/libmongoose.a
	$(MV) ${LIBDIR}/libtrickHTTP.a    ${TRICK_LIBDIR}/libtrickHTTP.a
	# 
	$(MV) mongoose.h                  ${TRICK_INCDIR}/mongoose/mongoose.h
	# 
	#$(CP) include/WebServer.hh      ${TRICK_INCDIR}/trick/WebServer.hh
	#$(CP) include/WebSocketSession.hh ${TRICK_INCDIR}/trick/WebSocketSession.hh
	#
	#$(CP) HttpServer.sm               ${TRICK_HOME}/share/trick/sim_objects/HttpServer.sm

${MONGOOSE_INCDIR}: 
	$(MKDIR) -p ${MONGOOSE_INCDIR}

build_libs: ${LIBDIR}/${LIB_TRICK_HTTP} ${LIBDIR}/libmongoose.a

# Build Mongoose Library
mongoose.h:
	$(CURL) --retry 4 -O http://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.h

mongoose.c:
	$(CURL) --retry 4 -O http://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.c

${MONGOOSE_OBJS}: mongoose.h mongoose.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o ${MONGOOSE_OBJS} mongoose.c

${LIBDIR}/${LIB_MONGOOSE}: ${MONGOOSE_OBJS} | ${LIBDIR}
	ar crs $@ ${MONGOOSE_OBJS}


# Build Trick HTTP Server Library
$(TRICK_HTTP_OBJS): $(OBJDIR)/%.o : src/%.cpp | $(OBJDIR)
	$(CPP) $(CFLAGS) ${INCLUDE_DIRS} -c $< -o $@

${LIBDIR}/${LIB_TRICK_HTTP}: ${TRICK_HTTP_OBJS} | ${LIBDIR}
	ar crs $@ ${TRICK_HTTP_OBJS}

# ---------------------------------

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBDIR}:
	mkdir -p ${LIBDIR}

clean:
	${RM} *~
	${RM} ${OBJDIR}

spotless: clean
	${RM} ${LIBDIR}/${LIB_TRICK_HTTP}


