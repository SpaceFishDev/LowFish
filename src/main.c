#include <setjmp.h>
#include <signal.h>

#include "il/il.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "typechecker/typechecker.h"
#include "util.h"
// if you're looking for comments...
// about that...

char *get_output_file(char **argv)
{
	while (*argv)
	{
		if (!strcmp(*argv, "-o"))
		{
			return *(++argv);
		}
		++argv;
	}
}

char *get_input_file(char **argv)
{
	char *input = "test.lf";
	while (*argv)
	{
		if ((*argv)[0] != '-')
		{
			input = *argv;
			break;
		}
	}
	return input;
}

node *global_tree;
token *global_tokens;
parser *global_parser;
typechecker *global_typechecker;

int state = 0;

enum states
{
	LEXING,
	PARSING,
	RESTRUCTURING,
	TYPECHECKING,
	IL_CREATION,
};

void panic_handler(int s)
{
	if (s == SIGSEGV)
	{
		printf("ERROR: An unkown error occurred.\n");
		printf("The error occured at during the '%s' stage.\n",
			   ((char *[]){"lexing", "parsing", "tree restructuring",
						   "typechecking", "il creation"})[state]);
		printf(
			"This error could not be resolved in any way and so the compiler "
			"will "
			"exit.\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	signal(SIGSEGV, panic_handler);
	++argv;
	char *input_file = read_file(get_input_file(argv));

	lexer Lexer = (lexer){input_file, 1, 0, 0};
	token *Tokens = malloc(sizeof(token));
	size_t n_token = 1;
	for (;;)
	{
		token T = lex(&Lexer);
		printf("TOKEN(%s,%s, %d)\n", T.text,
			   (T.type <= DEREF) ? (token_type_to_string(T.type)) : "UNK", T.type);
		Tokens = realloc(Tokens, n_token * sizeof(token));
		Tokens[n_token - 1] = T;
		++n_token;
		if (T.type == BAD || T.type == END_OF_FILE)
		{
			break;
		}
	}
	++state;

	free(input_file);
	parser *Parser = create_parser(Tokens, n_token - 1);
	node *root = parse(Parser);
	++state;

	restructure_parents(root);
	++state;
	free(Tokens);

	print_tree(root, 0);

	typechecker *Typechecker = calloc(sizeof(typechecker), 1);
	type_check_tree(root, Typechecker);
	++state;
	char *il = generate_il(root, Typechecker);
	printf("--IL--\n\n%s\n", il);
	free_tree(root);
	free(Typechecker->functions);
	free(Typechecker->variables);
	free(Typechecker);
	return 0;
}
