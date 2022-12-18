CC        = g++
CFLAGS    = -std=c++11 -Wall -Wextra -Wpedantic -Wconversion -g

JC        = javac
JFLAGS    = -cp
sources   = $(wildcard *.java)
classes   = $(sources:.java-.class)

all: testdriver javabuffet testdrivermutex

jar: $(classes)
	jar cvf javabuffet.jar $(classes)

javabuffet: $(classes)
	$(JC) $(JFLAGS) :. TestDriver.java
testdriver: testdriver.o buffet.o
	$(CC) $(CFLAGS) -o testdriver testdriver.o buffet.o -lpthread
testdrivermutex: testdrivermutex.o buffetmutex.o
	$(CC) $(CFLAGS) -o testdrivermutex testdrivermutex.o buffetmutex.o -lpthread

buffet.o: buffet.h buffet.cpp
	$(CC) $(CFLAGS) -c buffet.cpp
buffetmutex.o: buffet.h buffetmutex.cpp
	$(CC) $(CFLAGS) -c buffetmutex.cpp

ThreadProcs.o: buffet.h ThreadProcs.cpp
	$(CC) $(CFLAGS) -c ThreadProcs.cpp
ThreadProcMutex: buffet.h ThreadProcMutex.cpp
	$(CC) $(CFLAGS) -c ThreadProcMutex.cpp
testdriver.o: buffet.h ThreadProcs.cpp testdriver.cpp
	$(CC) $(CFLAGS) -c testdriver.cpp
testdrivermutex.o: buffet.h ThreadProcMutex.cpp testdrivermutex.cpp
	$(CC) $(CFLAGS) -c testdrivermutex.cpp


.PHONY: clean

clean:
	rm *.o *.class cbuffet
