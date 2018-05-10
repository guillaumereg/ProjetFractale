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
	rm libfractal/*.o

tests : tests/test1
	./tests/test1

tests/test1 :
	gcc -L/usr/local/lib tests/test1.c -o tests/test1 -lcunit
