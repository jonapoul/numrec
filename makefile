CC=g++
CCFLAGS=-c -Wall
LDFLAGS=-Wall
PLOTFLAGS=-std=c++11 -I/usr/include/python2.7 -lpython2.7
CP2_CPP := $(wildcard cp2/*.cpp)
OBJ := $(addprefix obj/,$(notdir $(CP2_CPP:.cpp=.o)))

default: checkpoint1 checkpoint2

checkpoint1: cp1/cp1.cpp
	$(CC) $(LDFLAGS) $^ -o bin/$@ $(PLOTFLAGS)

checkpoint2: $(OBJ)
	$(CC) $(LDFLAGS) -o bin/$@ $^ $(PLOTFLAGS)

obj/%.o: cp2/%.cpp
	$(CC) $(CCFLAGS) -o $@ $< $(PLOTFLAGS)

clean:
	@rm -r obj/*
	@rm -r bin/*