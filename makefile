all: main.o
	gcc -o main main.o

main.o: main.c
	gcc -x c -c main.c

run:
	./main

clean:
	rm *.o
