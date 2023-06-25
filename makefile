src := $(wildcard src/*.c) $(wildcard src/lexer/*.c) $(wildcard src/parser/*.c) $(wildcard src/typechecker/*.c) $(wildcard src/ir/*.c)
out := lowfish
cflags := -Isrc/ 

all: $(src)
	gcc $(src) $(cflags) -o $(out) -std=c99
	./$(out)
debug: $(src)
	gcc $(src) $(cflags) -o $(out) -g
	gdb ./$(out)
