#include "typechecker.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
bool is_node(node *t, node_type type)
{
	assert((t && "T should exist in this situation."));
	return t->type == type;
}

void restructure_parents(node *tree)
{
	if (tree->n_child == 0)
	{
		return;
	}
	for (size_t i = 0; i < tree->n_child; ++i)
	{
		tree->children[i]->parent = tree;
		restructure_parents(tree->children[i]);
	}
}

bool has_child_recursive(node *check, node_type type)
{
	if (is_node(check, type))
	{
		return true;
	}
	for (size_t i = 0; i < check->n_child; ++i)
	{
		bool is = has_child_recursive(check->children[i], type);
		if (is)
		{
			return is;
		}
	}
	return false;
}
// gets first instance...
node *get_child_recursive(node *check, node_type type)
{
	if (is_node(check, type))
	{
		return check;
	}
	for (size_t i = 0; i < check->n_child; ++i)
	{
		node *is = get_child_recursive(check->children[i], type);
		if (is)
		{
			return is;
		}
	}
	return 0;
}

node *get_parent(node *check, node_type type)
{
	node *p = check;
	while (p)
	{
		if (p->type == type)
		{
			return p;
		}
		p = p->parent;
	}
	return 0;
}

function *get_function(typechecker *type_checker, char *title)
{
	for (size_t i = 0; i != type_checker->n_func; ++i)
	{
		if (!strcmp(title, type_checker->functions[i].title))
		{
			return type_checker->functions + i;
		}
	}
	return 0;
}

variable *get_var(typechecker *type_checker, char *title, size_t scope,
				  function *func)
{
	for (size_t i = 0; i != type_checker->n_var; ++i)
	{
		if (!strcmp(title, type_checker->variables[i].title) &&
			type_checker->variables[i].scope == scope &&
			type_checker->variables[i].func->title == func->title)
		{
			return &type_checker->variables[i];
		}
	}
	return 0;
}
// big mac of a function name right there.
void put_error_incompatible_types_assignment(char *type_a, char *type_b,
											 node *n)
{
	// dont need to free since put_error kills the program
	char *buffer = malloc(1024);
	sprintf(buffer,
			"Incompatible types used in an assigmnent Type '%s' is assigned to "
			"Type '%s'",
			type_a, type_b);
	put_error(buffer, 0, n->node_token);
}

bool starts_with(char *a, char *b)
{
	if (!a || !b)
	{
		return false;
	}
	if (!strcmp(a, b))
	{
		return true;
	}
	if (strlen(a) < strlen(b))
	{
		return false;
	}
	size_t lenb = strlen(b);
	char *start_of_a = a;
	while (*a)
	{
		if (*a != *b)
		{
			return (a - start_of_a == lenb);
		}
		++a;
		++b;
	}
	return false;
}

void put_error_incompatible_operation(char *operation, char *a, char *b,
									  node *n)
{
	char *buffer = malloc(1024);
	sprintf(buffer, "Cannot do operation '%s' on type '%s' with '%s'.",
			operation, a, b);
	put_error(buffer, 0, n->node_token);
}
// put error frees the string so it gotta be on the heap :(

node *eval(node *x, node *parent);

char *get_type_recursive(typechecker *type_checker, node *expr)
{
	if (expr->type == BINEXPR)
	{
		if (expr->children[0]->node_token.text[0] == '=')
		{
			return mkstr("bool");
		}
	}
	if (expr->type == BASICEXPRESSION)
	{
		if (expr->children[0]->node_token.type == ID)
		{
			node *f = get_parent(expr, FUNCTION);
			function *func =
				get_function(type_checker, f->children[1]->node_token.text);
			variable *v =
				get_var(type_checker, expr->children[0]->node_token.text,
						type_checker->scope, func);
			if (!v)
			{
				char *buffer = malloc(1024);
				sprintf(buffer, "Variable '%s' doesnt exist.",
						expr->children[0]->node_token.text);
				put_error(buffer, 0, expr->children[0]->node_token);
			}
			return v->type;
		}
		if (expr->children[0]->node_token.type == DEREF)
		{
			node *f = get_parent(expr, FUNCTION);
			function *func =
				get_function(type_checker, f->children[1]->node_token.text);
			variable *v =
				get_var(type_checker, expr->children[0]->node_token.text,
						type_checker->scope, func);
			if (!v)
			{
				char *buffer = malloc(1024);
				sprintf(buffer, "Variable '%s' doesnt exist.",
						expr->children[0]->node_token.text);
				put_error(buffer, 0, expr->children[0]->node_token);
			}
			if (!v->pointing_to)
			{
				char *buffer = malloc(1024);
				sprintf(buffer,
						"Variable '%s' is not a pointer and cannot be "
						"dereferenced.",
						expr->children[0]->node_token.text);
				put_error(buffer, 0, expr->children[0]->node_token);
			}
			return v->pointing_to->type;
		}
		if (expr->children[0]->node_token.type == NUMBER)
		{
			return mkstr("i32?"); // question mark denotes literal
		}
		if (expr->children[0]->node_token.type == STRING)
		{
			if (strlen(expr->children[0]->node_token.text) == 1)
			{
				return mkstr("u8?");
			}
			return mkstr("ptr8");
		}
	}
}

