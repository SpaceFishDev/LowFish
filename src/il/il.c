#include "il.h"
#include "../typechecker/typechecker.h"

#include <stdio.h>

void visit_node(il_generator *il_gen, node *curr)
{
	// stage one
	switch (curr->type)
	{
	case EXTERN:
	{
		char *buffer = malloc(1 + strlen("extern ") +
							  strlen(curr->children[0]->node_token.text));
		sprintf(buffer, "extern %s", curr->children[0]->node_token.text);
		append_src(buffer);
		free(buffer);
	}
	break;
	case ASSIGNMENT:
	{
		if (curr->parent->type == BASICEXPRESSION)
		{
			char *var = curr->children[0]->node_token.text;
			node *n = eval(curr->children[1], curr);
			if (n->type == BASICEXPRESSION)
			{
				n = n->children[0];
				char *value = n->node_token.text;
				char *type = get_type_recursive(il_gen->type_checker, n);
				char *buffer = malloc(1 + strlen(var) + strlen(value) + 50);
				sprintf(buffer, "set %s %s", var, value);
				append_src(buffer);
				free(buffer);
			}
			else
			{
				if (n->children[0]->node_token.text[0] == '+')
				{
					char *buffer = malloc(1024);
					sprintf(buffer, "add %s %s\nset %s %s ", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, var, "%rax");
					append_src(buffer);
					free(buffer);
				}
				else if (n->children[0]->node_token.text[0] == '-')
				{
					char *buffer = malloc(1024);
					sprintf(buffer, "sub %s %s\nset %s %s ", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, var, "%rax");
					append_src(buffer);
					free(buffer);
				}
				else if (n->children[0]->node_token.text[0] == '*')
				{
					char *buffer = malloc(1024);
					sprintf(buffer, "mul %s %s\nset %s %s ", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, var, "%rax");
					append_src(buffer);
					free(buffer);
				}
				else if (n->children[0]->node_token.text[0] == '/')
				{
					char *buffer = malloc(1024);
					sprintf(buffer, "div %s %s\nset %s %s ", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, var, "%rax");
					append_src(buffer);
					free(buffer);
				}
			}
		}
	}
	break;
	case FUNCTION:
	{
		char *type = get_function(il_gen->type_checker,
								  curr->children[1]->node_token.text)
						 ->type;
		char *buffer = malloc(strlen(curr->children[1]->node_token.text) +
							  1 + strlen("function ") + 1 + strlen(type));
		sprintf(buffer, "%s %s %s", "function", type,
				curr->children[1]->node_token.text);
		append_src(buffer);
		free(buffer);
	}
	break;
	case VARDECL:
	{
		char *type = curr->children[0]->node_token.text;
		char *title = curr->children[1]->children[0]->node_token.text;

		char *value = eval(curr->children[1]->children[1], curr->children[1]->children[1]->parent)->children[0]->node_token.text;
		if (eval(curr->children[1]->children[1], curr->children[1]->children[1]->parent)->type == BINEXPR)
		{
			node *n = eval(curr->children[1]->children[1], curr->children[1]->children[1]->parent);
			if (n->children[0]->node_token.text[0] == '+')
			{
				char *buffer = malloc(1024);
				sprintf(buffer, "add %s %s\nvar %s %s %s", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, type, title, "\%rax");
				append_src(buffer);
				free(buffer);
			}
			else if (n->children[0]->node_token.text[0] == '-')
			{
				char *buffer = malloc(1024);
				sprintf(buffer, "sub %s %s\nvar %s %s %s", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, type, title, "\%rax");
				append_src(buffer);
				free(buffer);
			}
			else if (n->children[0]->node_token.text[0] == '/')
			{
				char *buffer = malloc(1024);
				sprintf(buffer, "div %s %s\nvar %s %s %s", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, type, title, "\%rax");
				append_src(buffer);
				free(buffer);
			}
			else if (n->children[0]->node_token.text[0] == '*')
			{
				char *buffer = malloc(1024);
				sprintf(buffer, "mul %s %s\nvar %s %s %s", n->children[0]->children[0]->children[0]->node_token.text, n->children[0]->children[1]->children[0]->node_token.text, type, title, "\%rax");
				append_src(buffer);
				free(buffer);
			}
		}
		else
		{
			char *buffer = malloc(1024);
			sprintf(buffer, "var %s %s %s", type, title, value);
			append_src(buffer);
			free(buffer);
		}
	}
	break;
	case BASICEXPRESSION:
	{
		if (curr->parent->type == FUNCTION_CALL)
		{
			switch (curr->children[0]->node_token.type)
			{
			case STRING:
			{
				if (strlen(curr->children[0]->node_token.text) == 1)
				{
					char *buffer = malloc(
						strlen(curr->children[0]->node_token.text) + 2 +
						strlen("arg i8 ") + 1);
					sprintf(buffer, "arg i8 \"%s\"",
							curr->children[0]->node_token.text);
					append_src(buffer);
					free(buffer);
				}
				else
				{
					char *buffer = malloc(
						strlen(curr->children[0]->node_token.text) + 2 +
						strlen("arg ptr8 ") + 1);
					sprintf(buffer, "arg ptr8 \"%s\"",
							curr->children[0]->node_token.text);
					append_src(buffer);
					free(buffer);
				}
			}
			break;
			case NUMBER:
			{
				char *buffer =
					malloc(strlen(curr->children[0]->node_token.text) +
						   strlen("arg i32 ") + 1);
				sprintf(buffer, "arg i32 %s",
						curr->children[0]->node_token.text);
				append_src(buffer);
				free(buffer);
			}
			break;
			}
		}
	}
	break;
	}
	for (size_t i = 0; i < curr->n_child; ++i)
	{
		visit_node(il_gen, curr->children[i]);
	}
	// stage 2
	switch (curr->type)
	{
	case FUNCTION:
	{
		append_src("return");
	}
	break;
	case FUNCTION_CALL:
	{
		char *buffer =
			malloc(strlen(curr->node_token.text) + strlen("goto ") + 1);
		sprintf(buffer, "%s%s", "call ", curr->node_token.text);
		append_src(buffer);
	}
	}
}

char *generate_il(node *root, typechecker *type_checker)
{
	il_generator generator =
		(il_generator){root, root, 0, malloc(1), type_checker};
	visit_node(&generator, root);
	return generator.src;
}
