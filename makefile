src := $(wildcard src/*.c) $(wildcard src/lexer/*.c) $(wildcard src/parser/*.c)
out := lowfish
cflags := -Isrc/

all: $(src)
	gcc $(src) $(cflags) -o $(out)
	./$(out)
debug: $(src)
	gcc $(src) $(cflags) -o $(out) -g
	gdb ./$(out)
