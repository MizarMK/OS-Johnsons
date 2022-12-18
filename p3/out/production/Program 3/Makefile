CC        = g++
CFLAGS    = -Wall -Wextra -Wpedantic -Wconversion -g -std=c++11

JC        = javac
JFLAGS    = -cp
sources   = $(wildcard *.java)
classes   = $(sources:.java-.class)

all: cbuffet javabuffet

javabuffet: $(classes)
	$(JC) $(JFLAGS) :. TestDriver.java

jar: $(classes)
	jar cvf javabuffet.jar $(classes)


cbuffet: testdriver.o buffet.o
	$(CC) $(CFLAGS) -o cbuffet testdriver.o buffet.o -lpthread

buffet.o: buffet.h

ThreadProcs.o: buffet.h ThreadProcs.cpp
	$(CC) $(CFLAGS) -c ThreadProcs.cpp
testdriver.o: buffet.h ThreadProcs.cpp testdriver.cpp
	$(CC) $(CFLAGS) -c testdriver.cpp


.PHONY: clean

clean:
	rm *.o *.class cbuffet