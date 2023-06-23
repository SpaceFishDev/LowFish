#ifndef PARSER_H

#define PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lexer/lexer.h"
#include "../util.h"
#define node_type_to_string(x) \
    (((char *[]){              \
	"PROGRAM",             \
	"FUNCTION",            \
	"TYPE",                \
	"TOKEN_NODE",          \
	"BLOCK",               \
	"NORMALBLOCK",         \
	"ARROWBLOCK",          \
	"EXTERN",              \
	"ASM",                 \
	"BASICEXPRESSION",     \
	"EXPRESSION",          \
	"BINEXPR",             \
	"FUNCTION_CALL",       \
	"VARDECL",             \
	"ASSIGNMENT",          \
	"CONDITIONAL",         \
	"IF",                  \
	"WHILE",               \
	"ELSE",                \
	"REFERENCE",           \
	"LABEL",               \
	"GOTO",                \
    })[x])
#undef next
typedef enum {
    PROGRAM,
    FUNCTION,
    TYPE,
    TOKENNODE,
    BLOCK,
    NORMALBLOCK,
    ARROW_BLOCK,
    EXTERN,
    ASM,
    BASICEXPRESSION,
    EXPRESSION,
    BINEXPR,
    FUNCTION_CALL,
    VARDECL,
    ASSIGNMENT,
    CONDITIONAL,
    IF,
    WHILE,
    ELSE,
    REFERENCE,
    LABEL_NODE,
    GOTO,
} node_type;

typedef struct node {
    node_type type;
    token node_token;
    struct node **children;
    size_t n_child;
    struct node *parent;
} node;

typedef struct {
    size_t n_token;
    token *tokens;
    size_t pos;
    node *root;
    node *current_parent;
} parser;

parser *create_parser(token *tokens, size_t n_token);

node *parse(parser *Parser);
node *create_node(node_type type, token node_token, node *parent);
node *create_arbitrary_node(node_type type, node *parent);
void print_tree(node *n, int indent);
void put_error(char *msg, node *Node, token t);
#define append_child(x, y) x = append_child_to_x(x, y)
node *append_child_to_x(node *x, node *y);

void free_tree(node *n);

#endif
