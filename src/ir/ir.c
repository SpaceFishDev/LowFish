#include "ir.h"

stmt_t get_stmt(node* n, typechecker* type_checker)
{
	switch (n->type)
	{
		case FUNCTION:
		{
			return (stmt_t){
				n,
				get_function(type_checker, n->children[1]->node_token.text)
					->type,
				false, true};
		}
		break;
	}
	return (stmt_t){0};
}

stmt_gatherer* create_gatherer(node* root)
{
	stmt_gatherer* t = malloc(sizeof(stmt_gatherer));
	if (!t)
	{
		printf("Couldn't create a gatherer.\n");
		exit(1);
	}
	*t = (stmt_gatherer){0, malloc(sizeof(stmt_t)), root};
	return t;
}
void append_stmt(stmt_gatherer* gatherer, stmt_t stmt)
{
	gatherer->n_stmt++;
	gatherer->statements =
		realloc(gatherer->statements, sizeof(stmt_t) * gatherer->n_stmt);
	gatherer->statements[gatherer->n_stmt - 1] = stmt;
}
void gather_all(node* current, stmt_gatherer* gatherer, typechecker* tchecker)
{
	if (!current || !gatherer || !tchecker)
	{
		printf("DEBUG: Current node, gatherer or tchecker are null.");
		return;
	}
	stmt_t stmt = get_stmt(current, tchecker);
	append_stmt(gatherer, get_stmt(current, tchecker));
	for (size_t i = 0; i < current->n_child; ++i)
	{
		gather_all(current->children[i], gatherer, tchecker);
	}
}

void gather_statements(stmt_gatherer* gatherer, typechecker* tchecker)
{
	gather_all(gatherer->root, gatherer, tchecker);
}
