SRC = $(wildcard src/*.cpp)
CFLAGS = -std=c++17 -O3 -Ofast -Isrc/include

CPP = g++

all: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o LF.exe 

