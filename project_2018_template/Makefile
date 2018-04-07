CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=hello

fractal: main.o fractal.o
	$(CC) -o fractal fractal.o ain.o $(LDFLAGS)

fractal.o:
						@(cd $(libfractal) && $(MAKE) $@)

main.o: main.c fractal.o
	$(CC) -o main.o -c main.c $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
