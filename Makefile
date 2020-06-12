
CXX=g++
CXXFLAGS=-g3 -std=c++17 -Wall -Wextra -pedantic -pthread -Wno-switch
TEMPDIR=temp/
BIN=Emu
LIBS=-lpthread -lsfml-graphics -lsfml-window -lsfml-system 
CPPFLAGS= -DDEBUG -I/usr/include/ -I./includes -I/usr/include/SFML/ 
SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:%.cpp=%.o) 


all: $(OBJ)
	$(CXX) $^ -o $(BIN)   $(LIBS)


%.o: %.c
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $@ -c $< 

clean:
	rm -f *.o
	rm -f emu.log
	rm -f src/*.o
	rm $(BIN) 


