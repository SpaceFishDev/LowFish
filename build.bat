gcc src/main.c src/parser/parser.c src/lexer/lexer.c src/bytecode/bytecode.c src/util.c -Isrc/ -std=c99 -o lowfish.exe
lowfish.exe