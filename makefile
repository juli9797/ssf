
OBJECTS = src/main.o src/log.o
OBJECTS_TEST = src/main_test.o src/log.o
EXEC = ssf
CXXFLAGS = -Wall -Wextra -std=c++2a -ggdb 
CXX = g++

all: clean $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)
test: clean $(OBJECTS_TEST)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS_TEST)
clean:
	-@rm -rf src/*.o 2>/dev/null || true
install: all
	cp ssf /bin

