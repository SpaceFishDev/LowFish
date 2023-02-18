SRC = $(wildcard src/*.cpp)
CFLAGS = -std=c++11 -O3 -Ofast -Isrc/include

CPP = g++

all: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o LF.exe 

