
OBJECTS = src/main.o
EXEC = main.out
CXXFLAGS = -Wall -Wextra -std=c++17
CXX = g++

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)
clean:
	rm $(EXEC) $(OBJECTS)
