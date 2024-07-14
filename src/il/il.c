#include "il.h"

#include <stdio.h>

#include "../typechecker/typechecker.h"

node **get_mul(node **list, int *len, node *p)
{

	if (p->type == BASICEXPRESSION)
	{
		return list;
	}
	node *op = p;
	if (p->type == BINEXPR)
	{
		op = p->children[0];
	}
	if (op->node_token.type == MUL)
	{
		list[*len] = op;
		list = realloc(list, (*len + 1) * sizeof(node *));
		++len[0];
	}
	if (op->n_child == 2)
	{
		get_mul(list, len, op->children[0]);
		get_mul(list, len, op->children[1]);
	}
	return list;
}

char *visit_node(il_generator *il_gen, node *curr)
{
	if (!curr)
	{
		return "";
	}
	switch (curr->type)
	{
	case FUNCTION:
	{
		char *out = malloc(strlen("function  ") +
						   strlen(curr->children[1]->node_token.text));
		sprintf(out, "function %s\n", curr->children[1]->node_token.text);

		node *block = curr->children[2];
		for (int i = 0; i < block->n_child; ++i)
		{
			char *str = visit_node(il_gen, block->children[i]);
			int x = strlen(str);
			out = realloc(out, x + strlen(out) + 2);
			strcat(out, str);
			strcat(out, "\n");
			free(str);
		}
		printf(out);

		return out;
	}
	case TOKENNODE:
	{
		if (curr->node_token.type == MUL)
		{
			char *left;
			char *right;
			left = visit_node(il_gen, curr->children[0]);
			right = visit_node(il_gen, curr->children[1]);
			char *fmt = "%s mul %s";
			char *out = malloc(100);
			sprintf(out, fmt, left, right);
			free(left);
			free(right);
			return out;
		}
	}
	case BINEXPR:
	{
		if (curr->children[0]->node_token.type == MUL)
		{
			char *left;
			char *right;
			left = visit_node(il_gen, curr->children[0]->children[0]);
			right = visit_node(il_gen, curr->children[0]->children[1]);
			char *fmt = "%s mul %s";
			char *out = malloc(100);
			sprintf(out, fmt, left, right);
			free(left);
			free(right);
			return out;
		}
		if (curr->children[0]->node_token.type == DIV)
		{
			char *left;
			char *right;
			left = visit_node(il_gen, curr->children[0]->children[0]);
			right = visit_node(il_gen, curr->children[0]->children[1]);
			char *fmt = "%s div %s";
			char *out = malloc(100);
			sprintf(out, fmt, left, right);
			free(left);
			free(right);
			return out;
		}
		int len = 0;
		node **multiplications = get_mul(malloc(sizeof(node *)), &len, curr);
		if (len == 0)
		{
			if (curr->children[0]->node_token.type == PLUS)
			{
				char *left;
				char *right;
				left = visit_node(il_gen, curr->children[0]->children[0]);
				right = visit_node(il_gen, curr->children[0]->children[1]);
				char *fmt = "%s add %s";
				char *out = malloc(100);
				sprintf(out, fmt, left, right);
				free(left);
				free(right);
				return out;
			}
			char *left;
			char *right;
			left = visit_node(il_gen, curr->children[0]->children[0]);
			right = visit_node(il_gen, curr->children[0]->children[1]);
			char *fmt = "%s sub %s";
			char *out = malloc(100);
			sprintf(out, fmt, left, right);
			free(left);
			free(right);
			return out;
		}
		char *out = malloc(1000);
		for (int i = 0; i < len; ++i)
		{
			char *x = visit_node(il_gen, multiplications[i]);
			strcat(out, x);
		}
		char *str = malloc(100);
		char *fmt = " add %s";
		sprintf(str, fmt, visit_node(il_gen, curr->children[0]->children[0]));
		strcat(out, str);
		free(str);
		return out;
	}
	break;
	case BASICEXPRESSION:
	{
		switch (curr->children[0]->node_token.type)
		{
		case NUMBER:
		{
			return curr->children[0]->node_token.text;
		}
		break;
		}
	}
	break;
	default:
	{
		return visit_node(il_gen, curr->children[0]);
	}
	}
}

char *generate_il(node *root, typechecker *type_checker)
{
	il_generator generator =
		(il_generator){root, root, 0, malloc(1), type_checker};
	char *src = visit_node(&generator, root);

	return generator.src;
}