node *eval(node *x, node *parent)
{
	if (x->type == BASICEXPRESSION)
	{
		return x;
	}
	switch (x->type)
	{
	case BINEXPR:
	{
		node *op = x->children[0];
		node *left = op->children[0];
		node *right = op->children[1];
		if (!has_child_recursive(x, BASICEXPRESSION) &&
				left->type != BASICEXPRESSION ||
			right->type != BASICEXPRESSION)
		{
			return x;
		}
		if (left->type != BASICEXPRESSION)
		{
			left = eval(left, op);
		}
		if (right->type != BASICEXPRESSION)
		{
			right = eval(right, op);
		}
		left = left->children[0];
		right = right->children[0];
		if (left->node_token.type == ID || right->node_token.type == ID)
		{
			return x;
		}
		switch (op->node_token.text[0])
		{
		case '+':
		{
			if (left->node_token.type == NUMBER &&
				right->node_token.type == NUMBER)
			{
				int value = atoi(left->node_token.text) +
							atoi(right->node_token.text);
				char *buf = malloc(32);
				sprintf(buf, "%d", value);
				node *x =
					create_arbitrary_node(BASICEXPRESSION, parent);
				node *n = create_node(
					TOKENNODE,
					create_token(left->node_token.col,
								 left->node_token.row, buf, NUMBER),
					x);
				append_child(x, n);
				return x;
			}
			put_error_incompatible_operation(
				"+", token_type_to_string(left->node_token.type),
				token_type_to_string(right->node_token.type), right);
		}
		break;
		case '-':
		{
			if (left->node_token.type == NUMBER &&
				right->node_token.type == NUMBER)
			{
				int value = atoi(left->node_token.text) -
							atoi(right->node_token.text);
				char *buf = malloc(32);
				sprintf(buf, "%d", value);
				node *x =
					create_arbitrary_node(BASICEXPRESSION, parent);
				node *n = create_node(
					TOKENNODE,
					create_token(left->node_token.col,
								 left->node_token.row, buf, NUMBER),
					x);
				append_child(x, n);
				return x;
			}
			put_error_incompatible_operation(
				"-", token_type_to_string(left->node_token.type),
				token_type_to_string(right->node_token.type), right);
		}
		break;
		case '/':
		{
			if (left->node_token.type == NUMBER &&
				right->node_token.type == NUMBER)
			{
				if (atoi(right->node_token.text) == 0)
				{
					put_error(mkstr("Attempt to divide by zero."), 0,
							  right->node_token);
				}
				int value = atoi(left->node_token.text) /
							atoi(right->node_token.text);
				char *buf = malloc(32);
				sprintf(buf, "%d", value);
				node *x =
					create_arbitrary_node(BASICEXPRESSION, parent);
				node *n = create_node(
					TOKENNODE,
					create_token(left->node_token.col,
								 left->node_token.row, buf, NUMBER),
					x);
				append_child(x, n);
				return x;
			}
			put_error_incompatible_operation(
				"/", token_type_to_string(left->node_token.type),
				token_type_to_string(right->node_token.type), right);
		}
		case '*':
		{
			if (left->node_token.type == NUMBER &&
				right->node_token.type == NUMBER)
			{
				int value = atoi(left->node_token.text) *
							atoi(right->node_token.text);
				char *buf = malloc(32);
				sprintf(buf, "%d", value);
				node *x =
					create_arbitrary_node(BASICEXPRESSION, parent);
				node *n = create_node(
					TOKENNODE,
					create_token(left->node_token.col,
								 left->node_token.row, buf, NUMBER),
					x);
				append_child(x, n);
				return x;
			}
			put_error_incompatible_operation(
				"*", token_type_to_string(left->node_token.type),
				token_type_to_string(right->node_token.type), right);
		}
		case '=':
		{
			if (left->node_token.type == STRING &&
				right->node_token.type == STRING)
			{
				char *buf = malloc(32);
				sprintf(buf, "%d",
						!strcmp(left->node_token.text,
								right->node_token.text));
				node *x =
					create_arbitrary_node(BASICEXPRESSION, parent);
				node *n = create_node(
					TOKENNODE,
					create_token(left->node_token.col,
								 left->node_token.row, buf, NUMBER),
					x);
				append_child(x, n);
				return x;
			}
			if (left->node_token.type == NUMBER &&
				right->node_token.type == NUMBER)
			{
				int value = atoi(left->node_token.text) ==
							atoi(right->node_token.text);
				char *buf = malloc(32);
				sprintf(buf, "%d", value);
				node *x =
					create_arbitrary_node(BASICEXPRESSION, parent);
				node *n = create_node(
					TOKENNODE,
					create_token(left->node_token.col,
								 left->node_token.row, buf, NUMBER),
					x);
				append_child(x, n);
				return x;
			}
			put_error_incompatible_operation(
				"==", token_type_to_string(left->node_token.type),
				token_type_to_string(right->node_token.type), right);
		}
		}
	}
	default:
		return 0;
	}
}

