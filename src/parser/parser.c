#include"parser.h"

bool expect(token_type type, parser* Parser)
{
    return Parser->tokens[Parser->pos].type == type;
}

node* parse_type(token Token, node* Parent, parser* Parser)
{
    if
        (
            !strcmp(Token.text, "void")
            || !strcmp(Token.text, "i64")
            || !strcmp(Token.text, "i32")
            || !strcmp(Token.text, "i16")
            || !strcmp(Token.text, "i8")
            || !strcmp(Token.text, "u64")
            || !strcmp(Token.text, "u32")
            || !strcmp(Token.text, "u16")
            || !strcmp(Token.text, "u8")
            || !strcmp(Token.text, "ptr64")
            || !strcmp(Token.text, "ptr32")
            || !strcmp(Token.text, "ptr16")
            || !strcmp(Token.text, "ptr8")
            )
    {
        return create_node(TYPE, Token, Parent);
    }
    else if (!strcmp(Token.text, "struct"))
    {
        if (expect(ID, Parser) && !expect(BEGINOFBLOCK, Parser))
        {
            return create_node(TYPE, Parser->tokens[Parser->pos + 1], Parent);
        }
    }
    return NULL;
}

node* create_node(node_type type, token node_token, node* parent)
{
    node* n = malloc(sizeof(node));
    n->type = type;
    n->node_token = node_token;
    n->parent = parent;
    n->children = malloc(sizeof(void*));
    n->n_child = 0;
}
parser* create_parser(token* tokens, size_t n_token)
{
    parser* Parser = malloc(sizeof(parser));
    Parser->root = create_node(PROGRAM, create_token(0, 0, "PROGRAM", END_OF_FILE), 0);
    Parser->current_parent = Parser->root;
    Parser->pos = 0;
    Parser->tokens = tokens;
    Parser->n_token = n_token;
    return Parser;
}

#define append_child(x, y) \
    x->children = realloc(x->children, ++x->n_child); \
    x->children[x->n_child - 1] = y

node* parse(parser* Parser)
{
    if (Parser->pos < Parser->n_token)
    {
        if (parse_type(Parser->tokens[Parser->pos], 0, Parser))
        {
            if (expect(ID, Parser))
            {
                token node_token = Parser->tokens[Parser->pos + 1];
                node* n = create_node(FUNCTION, node_token, Parser->current_parent);
                node* child = parse_type(Parser->tokens[Parser->pos], 0, Parser);
                node* child2 = create_node(Parser->tokens[]);
                append_child(n, child);
                append_child(Parser->current_parent, n);
                Parser->current_parent = n;
                ++Parser->pos;
                return parse(Parser);
            }
        }
        ++Parser->pos;
        return parse(Parser);
    }
    return Parser->root;
}

#define node_type_to_string(x) \
    (((char*[]){"PROGRAM", "FUNCTION", "TYPE", "IDNODE", "OPENBRNODE", "CLOSEBRNODE"})[x])

void print_tree(node* n, int indent)
{
    for (size_t i = 0; i < indent; ++i)
    {
        printf(" ");
    }
    printf("NODE(%s, %s)\n", n->node_token.text, node_type_to_string(n->type));
    ++indent;
    for (size_t i = 0; i < n->n_child; ++i)
    {
        print_tree(n->children[i], indent);
    }
}