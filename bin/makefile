SRC = $(wildcard ../src/*.cpp)
CFLAGS = -std=c++17 -fpermissive -O3 -Ofast -I../src/include res\ico.res
OUT = lf.exe
CPP = g++

all: 
	make clean
	make build
	make run
build: $(SRC)
	windres res\ico.rc -O coff -o res\ico.res
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT)
clean:
	del $(OUT)
	del res\ico.res
run:
	$(OUT) --tree
debug: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT) -g
test: 
	py ../tests/test.py