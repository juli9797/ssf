main: main.o
	g++ main.o -o ssf
main.o: main.cpp
	g++ -c main.cpp
clean:
	rm *.o
