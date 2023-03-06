SRC = $(wildcard ../src/*.cpp)
CFLAGS = -std=c++17 -fpermissive -O3 -Ofast -I../src/include
OUT = lf.exe
CPP = g++

all: 
	make clean
	make build
	make run
build: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT)
clean:
	del $(OUT)
run:
	$(OUT) --tree
debug: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o $(OUT) -g
