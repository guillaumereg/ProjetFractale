CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=

fractal: fractal.o main.o
	$(CC) -o fractal fractal.o main.o $(LDFLAGS)

fractal.o:
	cd libfractal && ${MAKE}

main.o: main.c fractal.o
	$(CC) -o -c main.c $(CFLAGS)

clean:
	rm -rf *.o
