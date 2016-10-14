#!/bin/bash
# Makefile for libturbotextanalysis
CC = g++
DEBUG = -g

LIBTURBOPARSER = /home/user/Projects/TurboParser/libturboparser
UTIL = /home/user/Projects/TurboParser/src/util
CLASSIFIER = /home/user/Projects/TurboParser/src/classifier
SEQUENCE = /home/user/Projects/TurboParser/src/sequence
TAGGER = /home/user/Projects/TurboParser/src/tagger
ENTITYRECOGNIZER = /home/user/Projects/TurboParser/src/entity_recognizer
PARSER = /home/user/Projects/TurboParser/src/parser
SEMANTICPARSER = /home/user/Projects/TurboParser/src/semantic_parser
COREFERENCERESOLVER = /home/user/Projects/TurboParser/src/coreference_resolver
MORPHOLOGICALTAGGER = /home/user/Projects/TurboParser/src/morphological_tagger


AUXLIBS = /home/user/Projects/TurboParser/deps/local/lib
AUXINCLUDES = /home/user/Projects/TurboParser/deps/local/include
LOCALDEPSINCLUDES = /home/user/Projects/TurboTextAnalysis/Deps
LIBCONFIGINCLUDES = /home/user/Projects/TurboTextAnalysis/Deps/libconfig-1.4.9/lib
LIBCONFIG = /home/user/Projects/TurboTextAnalysis/Deps/libconfig-1.4.9/lib/.libs

INCLUDES = -I$(UTIL) -I$(CLASSIFIER) -I$(SEQUENCE) -I$(TAGGER) -I$(ENTITYRECOGNIZER) -I$(PARSER) -I$(SEMANTICPARSER) -I$(COREFERENCERESOLVER)  -I$(MORPHOLOGICALTAGGER) -I$(LIBTURBOPARSER) -I$(AUXINCLUDES) -I$(LOCALDEPSINCLUDES) -I$(LIBCONFIGINCLUDES)
LIBS = -L$(AUXLIBS) -L$(LIBTURBOPARSER) -L$(LIBCONFIG)
#LIBS += -L/usr/local/lib/
CFLAGS = -std=gnu++11 -std=c++11 -O3 -Wall -Wno-sign-compare -fmessage-length=0 -fPIC $(INCLUDES)
LDFLAGS = -shared
LFLAGS = $(LIBS) -Wl,-whole-archive -lad3 -Wl,-no-whole-archive -lgflags -lglog $(LIBCONFIG)/libconfig++.so.9.1.3 $(LIBTURBOPARSER)/libturboparser.a

HDRS = $(wildcard *.h)
HDRS += $(shell find $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LIBCONFIGINCLUDES) -name '*.h')
HDRS += $(shell find $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LIBCONFIGINCLUDES) -name '*.h++')
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TXTS = $(wildcard *.txt)
SCRIPTS = $(wildcard *.sh)

#For verbosity
LFLAGS += -v
CFLAGS += $(DEBUG)

all : libturbotextanalysis.a libturbotextanalysis.so

libturbotextanalysis.a : $(OBJS)
	@echo Creating static lib $@
	ar rcs $@ $(OBJS)

libturbotextanalysis.so : $(OBJS)
	@echo Creating dynamic lib $@
	$(CC) -o $@ $(OBJS) $(LDFLAGS) $(LFLAGS) 

%.o: %.cpp $(HDRS)
	$(CC) -c $(CFLAGS) $(LFLAGS) -o $@ $<


clean:
	@echo Clean
	rm -f *~ *.o *~
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ *.o *~ libturbotextanalysis.a libturbotextanalysis.so
	@echo Success
