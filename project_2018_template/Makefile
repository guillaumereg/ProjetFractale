#	CC=gcc
#	CFLAGS=-W -Wall -ansi
#	LDFLAGS=

#	all: main.o fractal.o
#		gcc -o main.o libfractal/fractal.o libfractal/tools.o -pthread 'sdl-config --cflags --libs'

	# fractal: fractal.o main.o
	# 	$(CC) -std=c99 -o fractal fractal.o main.o $(LDFLAGS)

#	fractal.o:
#		cd libfractal && ${MAKE}

#	main.o: main.c fractal.o
#		$(CC) -std=c99 -o -c main.c $(CFLAGS) -lSDL

#	clean:
#		rm -rf *.o


ARGS=-c -Wall -W

edit : main.o libfractal/fractal.o libfractal/tools.o
	gcc -o main main.o libfractal/fractal.o libfractal/tools.o -lSDL -pthread

main.o : main.c
	gcc $(ARGS) main.c

libfractal/fractal.o : ./libfractal/fractal.c
	gcc $(ARGS) libfractal/fractal.c -o libfractal/fractal.o

libfractal/tools.o : ./libfractal/tools.c ./libfractal/fractal.h
	gcc -o ./libfractal/tools.o $(ARGS) ./libfractal/tools.c -lSDL

debug : main.o libfractal/fractal.o libfractal/tools.o
	gcc -g -o main main.o libffractal/fractal.o libfractal/tools.o -lSDL -pthread

lib :
	cd libfractal && $(MAKE)

clean :
	rm *.o
	rm ./main

mrProper :
	rm *.o
	rm ./main
	rm libfractal/*.o
	rm libfractal/libfractal.a
