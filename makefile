CC      = g++
CCFLAGS = -c -Wall
LDFLAGS = -Wall
PLOT    = -std=c++14 -I/usr/include/python2.7 -lpython2.7

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
CIMG     = -O2 -L/usr/X11R6/lib -lm -lpthread -lX11 -lfftw3_threads


default: checkpoint1 checkpoint2 checkpoint3 fft

checkpoint1: cp1/cp1.cpp obj/global.o
	$(CC) $(LDFLAGS) $^ -o bin/$@ $(PLOT)

checkpoint2: $(CP2_OBJ) obj/global.o
	$(CC) $(LDFLAGS) -o bin/$@ $^ $(PLOT)

checkpoint3: $(CP3_OBJ) obj/global.o $(MINUIT_OBJ)
	$(CC) $(LDFLAGS) -o bin/$@ $^ $(PLOT) $(MINUIT)

fft: $(FFT_OBJ) obj/global.o
	$(CC) $(LDFLAGS) -o bin/$@ $^ $(PLOT) $(FFT) $(CIMG)

obj/%.o: cp2/%.cpp obj/global.o
	$(CC) $(CCFLAGS) -o $@ $< $(PLOT)

obj/%.o: cp3/%.cpp obj/global.o	$(MINUIT_OBJ)
	$(CC) $(CCFLAGS) -o $@ $< $(PLOT) $(MINUIT)

obj/%.o: fft/%.cpp obj/global.o 
	$(CC) $(CCFLAGS) -o $@ $< $(PLOT) $(FFT)

obj/global.o: global.cpp
	$(CC) $(CCFLAGS) -o $@ $<

clean:
	@rm -r obj/* 2>/dev/null || true
	@rm -r bin/* 2>/dev/null || true