
OBJECTS = src/main.o
EXEC = main.out
CXXFLAGS = -Wall -Wextra -std=c++2a
CXX = g++

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)
filetree: src/filetree.o
	g++ $(CXXFLAGS) -o filetree.out src/filetree.o
clean:
	rm $(EXEC)
	rm src/*.o
