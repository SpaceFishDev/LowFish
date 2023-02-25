SRC = $(wildcard src/*.cpp)
CFLAGS = -std=c++17 -fpermissive -O3 -Ofast -Isrc/include

CPP = g++

all: 
	make clean
	make build
	make run
build: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o LF.exe
clean:
	del lf.exe
run:
	timer lf.exe --tree
debug: $(SRC)
	$(CPP) $(CFLAGS) $(SRC) -o lf.exe -g
