#!/bin/bash
# Makefile for DemoTextAnalysisTest
CC = g++
DEBUG = -g
EXEC = DemoTextAnalysisTest

HEADERPATH = .
SOURCEPATH = .
LOCALINCLUDES =  ..

LIBTURBOTEXTANALYSIS = ../TurboTextAnalysis
LIBTURBOPARSER = ../../TurboParser/libturboparser
UTIL = ../../TurboParser/src/util
CLASSIFIER = ../../TurboParser/src/classifier
SEQUENCE = ../../TurboParser/src/sequence
TAGGER = ../../TurboParser/src/tagger
ENTITYRECOGNIZER = ../../TurboParser/src/entity_recognizer
PARSER = ../../TurboParser/src/parser
SEMANTICPARSER = ../../TurboParser/src/semantic_parser
COREFERENCERESOLVER = ../../TurboParser/src/coreference_resolver
MORPHOLOGICALTAGGER = ../../TurboParser/src/morphological_tagger

AUXLIBS = ../../TurboParser/deps/local/lib
AUXINCLUDES = ../../TurboParser/deps/local/include
LOCALDEPSINCLUDES = ../../TurboTextAnalysis/deps
#LIBCONFIGINCLUDES = ../../TurboTextAnalysis/deps/libconfig-1.4.9/lib
#LIBCONFIG = ../../TurboTextAnalysis/deps/libconfig-1.4.9/lib/.libs
#UCIINCLUDES = /pba/lib_deps/icu/include
#UCI = /pba/lib_deps/icu/lib

INCLUDES = -I$(LIBTURBOTEXTANALYSIS) -I$(UTIL) -I$(CLASSIFIER) -I$(SEQUENCE) -I$(TAGGER) -I$(ENTITYRECOGNIZER) -I$(PARSER) -I$(SEMANTICPARSER) -I$(COREFERENCERESOLVER)  -I$(MORPHOLOGICALTAGGER) -I$(LIBTURBOPARSER) -I$(AUXINCLUDES) -I$(LOCALDEPSINCLUDES) -I$(LOCALINCLUDES)  -I$(LIBCONFIGINCLUDES) -I$(UCIINCLUDES)
LIBS = -L$(AUXLIBS) -L$(LIBTURBOTEXTANALYSIS) -L$(LIBTURBOPARSER)
#LIBS += -L$(UCI)
#LIBS += -L/usr/local/lib/
CFLAGS = -std=gnu++11 -std=c++11 -O3 -Wall -Wno-sign-compare -fmessage-length=0 -fPIC $(INCLUDES)
LDFLAGS = -shared
LFLAGS = $(LIBS)
LFLAGS += -Wl,--whole-archive -lturbotextanalysis -Wl,--no-whole-archive
LFLAGS += -Wl,--whole-archive -lturboparser -Wl,--no-whole-archive
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
HDRS += $(shell find $(HEADERPATH) $(LIBTURBOTEXTANALYSIS) $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LOCALINCLUDES) $(UCIINCLUDES) -name '*.h')
HDRS += $(shell find $(HEADERPATH) $(TURBOTEXTANALYSISINCLUDES)  $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LOCALINCLUDES) $(UCIINCLUDES) -name '*.h++')
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
