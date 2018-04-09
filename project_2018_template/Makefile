CC=gcc
CFLAGS=-W -Wall -ansi
LDFLAGS=

fractal: fractal.o main.o
	$(CC) -std=c99 -o fractal fractal.o main.o $(LDFLAGS)

fractal.o:
	cd libfractal && ${MAKE}

main.o: main.c fractal.o
	$(CC) -std=c99 -o -c main.c $(CFLAGS)

clean:
	rm -rf *.o
