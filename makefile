SRC = $(wildcard src/*.cpp)
CFLAGS = -O3 -Ofast -Isrc/include

CPP = g++

all: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o LF.exe 

