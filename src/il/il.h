#include "../typechecker/typechecker.h"

typedef struct
{
	node* root;
	node* current;
	size_t length_of_src;
	char* src;	// generated source code NOT the input source code!
	typechecker* type_checker;
} il_generator;
#define append_src(_src)                                                \
	il_gen->src =                                                       \
		realloc(il_gen->src, il_gen->length_of_src + strlen(_src) + 2); \
	il_gen->length_of_src += strlen(_src) + 1;                          \
	strcat(il_gen->src, _src);                                          \
	il_gen->src[il_gen->length_of_src - 1] = '\n';                      \
	il_gen->src[il_gen->length_of_src] = 0;

void visit_node(il_generator* generator, node* curr);

char* generate_il(node* root, typechecker* type_checker);

