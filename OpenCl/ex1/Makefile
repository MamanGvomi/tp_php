CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS= -O3 -lOpenCL -o programme_hote
EXEC=main

all: $(EXEC)

main: programme_hote.cpp
	$(CC) programme_hote.cpp $(LDFLAGS)

clean:
	rm -rf *.o

run:
	./programme_hote

mrproper: clean
	rm -rf $(EXEC)