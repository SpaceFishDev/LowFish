#ifndef TYPE_CHECKER_H

#define TYPE_CHECKER_H
#include "../parser/parser.h"

typedef struct {
    char *title;
    char *type;
    size_t n_args;  // -1 means unknown, like for a C function.
} function;

typedef struct var {
    char *title;
    char *type;
    size_t scope;
    function *func;
    struct var *pointing_to;
} variable;

typedef struct {
    size_t n_func;
    function *functions;
    size_t n_var;
    variable *variables;
    size_t n_label;
    char **labels;
    size_t scope;
} typechecker;

bool type_check_tree(node *tree, typechecker *type_checker);
void restructure_parents(node *tree);

#endif
