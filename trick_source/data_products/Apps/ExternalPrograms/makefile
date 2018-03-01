CC            = gcc 

ifndef TRICK_HOST_CPU
TRICK_HOST_CPU := $(shell trick-gte TRICK_HOST_CPU)
endif

ifndef TRICK_HOST_TYPE
TRICK_HOST_TYPE := $(shell trick-gte TRICK_HOST_TYPE)
endif

OBJDIR        = object_${TRICK_HOST_CPU}
TRICK_CFLAGS = -I$(TRICK_HOME)/trick_source -fPIC
OBJECTS0 = $(OBJDIR)/mat_lvlh_inertial.so \
           $(OBJDIR)/quat_to_mat.so \
	    $(OBJDIR)/dp_body_rates.so \
           $(OBJDIR)/dp_euler_ang.so \
           $(OBJDIR)/dp_inrtl_attitude.so \
           $(OBJDIR)/dp_lvlh_attitude.so \
           $(OBJDIR)/dp_lvlh.so \
           $(OBJDIR)/dp_payload_attitude.so \
           $(OBJDIR)/dp_quat_inrtl_attitude.so \
           $(OBJDIR)/dp_quat_lvlh_attitude.so \
           $(OBJDIR)/dp_test.so \
           $(OBJDIR)/dp_subtract.so \
           $(OBJDIR)/dp_vector_magnitude.so 

OBJECTS1 = $(OBJDIR)/dp_test.so \
           $(OBJDIR)/dp_subtract.so

LD_SHARED = ld -shared 

ifeq ($(TRICK_HOST_TYPE),Linux)
TRICK_CFLAGS += -Wall
endif

ifeq ($(TRICK_HOST_TYPE),Darwin)
CC = cc
LD_SHARED = $(CC) -bundle
TRICK_CFLAGS += -Wall
endif

top:
	@ if [ "${TRICK_HOME}" != "" ] ; then \
            $(MAKE) -e all ; \
         else \
            echo "External programs need Trick. Not compiling." ; \
         fi

all: object_dir objects install

#objects: $(OBJECTS0)
objects: $(OBJECTS1)
     
object_dir:
	@ mkdir -p $(OBJDIR)

$(OBJDIR)/mat_lvlh_inertial.so: mat_lvlh_inertial.c 
	$(CC) $(TRICK_CFLAGS) -c mat_lvlh_inertial.c -o $(OBJDIR)/mat_lvlh_inertial.o
	$(LD_SHARED) -o $(OBJDIR)/mat_lvlh_inertial.so $(OBJDIR)/mat_lvlh_inertial.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/quat_to_mat.so: quat_to_mat.c 
	$(CC) $(TRICK_CFLAGS) -c quat_to_mat.c -o $(OBJDIR)/quat_to_mat.o
	$(LD_SHARED) -o $(OBJDIR)/quat_to_mat.so $(OBJDIR)/quat_to_mat.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_body_rates.so: dp_body_rates.c
	$(CC) $(TRICK_CFLAGS) -c dp_body_rates.c -o $(OBJDIR)/dp_body_rates.o
	$(LD_SHARED) -o $(OBJDIR)/dp_body_rates.so $(OBJDIR)/dp_body_rates.o  \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_euler_ang.so: dp_euler_ang.c 
	$(CC) $(TRICK_CFLAGS) -c dp_euler_ang.c -o $(OBJDIR)/dp_euler_ang.o
	$(LD_SHARED) -o $(OBJDIR)/dp_euler_ang.so $(OBJDIR)/dp_euler_ang.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_inrtl_attitude.so: dp_inrtl_attitude.c 
	$(CC) $(TRICK_CFLAGS) -c dp_inrtl_attitude.c -o $(OBJDIR)/dp_inrtl_attitude.o
	$(LD_SHARED) -o $(OBJDIR)/dp_inrtl_attitude.so $(OBJDIR)/dp_inrtl_attitude.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_lvlh_attitude.so: dp_lvlh_attitude.c 
	$(CC) $(TRICK_CFLAGS) -c dp_lvlh_attitude.c -o $(OBJDIR)/dp_lvlh_attitude.o
	$(LD_SHARED) -o $(OBJDIR)/dp_lvlh_attitude.so $(OBJDIR)/dp_lvlh_attitude.o \
 	$(OBJDIR)/mat_lvlh_inertial.o $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_lvlh.so: dp_lvlh.c 
	$(CC) $(TRICK_CFLAGS) -c dp_lvlh.c -o $(OBJDIR)/dp_lvlh.o
	$(LD_SHARED) -o $(OBJDIR)/dp_lvlh.so $(OBJDIR)/dp_lvlh.o $(OBJDIR)/mat_lvlh_inertial.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_payload_attitude.so: dp_payload_attitude.c 
	$(CC) $(TRICK_CFLAGS) -c dp_payload_attitude.c -o $(OBJDIR)/dp_payload_attitude.o
	$(LD_SHARED) -o $(OBJDIR)/dp_payload_attitude.so $(OBJDIR)/dp_payload_attitude.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_quat_inrtl_attitude.so: dp_quat_inrtl_attitude.c 
	$(CC) $(TRICK_CFLAGS) -c dp_quat_inrtl_attitude.c -o $(OBJDIR)/dp_quat_inrtl_attitude.o
	$(LD_SHARED) -o $(OBJDIR)/dp_quat_inrtl_attitude.so $(OBJDIR)/dp_quat_inrtl_attitude.o \
	$(OBJDIR)/quat_to_mat.o $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_quat_lvlh_attitude.so: dp_quat_lvlh_attitude.c 
	$(CC) $(TRICK_CFLAGS) -c dp_quat_lvlh_attitude.c -o $(OBJDIR)/dp_quat_lvlh_attitude.o
	$(LD_SHARED) -o $(OBJDIR)/dp_quat_lvlh_attitude.so $(OBJDIR)/dp_quat_lvlh_attitude.o \
			$(OBJDIR)/mat_lvlh_inertial.o $(OBJDIR)/quat_to_mat.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

$(OBJDIR)/dp_test.so: dp_test.c 
	$(CC) $(TRICK_CFLAGS) -c dp_test.c -o $(OBJDIR)/dp_test.o
	$(LD_SHARED) -o $(OBJDIR)/dp_test.so $(OBJDIR)/dp_test.o

$(OBJDIR)/dp_subtract.so: dp_subtract.c 
	$(CC) $(TRICK_CFLAGS) -c dp_subtract.c -o $(OBJDIR)/dp_subtract.o
	$(LD_SHARED) -o $(OBJDIR)/dp_subtract.so $(OBJDIR)/dp_subtract.o

$(OBJDIR)/dp_vector_magnitude.so: dp_vector_magnitude.c
	$(CC) $(TRICK_CFLAGS) -c dp_vector_magnitude.c -o $(OBJDIR)/dp_vector_magnitude.o
	$(LD_SHARED) -o $(OBJDIR)/dp_vector_magnitude.so $(OBJDIR)/dp_vector_magnitude.o \
                     $(TRICK_HOME)/lib_$(TRICK_HOST_CPU)/libtrick_math.a -lc -lm

install: objects
	#@ /bin/cp $(OBJDIR)/*.so ${TRICK_HOME}/lib_${TRICK_HOST_CPU}

real_clean:
	@ if [ "${TRICK_HOME}" != "" ] ; then \
            $(MAKE) -e clean ; \
         else \
            echo "External programs need Trick. Not cleaning." ; \
         fi

clean:
	rm -rf object_${TRICK_HOST_CPU}
	rm -rf ${TRICK_HOME}/lib_${TRICK_HOST_CPU}/*.so