bool type_check_tree(node *current, typechecker *type_checker)
{
start:
	if (!current)
	{
		return true;
	}
	switch (current->type)
	{
	case RETURN:
	{
		node *func_parent = get_parent(current, FUNCTION)->children[1];
		variable *v = get_var(
			type_checker, current->node_token.text, type_checker->scope,
			get_function(type_checker, func_parent->node_token.text));
		if (!v)
		{
			char *buffer = malloc(1024);
			sprintf(buffer, "Variable '%s' does not exist.\n",
					current->node_token.text);
		}
		if (strcmp(v->type,
				   get_function(type_checker, func_parent->node_token.text)
					   ->type))
		{
			char *buffer = malloc(1024);
			sprintf(buffer,
					"Function '%s' expects a return type of '%s' but was "
					"given a '%s'.",
					func_parent->node_token.text,
					get_function(type_checker, func_parent->node_token.text)
						->type,
					v->type);
		}
	}
	case LABEL_NODE:
	{
		bool found = false;
		for (size_t i = 0; i < type_checker->n_label; ++i)
		{
			if (!strcmp(type_checker->labels[i], current->node_token.text))
			{
				found = true;
			}
		}
		if (found)
		{
			char *buffer = malloc(1024);
			sprintf(buffer, "Label '%s' already exists.",
					current->node_token.text);
			put_error(buffer, 0, current->node_token);
		}
		type_checker->labels =
			realloc(type_checker->labels,
					(++type_checker->n_label) * sizeof(char *));
		type_checker->labels[type_checker->n_label - 1] =
			current->node_token.text;
	}
	break;
	case GOTO:
	{
		bool found = false;
		for (size_t i = 0; i < type_checker->n_label; ++i)
		{
			if (!strcmp(type_checker->labels[i], current->node_token.text))
			{
				found = true;
			}
		}
		if (!found)
		{
			char *buffer = malloc(1024);
			sprintf(buffer, "Label '%s' doesn't exist.",
					current->node_token.text);
			put_error(buffer, 0, current->node_token);
		}
	}
	break;
	case BINEXPR:
	{
		current = eval(current, current->parent);
		if (current->type == BINEXPR)
		{
			goto end;
		}
		goto start;
	}
	break;
	case WHILE:
	{
		char *type = get_type_recursive(type_checker, current->children[0]);
		if (!type || (strcmp(type, "i32") &&
					  (!starts_with(type, "i") && !starts_with(type, "u") &&
					   type[strlen(type)] == '?')))
		{
			char *buffer = malloc(1024);
			sprintf(buffer,
					"While loops can only be used using enumerable "
					"conditions. You gave a type '%s' but a while loop "
					"requires an integer type.",
					type);
			put_error(buffer, 0, current->children[0]->node_token);
		}
	}
	break;
	case EXTERN:
	{
		for (size_t i = 0; i < type_checker->n_func; ++i)
		{
			if (!strcmp(type_checker->functions[i].title,
						current->children[0]->node_token.text))
			{
				char *msg = (char *)malloc(1024);
				memset(msg, 0, 1024);
				strcat(msg, "Function '");
				strcat(msg, type_checker->functions[i].title);
				strcat(msg, "' has been redefined.");
				put_error(msg, 0, current->children[1]->node_token);
			}
		}
		// Extern is sort of a black box for the compiler, we have no idea
		// what is actually going on, since this links to any non-lowfish
		// function.
		printf("WARNING: UNSAFE FUNCTION DEFINITION FOR '%s': %s",
			   current->children[0]->node_token.text,
			   " The type checker is unable to check functions used from "
			   "extern make sure that you match the argument count and "
			   "type.\n");
		type_checker->functions =
			realloc(type_checker->functions,
					(++type_checker->n_func) * sizeof(function));
		current->children[0]->node_token.text = current->children[0]->node_token.text + 1;
		current->children[0]->node_token.text[strlen(current->children[0]->node_token.text) - 1] = 0;

		type_checker->functions[type_checker->n_func - 1] =
			(function){current->children[0]->node_token.text, "unk", -1};
	}
	break;
	case FUNCTION:
	{
		for (size_t i = 0; i < type_checker->n_func; ++i)
		{
			if (!strcmp(type_checker->functions[i].title,
						current->children[1]->node_token.text))
			{
				char *msg = (char *)malloc(1024);
				memset(msg, 0, 1024);
				strcat(msg, "Function '");
				strcat(msg, type_checker->functions[i].title);
				strcat(msg, "' has been redefined.");
				put_error(msg, 0, current->children[1]->node_token);
			}
		}
		type_checker->functions =
			realloc(type_checker->functions,
					(++type_checker->n_func) * sizeof(function));
		type_checker->functions[type_checker->n_func - 1] = (function){
			current->children[1]->node_token.text,
			current->children[0]->node_token.text, current->n_child - 3};
	}
	break;
	case FUNCTION_CALL:
	{
		bool found = false;
		size_t index = 0;
		for (size_t i = 0; i < type_checker->n_func; ++i)
		{
			printf("%s\n", type_checker->functions[i].title);
			if (!strcmp(type_checker->functions[i].title,
						current->node_token.text))
			{
				found = true;
				index = i;
			}
		}
		if (!found)
		{
			char *buffer = malloc(1024);
			memset(buffer, 0, 1024);
			sprintf(buffer, "Function '%s' doesn't exist.",
					current->node_token.text);
			put_error(buffer, 0, current->node_token);
			free(buffer);
		}

		if (type_checker->functions[index].n_args == -1)
		{
			/*
			  This is done because -1 means the function is externed and I
			  cant type check it.
			*/
			goto end;
		}
		if (current->n_child > type_checker->functions[index].n_args)
		{
			char *buffer = malloc(1024);
			memset(buffer, 0, 1024);
			sprintf(buffer, "To many args for Function '%s'.",
					current->node_token.text);
			put_error(buffer, 0, current->node_token);
			free(buffer);
		}
		if (current->n_child < type_checker->functions[index].n_args)
		{
			char *buffer = malloc(1024);
			memset(buffer, 0, 1024);
			sprintf(buffer, "To few args for Function '%s'.",
					current->node_token.text);
			put_error(buffer, 0, current->node_token);
			free(buffer);
		}
	}
	break;
	case VARDECL:
	{
		node *type = get_child_recursive(current, TYPE);
		node *title = get_child_recursive(current, TOKENNODE);
		node *func = get_parent(current, FUNCTION)->children[1];
		if (!func)
		{
			printf("HOW IS THIS POSSIBLE???\n");
			exit(9001); // hehe over 9000
		}
		variable v =
			(variable){title->node_token.text, type->node_token.text,
					   type_checker->scope,
					   get_function(type_checker, func->node_token.text)};
		if (!type_checker->variables)
		{
			type_checker->variables = malloc(sizeof(variable));
			*type_checker->variables = v;
			type_checker->n_var = 1;
		}
		else
		{
			type_checker->variables =
				realloc(type_checker->variables,
						sizeof(variable) * (type_checker->n_var + 1));
			type_checker->variables[type_checker->n_var] = v;
			type_checker->n_var++;
		}
	}
	break;
	case ASSIGNMENT:
	{
		if (current->node_token.type == DEREF)
		{
			node *title = get_child_recursive(current, TOKENNODE);
			node *node_func = get_parent(current, FUNCTION)->children[1];
			function *func =
				get_function(type_checker, node_func->node_token.text);
			variable *var = get_var(type_checker, title->node_token.text,
									type_checker->scope, func);
			if (!var)
			{
				// no need to free because put_error exits the program.
				char *buffer = malloc(1024);
				sprintf(buffer,
						"The variable you are trying to use '%s' does not "
						"exist.",
						title->node_token.text);
				put_error(buffer, 0, title->node_token);
			}
			if (!var->pointing_to)
			{
				char *buffer = malloc(1024);
				sprintf(buffer,
						"Variable '%s' is not a pointer and cannot be "
						"dereferenced.",
						var->title);
				put_error(buffer, 0, title->node_token);
			}
		}
		if (has_child_recursive(current, FUNCTION_CALL))
		{
			node *call = get_child_recursive(current, FUNCTION_CALL);
			node *title = get_child_recursive(current, TOKENNODE);
			node *node_func = get_parent(current, FUNCTION)->children[1];
			function *func =
				get_function(type_checker, node_func->node_token.text);
			variable *var = get_var(type_checker, title->node_token.text,
									type_checker->scope, func);
			if (!var)
			{
				// no need to free because put_error exits the program.
				char *buffer = malloc(1024);
				sprintf(buffer,
						"The variable you are trying to use '%s' does not "
						"exist.",
						title->node_token.text);
				put_error(buffer, 0, title->node_token);
			}
			if (func)
			{
				if (var)
				{
					if (strcmp(var->type, func->type))
					{
						put_error_incompatible_types_assignment(
							var->type, func->type,
							call); // gotta be careful since some tokens
								   // dont have a line number and column
								   // stupid me I should've thougth a small
								   // bit..
					}
				}
			}
			// if the function doesnt exist then later on when the type
			// checker actually gets to the function call an error will be
			// shown.
		}
		else if (has_child_recursive(
					 current,
					 BASICEXPRESSION)) // remember that damn else if
		{
			node *basic_expr =
				get_child_recursive(current, BASICEXPRESSION)->children[0];
			if (basic_expr->node_token.type == ID)
			{
				node *title = get_child_recursive(current, TOKENNODE);
				node *node_func =
					get_parent(current, FUNCTION)->children[1];
				function *func =
					get_function(type_checker, node_func->node_token.text);
				variable *var =
					get_var(type_checker, title->node_token.text,
							type_checker->scope, func);
				variable *var_get =
					get_var(type_checker, basic_expr->node_token.text,
							type_checker->scope, func);
				if (!var)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							title->node_token.text);
					put_error(buffer, 0, title->node_token);
				}
				if (!var_get)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							basic_expr->node_token.text);
					put_error(buffer, 0, basic_expr->node_token);
				}
				if (strcmp(var->type, var_get->type))
				{
					put_error_incompatible_types_assignment(
						var->type, var_get->type, basic_expr);
				}
			}
			else if (basic_expr->node_token.type == DEREF)
			{
				node *title = get_child_recursive(current, TOKENNODE);
				node *node_func =
					get_parent(current, FUNCTION)->children[1];
				function *func =
					get_function(type_checker, node_func->node_token.text);
				variable *var =
					get_var(type_checker, title->node_token.text,
							type_checker->scope, func);
				variable *var_get =
					get_var(type_checker, basic_expr->node_token.text,
							type_checker->scope, func);
				if (!var)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							title->node_token.text);
					put_error(buffer, 0, title->node_token);
				}
				if (!var_get)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							basic_expr->node_token.text);
					put_error(buffer, 0, basic_expr->node_token);
				}
				if (title->node_token.type == DEREF)
				{
					var = var->pointing_to;
				}
				if (!var_get->pointing_to)
				{
					char *buffer = malloc(1024);
					sprintf(buffer,
							"Variable '%s' is not a pointer and cannot be "
							"dereferenced.",
							var_get->title);
					put_error(buffer, 0, basic_expr->node_token);
				}
				if (strcmp(var->type, var_get->pointing_to->type))
				{
					printf("'%s', '%s'\n", var->type, var_get->type);
					put_error_incompatible_types_assignment(
						var->type, var_get->pointing_to->type, basic_expr);
				}
			}
			else if (basic_expr->node_token.type == REF)
			{
				node *title = get_child_recursive(current, TOKENNODE);
				node *node_func =
					get_parent(current, FUNCTION)->children[1];
				function *func =
					get_function(type_checker, node_func->node_token.text);
				variable *var =
					get_var(type_checker, title->node_token.text,
							type_checker->scope, func);
				variable *var_get =
					get_var(type_checker, basic_expr->node_token.text,
							type_checker->scope, func);
				if (!var)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							title->node_token.text);
					put_error(buffer, 0, title->node_token);
				}
				if (!var_get)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							basic_expr->node_token.text);
					put_error(buffer, 0, basic_expr->node_token);
				}
				if (title->node_token.type == DEREF)
				{
					var = var->pointing_to;
				}
				if (var_get->type && (!strcmp("u8", var_get->type) ||
									  !strcmp("i8", var_get->type)))
				{
					if (strcmp(var->type, "ptr8"))
					{
						put_error_incompatible_types_assignment(
							var->type, "ptr8", basic_expr);
					}
					var->pointing_to = var_get;
				}

				if (var_get->type && (!strcmp("u16", var_get->type) ||
									  !strcmp("i16", var_get->type)))
				{
					if (strcmp(var->type, "ptr16"))
					{
						put_error_incompatible_types_assignment(
							var->type, "ptr16", basic_expr);
					}
					var->pointing_to = var_get;
				}

				if (var_get->type && (!strcmp("u32", var_get->type) ||
									  !strcmp("i32", var_get->type)))
				{
					if (strcmp(var->type, "ptr32"))
					{
						put_error_incompatible_types_assignment(
							var->type, "ptr32", basic_expr);
					}
					var->pointing_to = var_get;
				}

				if (starts_with(var_get->type, "ptr"))
				{
					if (strcmp(var->type, "ptr"))
					{
						put_error_incompatible_types_assignment(
							var->type, "ptr", basic_expr);
					}
					var->pointing_to = var_get;
				}
			}
			else if (basic_expr->node_token.type == STRING)
			{
				node *title = get_child_recursive(current, TOKENNODE);
				node *node_func =
					get_parent(current, FUNCTION)->children[1];
				function *func =
					get_function(type_checker, node_func->node_token.text);
				variable *var =
					get_var(type_checker, title->node_token.text,
							type_checker->scope, func);
				if (!var)
				{
					// no need to free because put_error exits the program.
					char *buffer = malloc(1024);
					sprintf(buffer,
							"The variable you are trying to use '%s' does "
							"not exist.",
							title->node_token.text);
					put_error(buffer, 0, title->node_token);
				}
				if (title->node_token.type == DEREF)
				{
					var = var->pointing_to;
				}
				if (strlen(basic_expr->node_token.text) == 1)
				{
					// "a" is a char or a string
					// "aa" is a string, the distinction is multiple
					// characters. strings dont exist tho so we use 'ptr8'
					// which is alot more descriptive.
					if (strcmp(var->type, "u8") &&
						strcmp(var->type, "i8") &&
						strcmp(var->type, "ptr8"))
					{
						put_error_incompatible_types_assignment(
							var->type, "u8", basic_expr);
					}
				}
				else if (strcmp(var->type, "ptr8"))
				{
					put_error_incompatible_types_assignment(
						var->type, "ptr8", basic_expr);
				}
			}
			else if (basic_expr->node_token.type == NUMBER)
			{
				node *title = get_child_recursive(current, TOKENNODE);
				node *node_func =
					get_parent(current, FUNCTION)->children[1];
				function *func =
					get_function(type_checker, node_func->node_token.text);
				variable *var =
					get_var(type_checker, title->node_token.text,
							type_checker->scope, func);
				if (title->node_token.type == DEREF)
				{
					var = var->pointing_to;
				}
				if (strcmp(var->type, "i32") && strcmp(var->type, "i16") &&
					strcmp(var->type, "i8") && strcmp(var->type, "u8") &&
					strcmp(var->type, "u16") && strcmp(var->type, "u32"))
				{
					put_error_incompatible_types_assignment(
						var->type, "i32", basic_expr);
				}
			}
		}
	}
	break;
	}
end:
	for (size_t i = 0; i != current->n_child; ++i)
	{
		type_check_tree(current->children[i], type_checker);
	}
	return true;
}
