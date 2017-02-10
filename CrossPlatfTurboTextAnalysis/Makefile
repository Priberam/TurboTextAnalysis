#!/bin/bash
# Makefile for TurboTextAnalysis/CrossPlatfTurboTextAnalysis
CC = g++
DEBUG = -g
PROJNAME = turbotextanalysis

LIBTURBOTEXTANALYSIS = ../TurboTextAnalysis
LIBTURBOPARSER = /home/dan/temp/TurboParser_DN20160603/libturboparser
UTIL = /home/dan/temp/TurboParser_DN20160603/src/util
CLASSIFIER = /home/dan/temp/TurboParser_DN20160603/src/classifier
SEQUENCE = /home/dan/temp/TurboParser_DN20160603/src/sequence
TAGGER = /home/dan/temp/TurboParser_DN20160603/src/tagger
ENTITYRECOGNIZER = /home/dan/temp/TurboParser_DN20160603/src/entity_recognizer
PARSER = /home/dan/temp/TurboParser_DN20160603/src/parser
SEMANTICPARSER = /home/dan/temp/TurboParser_DN20160603/src/semantic_parser
COREFERENCERESOLVER = /home/dan/temp/TurboParser_DN20160603/src/coreference_resolver
MORPHOLOGICALTAGGER = /home/dan/temp/TurboParser_DN20160603/src/morphological_tagger


AUXLIBS = /home/dan/temp/TurboParser_DN20160603/deps/local/lib
AUXINCLUDES = /home/dan/temp/TurboParser_DN20160603/deps/local/include
LOCALDEPSINCLUDES = /home/dan/temp/TurboTextAnalysis/deps
LIBCONFIGINCLUDES = /home/dan/temp/TurboTextAnalysis/deps/libconfig-1.4.9/lib
LIBCONFIG = /home/dan/temp/TurboTextAnalysis/deps/libconfig-1.4.9/lib/.libs

INCLUDES = -I$(UTIL) -I$(CLASSIFIER) -I$(SEQUENCE) -I$(TAGGER) -I$(ENTITYRECOGNIZER) -I$(PARSER) -I$(SEMANTICPARSER) -I$(COREFERENCERESOLVER)  -I$(MORPHOLOGICALTAGGER) -I$(LIBTURBOPARSER) -I$(LIBTURBOTEXTANALYSIS) -I$(AUXINCLUDES) -I$(LOCALDEPSINCLUDES) -I$(LIBCONFIGINCLUDES)
LIBS = -L$(AUXLIBS) -L$(LIBTURBOPARSER) -L$(LIBTURBOTEXTANALYSIS) -L$(LIBCONFIG)
#LIBS += -L/usr/local/lib/
CFLAGS = -std=gnu++11 -std=c++11 -O3 -Wall -Wno-sign-compare -fmessage-length=0 -fPIC $(INCLUDES)
LDFLAGS = -shared
LFLAGS = $(LIBS)
LFLAGS += -Wl,-whole-archive -lad3 -Wl,-no-whole-archive
LFLAGS += -lgflags
LFLAGS += -lglog
LFLAGS += $(LIBCONFIG)/libconfig++.so.9.1.3
LFLAGS += -Wl,--whole-archive $(LIBTURBOPARSER)/libturboparser.a -Wl,--no-whole-archive
LFLAGS += -Wl,--whole-archive $(LIBTURBOTEXTANALYSIS)/libturbotextanalysis.a -Wl,--no-whole-archive

HDRS = $(wildcard *.h)
HDRS += $(shell find $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(LIBTURBOTEXTANALYSIS) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LIBCONFIGINCLUDES) -name '*.h')
HDRS += $(shell find $(UTIL) $(CLASSIFIER) $(SEQUENCE) $(TAGGER) $(ENTITYRECOGNIZER) $(PARSER) $(SEMANTICPARSER) $(COREFERENCERESOLVER)  $(MORPHOLOGICALTAGGER) $(LIBTURBOPARSER) $(LIBTURBOTEXTANALYSIS) $(AUXINCLUDES) $(LOCALDEPSINCLUDES) $(LIBCONFIGINCLUDES) -name '*.h++')
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TXTS = $(wildcard *.txt)
SCRIPTS = $(wildcard *.sh)

#For verbosity
LFLAGS += -v
CFLAGS += $(DEBUG)

all : $(PROJNAME).a $(PROJNAME).so

$(PROJNAME).a : $(OBJS) 
	@echo Creating static lib $@
	ar rcs $@ $(OBJS)

$(PROJNAME).so : $(OBJS)
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
	rm -f *~ *.o *~ $(PROJNAME).a $(PROJNAME).so
	@echo Success
