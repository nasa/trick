
RM = rm -rf
CC = cc
CPP = c++

DECL_DIR = ..

GTEST_HOME = /usr/local

CFLAGS += -g -Wall -Wextra -I$(GTEST_HOME)/include -I$(DECL_DIR)/include

LIBS = -L${DECL_DIR}/lib -lDecl -L${GTEST_HOME}/lib -lgtest -lgtest_main -lpthread

# TESTS = PrimTypeSpecTest CompTypeSpecTest TypeDictionaryTest EnumTypeSpecTest ArrayTypeSpecTest LexicalAnalyzerTest ParsedDeclarationTest AllocInfoTest ClassicChkPtAgentTest

# ==================================================================================
# All tests produced by this Makefile. Add new tests you create to this list.
# ==================================================================================
TESTS = PrimTypeSpecTest \
        CompTypeSpecTest \
        TypeDictionaryTest \
        EnumTypeSpecTest \
        ArrayTypeSpecTest \
        LexicalAnalyzerTest \
        ParsedDeclarationTest \
        AllocInfoTest \
        ClassicChkPtAgentTest

# List of XML files produced by the tests.
unittest_results = $(patsubst %,%.xml,$(TESTS))

# List if Test-specific object files.
unittest_objects = $(patsubst %,%.o,$(TESTS))

# ==================================================================================
# TARGETS
# ==================================================================================
all : test

test: unit_tests $(unittest_results)

unit_tests: $(TESTS)

clean :
	rm -f $(TESTS)
	rm -f *.o
	rm -f *.xml

# ==================================================================================
# Generate JUNIT (XML) Test Results
# ==================================================================================
$(unittest_results): %.xml: %
	./$< --gtest_output=xml:$@

# ==================================================================================
# Build Unit test Objects
# ==================================================================================
$(unittest_objects): %.o: %.cpp
	$(CPP) $(CFLAGS) -c $<

DataTypeTestSupport.o : DataTypeTestSupport.cpp
	$(CPP) $(CFLAGS) -c $<

# ==================================================================================
# Build Unit test programs
# ==================================================================================
$(TESTS) : %: %.o
	$(CPP) -o $@ $^ ${LIBS}

CompTypeSpecTest : DataTypeTestSupport.o 
ClassicChkPtAgentTest : DataTypeTestSupport.o 
