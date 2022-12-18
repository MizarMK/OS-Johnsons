CC        = gcc
CFLAGS    = -Wall -Wextra -Wpedantic -Wconversion -std=gnu99
LDFLAGS   =
OBJFILES  = prog1.o
OBJFILES2 = prog2.o
TARGET    = seq
TARGET2   = seqt

all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJFILES) 
	$(CC) $(CFLAGS) -o $(TARGET) -lpthread $(OBJFILES) $(LDFLAGS)

$(TARGET2): $(OBJFILES2)
	$(CC) $(CFLAGS) -o $(TARGET2) -lpthread $(OBJFILES) $(LDFLAGS)

clean: $(OBJFILES) $(OBJFILES2) $(TARGET) $(TARGET2)
	rm *.o seq seqt
