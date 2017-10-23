CC=g++
CCFLAGS=-c -Wall
LDFLAGS=-Wall
PLOTFLAGS=-std=c++11 -I/usr/include/python2.7 -lpython2.7
CP2_CPP := $(wildcard cp2/*.cpp)
CP3_CPP := $(wildcard cp3/*.cpp)
CP2_OBJ := $(addprefix obj/,$(notdir $(CP2_CPP:.cpp=.o)))
CP3_OBJ := $(addprefix obj/,$(notdir $(CP3_CPP:.cpp=.o)))

default: checkpoint2 checkpoint3

checkpoint1: cp1/cp1.cpp obj/global.o
	$(CC) $(LDFLAGS) $^ -o bin/$@ $(PLOTFLAGS)

checkpoint2: $(CP2_OBJ) obj/global.o
	$(CC) $(LDFLAGS) -o bin/$@ $^ $(PLOTFLAGS)

checkpoint3: $(CP3_OBJ) obj/global.o
	$(CC) $(LDFLAGS) -o bin/$@ $^ $(PLOTFLAGS)

obj/%.o: cp2/%.cpp global.cpp
	$(CC) $(CCFLAGS) -o $@ $< $(PLOTFLAGS)

obj/%.o: cp3/%.cpp global.cpp	
	$(CC) $(CCFLAGS) -o $@ $< $(PLOTFLAGS)

obj/global.o: global.cpp
	$(CC) $(CCFLAGS) -o $@ $< $(PLOTFLAGS)

clean:
	@rm -r obj/*
	@rm -r bin/*