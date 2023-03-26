SRC = $(wildcard ../src/*.cpp)
CFLAGS = -std=c++20 -fpermissive -O3 -Ofast -I../src/include res\ico.res
OUT = lf.exe
CPP = g++
CC = gcc
SRC_C = $(wildcard ../src/IR/*.c)
OUT_C = lfir.exe
CFLAGSC = -O3 -Ofast -I../src/ir/include 

all: 
	make clean
	make build
	make run
build: $(SRC)
	windres res\ico.rc -O coff -o res\ico.res
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT)
	$(CC) $(CFLAGSC) $(SRC_C) -o $(OUT_C)
clean:
	del $(OUT)
	del res\ico.res
run:
	$(OUT) --tree
debug: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT) -g
test: 
	py ../tests/test.py
