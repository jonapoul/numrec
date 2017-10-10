CC=g++
CFLAGS=-c -Wall
STD=-std=c++11
PLOTFLAGS=-I/usr/include/python2.7 -lpython2.7

default: matrix cp1 cp2

matrix: matrix/matrix.cpp
	$(CC) $^ -o matrix/$@

cp1: cp1/cp1.cpp
	$(CC) $^ -o cp1/$@ $(STD) $(PLOTFLAG)

cp2: cp2/cp2.cpp obj/ChargeDistribution.o obj/RootFinder.o
	$(CC) $^ -o cp2/$@ $(PLOTFLAGS)

obj/ChargeDistribution.o: cp2/ChargeDistribution.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(PLOTFLAGS)

obj/RootFinder.o: cp2/RootFinder.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	@rm -r -f obj/