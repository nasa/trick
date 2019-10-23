include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = cc
CPP = c++
CURL = curl
MV = mv
CP = cp
MKDIR = mkdir

CFLAGS = -g -Wall
CPPFLAGS = -g -Wall -std=c++11

INCLUDE_DIRS = -Iinclude -I${TRICK_HOME}/include

OBJDIR = obj
LIBDIR = lib
INCDIR = include

#TRICK_LIB_DIR comes from Makefile.common

MONGOOSE_OBJS = ${OBJDIR}/mongoose.o
MONGOOSE_INCDIR = ${TRICK_HOME}/include/mongoose

TRICK_HTTP_OBJS = \
       ${OBJDIR}/VariableServerSession.o \
       ${OBJDIR}/VariableServerVariable.o \
       ${OBJDIR}/http_GET_handlers.o \
       ${OBJDIR}/WebServer.o \
       ${OBJDIR}/simpleJSON.o

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: build_libs

build_libs: ${TRICK_LIB_DIR}/libmongoose.a ${TRICK_LIB_DIR}/libtrickHTTP.a

${TRICK_LIB_DIR}/libmongoose.a: ${LIBDIR}/libmongoose.a ${MONGOOSE_INCDIR}/mongoose.h
	$(MV) ${LIBDIR}/libmongoose.a ${TRICK_LIB_DIR}

${TRICK_LIB_DIR}/libtrickHTTP.a: ${LIBDIR}/libtrickHTTP.a
	$(MV) ${LIBDIR}/libtrickHTTP.a ${TRICK_LIB_DIR}

${MONGOOSE_INCDIR}/mongoose.h: mongoose.h | ${MONGOOSE_INCDIR}
	$(CP) mongoose.h ${MONGOOSE_INCDIR}/mongoose.h

${MONGOOSE_INCDIR}: 
	$(MKDIR) -p ${MONGOOSE_INCDIR}

# Build Mongoose Library
mongoose.h:
	$(CURL) --retry 4 -O https://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.h

mongoose.c:
	$(CURL) --retry 4 -O https://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.c

${MONGOOSE_OBJS}: mongoose.h mongoose.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o ${MONGOOSE_OBJS} mongoose.c

${LIBDIR}/libmongoose.a: ${MONGOOSE_OBJS} | ${LIBDIR}
	ar crs $@ ${MONGOOSE_OBJS}


# Build Trick HTTP Server Library
$(TRICK_HTTP_OBJS): $(OBJDIR)/%.o : src/%.cpp ${MONGOOSE_INCDIR}/mongoose.h | $(OBJDIR)
	$(CPP) $(CPPFLAGS) ${INCLUDE_DIRS} -c $< -o $@

${LIBDIR}/libtrickHTTP.a: ${TRICK_HTTP_OBJS} | ${LIBDIR}
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
	${RM} ${LIBDIR}
	${RM} mongoose.h mongoose.c

