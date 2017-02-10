#!/bin/bash
# Makefile for TurboTextAnalysis/TestCrossPlatfTurboTextAnalysis
CXX = clang++-3.8
CC = clang-3.8
#CXX = g++-6
#CC = gcc-6
DEBUG = -g
EXEC = targetapplication

HEADERPATH = .
SOURCEPATH = .

LOCALDEPSINCLUDES = ..
AUXINCLUDES = 
AUXLIBS = 

INCLUDES =  
INCLUDES += -I$(LOCALDEPSINCLUDES)
#INCLUDES += -I$(AUXINCLUDES)
#INCLUDES += -I/usr/local/include/
#INCLUDES += -I/usr/include/
LIBS = 
#LIBS += -L$(AUXLIBS)
#LIBS += -L/home/dan/lib_deps/
#LIBS += -L/usr/local/lib/
#LIBS += -L/usr/lib/
#LIBS += -L/usr/lib/x86_64-linux-gnu
CXXFLAGS = -std=c++14 -O3 -Wall -fmessage-length=0 -fPIC $(INCLUDES) 
CXXFLAGS += $(DEBUG)
LFLAGS  = $(LIBS)
#For dynamic linking loader (libdl)
LFLAGS  += -ldl
#To export symbols from this application to the libraries loaded with dlopen.
LFLAGS  += -rdynamic
#For verbosity
LFLAGS += -v 
LDFLAGS = -shared
ENDLFLAGS = 

HDRS  = $(shell find $(HEADERPATH) $(LOCALDEPSINCLUDES) $(AUXINCLUDES) -name '*.h')
HDRS += $(shell find $(HEADERPATH) $(LOCALDEPSINCLUDES) $(AUXINCLUDES) -name '*.h++')
SRCS  = $(shell find $(SOURCEPATH) -name '*.cpp')
SRCS += $(shell find $(SOURCEPATH) -name '*.c')
OBJS = $(SRCS:.cpp=.o)
TXTS = $(wildcard *.txt)
SCRIPTS = $(wildcard *.sh)
 
all: $(EXEC)

$(EXEC): $(OBJS)
	@echo Compiling program $@
	$(CXX) $^ $(CXXFLAGS) $(LFLAGS) -o $@
	@echo Success

%.o: %.cpp $(HDRS)
	$(CXX) -c $(CXXFLAGS) $(LFLAGS) -o $@ $<

clean:
	@echo Clean
	rm -f *~ *.o
	@echo Success

cleanall:
	@echo Clean All
	rm -f *~ $(SOURCEPATH)/*.o $(EXEC)
	@echo Success