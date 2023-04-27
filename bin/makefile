SRC = $(wildcard ../src/*.cpp)
CFLAGS = -std=c++20 -fpermissive -O3 -Ofast -I../src/include res\ico.res
OUT = lf.exe
CPP = g++
CC = gcc
SRC_C = $(wildcard ../src/IR/*.cpp)
OUT_C = lf_ir.exe
CFLAGSC = -std=c++20 -O3 -Ofast -I../src/ir/include  -fpermissive

all: 
	make clean
	make build
	make run
	make ir 
build: $(SRC)
	windres res\ico.rc -O coff -o res\ico.res
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT)
	$(CPP) $(CFLAGSC) $(SRC_C) -o $(OUT_C) 
ir:
	$(OUT_C)
dir:
	$(CPP) $(CFLAGSC) $(SRC_C) -o $(OUT_C) -g

clean:
	del $(OUT)
	del res\ico.res
run:
	$(OUT) --tree
debug: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT) -g
test: 
	py ../tests/test.py
linux:
	$(CPP) -std=c++20 -fpermissive -O3 -Ofast -I../src/include $(SRC) -o lf.o
	$(CPP) $(CFLAGSC) $(SRC_C) -o lf_ir.o