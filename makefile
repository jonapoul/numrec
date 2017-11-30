CC      = g++ -std=c++11
CCFLAGS = -c -Wall
LDFLAGS = -Wall
PLOT    = -I/usr/include/python2.7 -lpython2.7
OBJ    := obj
BIN    := bin

# cp2
CP2_CPP := $(wildcard cp2/*.cpp)
CP2_OBJ := $(addprefix obj/,$(notdir $(CP2_CPP:.cpp=.o)))
# cp3
CP3_CPP    := $(wildcard cp3/*.cpp)
CP3_OBJ    := $(addprefix obj/,$(notdir $(CP3_CPP:.cpp=.o)))
MINUIT_OBJ := $(wildcard /usr/include/minuit/obj/*.o)
MINUIT      = -I/usr/include/minuit -fopenmp
# fft
FFT_CPP := $(wildcard fft/*.cpp)
FFT_OBJ := $(addprefix obj/,$(notdir $(FFT_CPP:.cpp=.o)))
FFT      = -lfftw3
# machine learning
MAC_CPP := $(wildcard machine/*.cpp)
MAC_OBJ := $(addprefix obj/,$(notdir $(MAC_CPP:.cpp=.o)))
#MLPACK   = -I/usr/include/libxml2/ -lxml2 -lmlpack -larmadillo

default: checkpoint1 checkpoint2 checkpoint3 fft machine

checkpoint1: cp1/cp1.cpp $(OBJ)/global.o | $(BIN)
	$(CC) $(LDFLAGS) $^ -o $(BIN)/$@ $(PLOT)
checkpoint2: $(CP2_OBJ) $(OBJ)/global.o | $(BIN)
	$(CC) $(LDFLAGS) -o $(BIN)/$@ $^ $(PLOT)
checkpoint3: $(CP3_OBJ) $(OBJ)/global.o $(MINUIT_OBJ) | $(BIN)
	$(CC) $(LDFLAGS) -o $(BIN)/$@ $^ $(PLOT) $(MINUIT)
fft: $(FFT_OBJ) $(OBJ)/global.o | $(BIN)
	$(CC) $(LDFLAGS) -o $(BIN)/$@ $^ $(PLOT) $(FFT)
machine: $(MAC_OBJ) $(OBJ)/global.o | $(BIN)
	$(CC) $(LDFLAGS) -o $(BIN)/$@ $^ #$(MLPACK)

$(OBJ)/%.o: cp2/%.cpp $(OBJ)/global.o | $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $< $(PLOT)
$(OBJ)/%.o: cp3/%.cpp $(OBJ)/global.o	$(MINUIT_OBJ) | $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $< $(PLOT) $(MINUIT)
$(OBJ)/%.o: fft/%.cpp $(OBJ)/global.o | $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $< $(PLOT) $(FFT)
$(OBJ)/%.o: machine/%.cpp $(OBJ)/global.o | $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $< #$(MLPACK)

$(OBJ)/global.o: global.cpp
	mkdir -p $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $<

$(OBJ):
	mkdir -p $(OBJ)
$(BIN):
	mkdir -p $(BIN)

clean:
	@rm -r $(OBJ) 2>/dev/null || true
	@rm -r $(BIN) 2>/dev/null || true
