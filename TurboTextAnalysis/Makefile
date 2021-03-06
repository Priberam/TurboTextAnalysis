#!/bin/bash
# Makefile for libturbotextanalysis
CC = g++
DEBUG = -g
PROJNAME = libturbotextanalysis

HEADERPATH = .
SOURCEPATH = .
LOCALINCLUDES =  ..

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

#LIBCONFIGINCLUDES = /pba/lib_deps/libconfig/lib
#LIBCONFIG = /pba/lib_deps/libconfig/lib/.libs
#LIBCONFIGINCLUDES = ../../TurboTextAnalysis/deps/libconfig-1.4.9/lib
#LIBCONFIG = ../../TurboTextAnalysis/deps/libconfig-1.4.9/lib/.libs

INCLUDES = -I$(UTIL) -I$(CLASSIFIER) -I$(SEQUENCE) -I$(TAGGER) -I$(ENTITYRECOGNIZER) -I$(PARSER) -I$(SEMANTICPARSER) -I$(COREFERENCERESOLVER)  -I$(MORPHOLOGICALTAGGER) -I$(LIBTURBOPARSER) -I$(AUXINCLUDES) -I$(LOCALDEPSINCLUDES) -I$(LOCALINCLUDES)
LIBS = -L$(AUXLIBS) -L$(LIBTURBOPARSER)
#LIBS += -L/usr/local/lib/

BYPASSINIT_GLOG_D =

CFLAGS = -std=gnu++11 -std=c++11 -O3 -Wall -Wno-sign-compare -fmessage-length=0 -fPIC $(BYPASSINIT_GLOG_D)   $(CMACROS) $(INCLUDES)
LDFLAGS = -shared
LFLAGS = $(LIBS)
LFLAGS += -Wl,--whole-archive -lturboparser -Wl,--no-whole-archive
LFLAGS += -Wl,-whole-archive -lad3 -Wl,-no-whole-archive
LFLAGS += -lgflags
LFLAGS += -lglog
LFLAGS += -lconfig++
LFLAGS += -licudata -licui18n -licutu -licuuc -licuio
#LFLAGS += $(LIBCONFIG)/libconfig++.so
#LFLAGS += $(LIBCONFIG)/libconfig++.so.9.1.3

#For verbosity
LFLAGS += -v
CFLAGS += $(DEBUG)

HDRS = $(wildcard *.h)
HDRS += $(shell find $(HEADERPATH) $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LOCALINCLUDES) -name '*.h')
HDRS += $(shell find $(HEADERPATH) $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LOCALINCLUDES) -name '*.h++')
SRCS  = $(shell find $(SOURCEPATH) -name '*.cpp')
SRCS += $(shell find $(SOURCEPATH) -name '*.c')

SRCS := $(filter-out ./TurboTokenizer_test.cpp,$(SRCS))

OBJS = $(SRCS:.cpp=.o)
TXTS = $(wildcard *.txt)
SCRIPTS = $(wildcard *.sh)

all : $(PROJNAME).a $(PROJNAME).so

$(PROJNAME).a : $(OBJS)
	@echo Creating static lib $@
	ar rcs $@ $(OBJS)

$(PROJNAME).so : $(OBJS)
	@echo Creating dynamic lib $@
	echo $(filter-out ./TurboTokenizer_test.cpp,$(SRCS))
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LFLAGS) 

%.o: %.cpp $(HDRS)
	$(CC) -c $(CFLAGS) $(LFLAGS) -o $@ $<


clean:
	@echo Clean
	rm -f *~ *.o *~
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ *.o *~ $(PROJNAME).a $(PROJNAME).so
	@echo Success
