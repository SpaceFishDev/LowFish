SRC = $(wildcard src/*.cpp)
CFLAGS = -O3 -Ofast -Wall -Wextra -Isrc/include

CPP = g++

all: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o LF.exe 

