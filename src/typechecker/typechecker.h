#ifndef TYPE_CHECKER_H

#define TYPE_CHECKER_H
#include<parser/parser.h>

typedef struct
{
    char* title;
    char* type;
} function;


typedef struct
{ } typechecker;

bool type_check_tree( node* tree );

#endif