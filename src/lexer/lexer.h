#ifndef LEXER_H

#define LEXER_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"

typedef enum
{
	STRING,
	NUMBER,
	ID,
	BEGINOFBLOCK,
	ENDOFBLOCK,
	OPENBR,
	CLOSEBR,
	EQUAL,
	BOOLNOTEQUAL,
	BOOLEQUAL,
	EXCLAMATION,
	INDEXBROPEN,
	INDEXBRCLOSE,
	MORE,
	ARROW,
	LESS,
	MOREEQUAL,
	LESSEQUAL,
	BOOLOR,
	BOOLAND,
	LOGICALAND,
	LOGICALOR,
	XOR,
	PLUS,
	MINUS,
	DIV,
	MUL,
	SEMI,
	NL,
	COMMA,
	END_OF_FILE,
	BAD,
	NOTOKEN,
	REF,
	DEREF,
	LABEL,
} token_type;

typedef struct
{
	uint32_t col, row;
	token_type type;
	size_t len;
	char *text;
	int on_stack;
} token;

#define string_version(x) (#x)

#define create_token(col, row, text, type) \
	((token){col, row, type, strlen(text), text})

#define token_type_to_string(__type)                                \
	(((char *[]){                                                   \
		"STRING",		"NUMBER",	 "ID",			"BEGINOFBLOCK", \
		"ENDOFBLOCK",	"OPENBR",	 "CLOSEBR",		"EQUAL",        \
		"BOOLNOTEQUAL", "BOOLEQUAL", "EXCLAMATION", "INDEXBROPEN",  \
		"INDEXBRCLOSE", "MORE",		 "ARROW",		"LESS",         \
		"MOREEQUAL",	"LESSEQUAL", "BOOLOR",		"BOOLAND",      \
		"LOGICALAND",	"LOGICALOR", "XOR",			"PLUS",         \
		"MINUS",		"DIV",		 "MUL",			"SEMI",         \
		"NL",			"COMMA",	 "END_OF_FILE", "BAD",          \
		"NOTOKEN",		"REF",		 "DEREF",		"LABEL"})[__type])

typedef struct
{
	char *src;
	int line;
	int column;
	int pos;
} lexer;

#define next      \
	++Lexer->pos; \
	++Lexer->column

token lex(lexer *);

char *read_file(char *path);

char *mkstr(char *str);
#endif
