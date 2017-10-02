CXX=g++
PLOTFLAG=-I/usr/include/python2.7 -lpython2.7
DIR0=./matrix
DIR1=./checkpoint1
STD=-std=c++17

default: matrix checkpoint1

matrix: $(DIR0)/matrix.cpp
	$(CXX) $^ -o $(DIR0)/$@ $(STD)

checkpoint1: $(DIR1)/checkpoint1.cpp
	$(CXX) $^ -o $(DIR1)/$@ $(STD) $(PLOTFLAG)