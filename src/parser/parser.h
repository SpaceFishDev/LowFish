#ifndef PARSER_H

#define PARSER_H

#include <lexer/lexer.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <util.h>
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
    })[x])
#undef next
typedef enum
{
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
} node_type;

typedef struct node
{
    node_type type;
    token node_token;
    struct node **children;
    size_t n_child;
    struct node *parent;
} node;

typedef struct
{
    size_t n_token;
    token *tokens;
    size_t pos;
    node *root;
    node *current_parent;
} parser;

parser *create_parser(token *tokens, size_t n_token);

node *parse(parser *Parser);
node *create_node(node_type type, token node_token, node *parent);
void print_tree(node *n, int indent);
void put_error(char *msg, node *Node, token t);

void free_tree(node *n);

#endif
