

RM = rm -rf
CC = cc
CPP = c++

DECL_DIR = ..
GTEST_DIR = /Users/penn/gtest-1.7.0

CFLAGS += -g -Wall -Wextra -I$(GTEST_DIR)/include -I$(DECL_DIR)/include

LIBS = ../lib/libDecl.a

TESTS = PrimTypeSpecTest CompTypeSpecTest TypeDictionaryTest EnumTypeSpecTest ArrayTypeSpecTest LexicalAnalyzerTest ParsedDeclarationTest AllocInfoTest ClassicChkPtAgentTest

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(TESTS)

test: $(TESTS)
	./PrimTypeSpecTest       --gtest_output=xml:XMLtestReports/PrimTypeSpecTest.xml
	./CompTypeSpecTest       --gtest_output=xml:XMLtestReports/CompTypeSpecTest.xml
	./TypeDictionaryTest     --gtest_output=xml:XMLtestReports/TypeDictionaryTest.xml
	./EnumTypeSpecTest       --gtest_output=xml:XMLtestReports/EnumTypeSpecTest.xml
	./ArrayTypeSpecTest      --gtest_output=xml:XMLtestReports/ArrayTypeSpecTest.xml
	./LexicalAnalyzerTest    --gtest_output=xml:XMLtestReports/LexicalAnalyzerTest.xml
	./ParsedDeclarationTest  --gtest_output=xml:XMLtestReports/ParsedDeclarationTest.xml
	./AllocInfoTest          --gtest_output=xml:XMLtestReports/AllocInfoTest.xml
	./ClassicChkPtAgentTest  --gtest_output=xml:XMLtestReports/ClassicChkPtAgentTest.xml

clean :
	rm -f $(TESTS) gtest.a gtest_main.a
	rm -f *.o
	rm -rf XMLtestReports

gtest-all.o :
	$(CPP) -I$(GTEST_DIR) $(CFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o :
	$(CPP) -I$(GTEST_DIR) $(CFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

DataTypeTestSupport.o : DataTypeTestSupport.cpp
	$(CPP) $(CFLAGS) -c $<

TypeDictionaryTest.o : TypeDictionaryTest.cpp
	$(CPP) $(CFLAGS) -c $<

TypeDictionaryTest : TypeDictionaryTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

PrimTypeSpecTest.o : PrimTypeSpecTest.cpp
	$(CPP) $(CFLAGS) -c $<

PrimTypeSpecTest : PrimTypeSpecTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

CompTypeSpecTest.o : CompTypeSpecTest.cpp
	$(CPP) $(CFLAGS) -c $<

CompTypeSpecTest : CompTypeSpecTest.o DataTypeTestSupport.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

EnumTypeSpecTest.o : EnumTypeSpecTest.cpp
	$(CPP) $(CFLAGS) -c $<

EnumTypeSpecTest : EnumTypeSpecTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

ArrayTypeSpecTest.o : ArrayTypeSpecTest.cpp
	$(CPP) $(CFLAGS) -c $<

ArrayTypeSpecTest : ArrayTypeSpecTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

LexicalAnalyzerTest.o : LexicalAnalyzerTest.cpp
	$(CPP) $(CFLAGS) -c $<

LexicalAnalyzerTest : LexicalAnalyzerTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

ParsedDeclarationTest.o : ParsedDeclarationTest.cpp
	$(CPP) $(CFLAGS) -c $<

ParsedDeclarationTest : ParsedDeclarationTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

AllocInfoTest.o : AllocInfoTest.cpp
	$(CPP) $(CFLAGS) -c $<

AllocInfoTest : AllocInfoTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

ClassicChkPtAgentTest.o : ClassicChkPtAgentTest.cpp
	$(CPP) $(CFLAGS) -c $<

ClassicChkPtAgentTest : ClassicChkPtAgentTest.o DataTypeTestSupport.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

