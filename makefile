CC=g++
CFLAGS=-c -Wall
PLOTFLAGS=-std=c++11 -I/usr/include/python2.7 -lpython2.7

default: matrix cp1 cp2

matrix: matrix/matrix.cpp
	$(CC) $^ -o matrix/$@

cp1: cp1/cp1.cpp
	$(CC) $^ -o cp1/$@ $(PLOTFLAGS)

cp2: cp2/cp2.cpp cp2/ChargeDistribution.o cp2/RootFinder.o cp2/Plotter.o
	$(CC) $^ -o cp2/$@ $(PLOTFLAGS)

cp2/ChargeDistribution.o: cp2/ChargeDistribution.cpp
	$(CC) $(CFLAGS) $^ -o $@ 

cp2/RootFinder.o: cp2/RootFinder.cpp
	$(CC) $(CFLAGS) $^ -o $@

cp2/Plotter.o: cp2/Plotter.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(PLOTFLAGS)

clean:
	@rm -r -f obj/