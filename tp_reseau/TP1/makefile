all: main clean

main: main.o myAlgo.o
	gcc -o main main.o myAlgo.o

main.o: .FORCE
	gcc -c main.c -o main.o -Wall -g

myAlgo.o: .FORCE
	gcc -c myAlgo.c -o myAlgo.o -Wall -g

clean:
	rm *.o

.FORCE:
