#ifndef PARSER_H

#define PARSER_H

#include<lexer/lexer.h>
#include<stdlib.h>
#include<stdint.h>
#include<stddef.h>
#include<stdio.h>
#include<util.h>

typedef enum
{
    PROGRAM ,
    FUNCTION ,
    TYPE ,
    TOKENNODE ,
    BLOCK ,
    NORMALBLOCK ,
    ARROW_BLOCK ,
} node_type;


typedef struct node
{
    node_type type;
    token node_token;
    struct node** children;
    size_t n_child;
    struct node* parent;
} node;


typedef struct
{
    size_t n_token;
    token* tokens;
    size_t pos;
    node* root;
    node* current_parent;
} parser;

parser* create_parser( token* tokens , size_t n_token );


node* parse( parser* Parser );
node* create_node( node_type type , token node_token , node* parent );
void print_tree( node* n , int indent );

#endif