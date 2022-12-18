CC        = g++
CFLAGS    = -Wall -Wextra -Wpedantic -Wconversion -g -std=c++11
JC        = javac
JFLAGS    = -cp
sources   = $(wildcard *.java)
classes   = $(sources:.java-.class)

all: driver tlbdriver

tlbdriver: $(classes)
	$(JC) $(JFLAGS) :. PageSim.java

driver: omalloc.cpp driver.cpp
	$(CC) $(CFLAGS) omalloc.cpp driver.cpp -o driver

.PHONY: clean

clean:
	rm *.o *.class driver
