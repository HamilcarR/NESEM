
CXX=g++
CXXFLAGS=-g3 -std=c++17 -Wall -pedantic -pthread
TEMPDIR=temp/
BIN=Emu
LIBS=-lpthread 
CPPFLAGS=-I/usr/include/ -I./includes
SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:%.cpp=%.o) 


all: $(OBJ)
	$(CXX) $^ -o $(BIN)   $(LIBS)


%.o: %.c
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $@ -c $< 

clean:
	rm -f *.o
	rm -f src/*.o
	rm $(BIN) 


