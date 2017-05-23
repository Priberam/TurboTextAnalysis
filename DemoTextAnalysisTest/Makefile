#!/bin/bash
# Makefile for DemoTextAnalysisTest
CC = g++
DEBUG = -g
EXEC = DemoTextAnalysisTest

HEADERPATH = .
SOURCEPATH = .
LOCALINCLUDES =  ..

LIBTURBOTEXTANALYSIS = ../TurboTextAnalysis
LIBTURBOPARSER = /pba/workspace/TurboParser/libturboparser
UTIL = /pba/workspace/TurboParser/src/util
CLASSIFIER = /pba/workspace/TurboParser/src/classifier
SEQUENCE = /pba/workspace/TurboParser/src/sequence
TAGGER = /pba/workspace/TurboParser/src/tagger
ENTITYRECOGNIZER = /pba/workspace/TurboParser/src/entity_recognizer
PARSER = /pba/workspace/TurboParser/src/parser
SEMANTICPARSER = /pba/workspace/TurboParser/src/semantic_parser
COREFERENCERESOLVER = /pba/workspace/TurboParser/src/coreference_resolver
MORPHOLOGICALTAGGER = /pba/workspace/TurboParser/src/morphological_tagger

AUXLIBS = /pba/workspace/TurboParser/deps/local/lib
AUXINCLUDES = /pba/workspace/TurboParser/deps/local/include
LOCALDEPSINCLUDES = /pba/workspace/TurboTextAnalysis/deps
#LIBCONFIGINCLUDES = /pba/workspace/TurboTextAnalysis/deps/libconfig-1.4.9/lib
#LIBCONFIG = /pba/workspace/TurboTextAnalysis/deps/libconfig-1.4.9/lib/.libs
#UCIINCLUDES = /pba/lib_deps/icu/include
#UCI = /pba/lib_deps/icu/lib

INCLUDES = -I$(LIBTURBOTEXTANALYSIS) -I$(UTIL) -I$(CLASSIFIER) -I$(SEQUENCE) -I$(TAGGER) -I$(ENTITYRECOGNIZER) -I$(PARSER) -I$(SEMANTICPARSER) -I$(COREFERENCERESOLVER)  -I$(MORPHOLOGICALTAGGER) -I$(LIBTURBOPARSER) -I$(AUXINCLUDES) -I$(LOCALDEPSINCLUDES) -I$(LOCALINCLUDES)  -I$(LIBCONFIGINCLUDES) -I$(UCIINCLUDES)
LIBS = -L$(AUXLIBS) -L$(LIBTURBOTEXTANALYSIS) -L$(LIBTURBOPARSER) -L$(LIBCONFIG) 
#LIBS += -L$(UCI)
#LIBS += -L/usr/local/lib/
CFLAGS = -std=gnu++11 -std=c++11 -O3 -Wall -Wno-sign-compare -fmessage-length=0 -fPIC $(INCLUDES)
LDFLAGS = -shared
LFLAGS = $(LIBS)
LFLAGS += -Wl,--whole-archive $(LIBTURBOTEXTANALYSIS)/libturbotextanalysis.a -Wl,--no-whole-archive
LFLAGS += -Wl,--whole-archive $(LIBTURBOPARSER)/libturboparser.a -Wl,--no-whole-archive
LFLAGS += -Wl,-whole-archive -lad3 -Wl,-no-whole-archive
LFLAGS += -lgflags
LFLAGS += -lglog
LFLAGS += -lconfig++
#LFLAGS += $(LIBCONFIG)/libconfig++.so.9.1.3
LFLAGS += -licudata -licui18n -licuio -licutu -licuuc
#LFLAGS += $(UCI)/libicudata.so $(UCI)/libicui18n.so  $(UCI)/libicuio.so  $(UCI)/libicutu.so  $(UCI)/libicuuc.so

#For verbosity
LFLAGS += -v
CFLAGS += $(DEBUG)

HDRS = $(wildcard *.h)
HDRS += $(shell find $(HEADERPATH) $(LIBTURBOTEXTANALYSIS) $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LOCALINCLUDES) $(LIBCONFIGINCLUDES) $(UCIINCLUDES) -name '*.h')
HDRS += $(shell find $(HEADERPATH) $(TURBOTEXTANALYSISINCLUDES)  $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LOCALINCLUDES) $(LIBCONFIGINCLUDES) $(UCIINCLUDES) -name '*.h++')
SRCS  = $(shell find $(SOURCEPATH) -name '*.cpp')
SRCS += $(shell find $(SOURCEPATH) -name '*.c')
OBJS = $(SRCS:.cpp=.o)
TXTS = $(wildcard *.txt)
SCRIPTS = $(wildcard *.sh)

all: $(EXEC)

$(EXEC): $(OBJS)
	@echo Compiling program $@
	$(CC) $^ $(CFLAGS) $(LFLAGS) -o $@  $(ENDLFLAGS)
	@echo Success
	
%.o: %.cpp $(HDRS)
	$(CC) -c $(CFLAGS) $(LFLAGS) -o $@ $<

clean:
	@echo Clean
	rm -f *~ *.o
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ $(SOURCEPATH)/*.o $(EXEC)
	@echo Success
