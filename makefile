
OBJECTS = src/main.o src/log.o
EXEC = ssf
CXXFLAGS = -Wall -Wextra -std=c++2a -ggdb 
CXX = g++

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)
clean:
	rm $(EXEC)
	rm src/*.o
fresh: clean all

install: 
	cp ssf /bin

