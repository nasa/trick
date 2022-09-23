
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CXX             = c++
DP_CFLAGS      = -g -I../..
OBJDIR         = object_${TRICK_HOST_CPU}
LIBDIR         = ../../lib_${TRICK_HOST_CPU}
DP_LIBS        = -L$(LIBDIR) -llog -lvar -L$(TRICK_LIB_DIR) -ltrick_units
ASCII_MAIN     = ${TRICK_HOME}/bin/trick-trk2ascii

ifeq ($(TRICK_HOST_TYPE), Linux)
       DP_CFLAGS += -Wall
endif
ifeq ($(TRICK_HOST_TYPE), Darwin)
       DP_CFLAGS += -Wall
       DP_LIBS   += -lc++abi
endif

ifeq ($(TRICK_DP_FORCE_32BIT), 1)
       DP_CFLAGS += -m32
endif

all: $(ASCII_MAIN)

$(ASCII_MAIN): $(OBJDIR)/trk2ascii.o
	$(CXX) $(DP_CFLAGS) -o $(ASCII_MAIN) $(OBJDIR)/trk2ascii.o $(DP_LIBS) $(DL_LIB) -lm

$(OBJDIR)/trk2ascii.o: trk2ascii.cpp | $(OBJDIR)
	$(CXX) $(DP_CFLAGS) -c trk2ascii.cpp -o $(OBJDIR)/trk2ascii.o

clean:
	rm -f trk2ascii
	rm -rf $(OBJDIR)
	rm -rf $(ASCII_MAIN)

real_clean: clean

$(OBJDIR):
	@ mkdir -p $(OBJDIR)

# Dependencies

# Library dependencies
$(ASCII_MAIN): $(LIBDIR)/liblog.a $(LIBDIR)/libvar.a
