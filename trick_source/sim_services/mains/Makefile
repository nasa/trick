
include ${TRICK_HOME}/makefiles/Makefile.common

LIB_DIR = ${TRICK_HOME}/lib_${TRICK_HOST_CPU}

all: $(LIB_DIR)/master.o

trick: $(LIB_DIR)/master.o

$(OBJ_DIR)/master.o : master.cpp | $(OBJ_DIR)
	$(TRICK_CPPC) $(TRICK_CXXFLAGS) -c $< -o $@

$(LIB_DIR)/master.o : $(OBJ_DIR)/master.o | $(LIB_DIR)
	$(CP) $< $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

clean: 
	$(RM) -rf $(OBJ_DIR)

real_clean: clean

depend:
	@ $(TRICK_CPPC) -MM $(TRICK_CXXFLAGS) master.cpp >> $(DEPTEMPFILE)
	@ $(PERL) ${TRICK_HOME}/bin/depend_cp $(DEPTEMPFILE) > $(DEPFILE)
	@ $(RM) $(DEPTEMPFILE)
	@ echo "Created dependency file $(DEPFILE)"


-include Makefile_deps

