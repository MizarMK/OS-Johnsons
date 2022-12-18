CFLAGS = -Wall -Wextra -Wconversion -Wno-unused-parameter -g `pkg-config fuse --cflags`
LDFLAGS = `pkg-config fuse --libs`

all:  hoofs

hoofs : hoofs.o

clean:
	rm *.o hoofs
