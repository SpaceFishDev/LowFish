#ifndef IR_H

#define IR_H

#include "../typechecker/typechecker.h"

typedef struct
{
	node* parent_node;
	char* type;	 // string because some types are user defined.
	bool is_enumerable;
	bool is_executable;
} stmt_t;

stmt_t get_stmt(node* n, typechecker* type_checker);

typedef struct
{
	size_t n_stmt;
	stmt_t* statements;
	node* root;
} stmt_gatherer;  // best name I've come up with.

stmt_gatherer* create_gatherer(node* root);

void append_stmt(stmt_gatherer* gatherer, stmt_t stmt);

void gather_statements(stmt_gatherer* gatherer, typechecker* tchecker);

#endif

