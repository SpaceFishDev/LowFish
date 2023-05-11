#include"parser.h"

#define node_type_to_string(x) \
    (((char*[]){"PROGRAM", "FUNCTION", "TYPE", "TOKEN_NODE","BLOCK", "NORMALBLOCK", "ARROWBLOCK", "EXTERN", "ASM", "BASICEXPRESSION", "EXPRESSION"})[x])

node* append_child_to_x( node* x , node* y )
{
    x->children = realloc( x->children , sizeof( node* ) * ( x->n_child + 1 ) );
    x->children [ x->n_child ] = y;
    ++x->n_child;
    return x;
}

#define append_child(x,y) \
    x = append_child_to_x(x,y)

void put_error( char* msg , node* Node , token t )
{
    if ( Node )
        printf( "%s LN: %d COL: %d\n" , msg , Node->node_token.row , Node->node_token.col );
    else
        printf( "%s LN: %d COL: %d\n" , msg , t.row , t.col );

    exit( -1 );
}

bool expect( token_type type , parser* Parser , bool error )
{
    if ( Parser->tokens [ Parser->pos + 1 ].type == type )
    {
        return true;
    }
    if ( error )
    {
        char* msg = "Expected '%s' but got '%s'.";
        char* buffer = malloc( 1024 );
        sprintf( buffer , msg , token_type_to_string( type ) , token_type_to_string( Parser->tokens [ Parser->pos + 1 ].type ) );
        put_error( buffer , 0 , Parser->tokens [ Parser->pos + 1 ] );
    }
    return false;
}

node* create_arbitrary_node( node_type type , node* parent )
{
    token node_token = create_token( 0 , 0 , node_type_to_string( type ) , NOTOKEN );
    node* n = malloc( sizeof( node ) );
    n->type = type;
    n->node_token = node_token;
    n->parent = parent;
    n->children = malloc( sizeof( void* ) );
    n->n_child = 0;
    return n;
}

node* create_node( node_type type , token node_token , node* parent )
{
    node* n = malloc( sizeof( node ) );
    n->type = type;
    n->node_token = node_token;
    n->parent = parent;
    n->children = malloc( sizeof( void* ) );
    n->n_child = 0;
    return n;
}
parser* create_parser( token* tokens , size_t n_token )
{
    parser* Parser = malloc( sizeof( parser ) );
    Parser->root = create_node( PROGRAM , create_token( 0 , 0 , "PROGRAM" , END_OF_FILE ) , 0 );
    Parser->current_parent = Parser->root;
    Parser->pos = 0;
    Parser->tokens = tokens;
    Parser->n_token = n_token;
    return Parser;
}

#define is_type(t) \
    ((bool)(Parser->tokens[Parser->pos].type == t))

#define expect_err(x) \
    expect(x, Parser, true); \
    ++Parser->pos

#define expect_no_err(x) \
    expect(x, Parser, false)

node* parse_basic_expression( parser* Parser )
{
    node* arb_expr = create_arbitrary_node( BASICEXPRESSION , 0 );
    node* expr = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , arb_expr );
    append_child( arb_expr , expr );
    return arb_expr;
}

node* parse_factor( parser* Parser )
{
    node* left = parse_basic_expression( Parser );
    while ( expect_no_err( DIV ) || expect_no_err( MUL ) )
    {
        ++Parser->pos;
        node* operator = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , left );
        ++Parser->pos;
        node* right = parse_basic_expression( Parser );
        append_child( operator , left );
        append_child( operator , right );
        left->parent = operator;
        right->parent = operator;
        left = operator;
    }
    return left;
}
node* parse_primary( parser* Parser )
{
    node* left = parse_factor( Parser );
    while ( expect_no_err( PLUS ) || expect_no_err( MINUS ) )
    {
        ++Parser->pos;
        node* operator = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , left );
        ++Parser->pos;
        node* right = parse_factor( Parser );
        left->parent = operator;
        right->parent = operator;
        append_child( operator, left );
        append_child( operator, right );
        left = operator;

    }
    return left;
}

node* parse_expression( parser* Parser )
{
    node* arb_expr = create_arbitrary_node( EXPRESSION , Parser->current_parent );
    node* expr = parse_primary( Parser );
    expr->parent = arb_expr;
    append_child( arb_expr , expr );
    append_child( Parser->current_parent , arb_expr );
    return arb_expr;
}

node* parse( parser* Parser )
{
    if ( Parser->pos > Parser->n_token )
    {
        return Parser->root;
    }
    if ( is_type( ID ) )
    {
        if ( !expect_no_err( OPENBR ) )
        {
            node* expr = parse_expression( Parser );
            ++Parser->pos;
            return parse( Parser );
        }
    }
    if ( is_type( NUMBER ) )
    {
        node* expr = parse_expression( Parser );
        ++Parser->pos;
        return parse( Parser );
    }
    ++Parser->pos;
    return parse( Parser );
}

void print_tree( node* n , int indent )
{
    print_n_times( "    " , indent );
    printf( "<NODE( '%s', %s)>\n" , n->node_token.text , node_type_to_string( n->type ) );
    if ( n->n_child > 0 )
    {
        print_n_times( "    " , indent );
        printf( "{\n" );
    }
    ++indent;
    for ( size_t i = 0; i < n->n_child; ++i )
    {
        print_tree( n->children [ i ] , indent );
    }
    if ( n->n_child > 0 )
    {
        print_n_times( "    " , indent - 1 );
        printf( "}\n" );
    }
}