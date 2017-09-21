all: main clean

main: main.o myAlgo.o
	gcc -o main main.o myAlgo.o

main.o: .FORCE
	gcc -c main.c -o main.o -Wall

myAlgo.o: .FORCE
	gcc -c myAlgo.c -o myAlgo.o -Wall

clean:
	rm *.o

.FORCE:
