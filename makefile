CXX=g++
PLOTFLAG=-I/usr/include/python2.7 -lpython2.7
DIR0=./matrix
DIR1=./cp1
DIR2=./cp2
STD=-std=c++17

default: matrix cp1 cp2

matrix: $(DIR0)/matrix.cpp
	$(CXX) $^ -o $(DIR0)/$@ $(STD)

cp1: $(DIR1)/cp1.cpp
	$(CXX) $^ -o $(DIR1)/$@ $(STD) $(PLOTFLAG)

cp2: $(DIR2)/cp2.cpp $(DIR2)/ChargeDistribution.o
	$(CXX) $^ -o $(DIR2)/$@ $(STD) $(PLOTFLAG)

ChargeDistribution.o: $(DIR2)/ChargeDistribution.cpp
	$(CXX) -c $^ -o $(DIR2)/$@ $(STD) $(PLOTFLAG)