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

node* parse_type( token Token , node* Parent , parser* Parser )
{
    if ( !Token.text )
        return NULL;
    if
        (
            !strcmp( Token.text , "void" )
            || !strcmp( Token.text , "i64" )
            || !strcmp( Token.text , "i32" )
            || !strcmp( Token.text , "i16" )
            || !strcmp( Token.text , "i8" )
            || !strcmp( Token.text , "u64" )
            || !strcmp( Token.text , "u32" )
            || !strcmp( Token.text , "u16" )
            || !strcmp( Token.text , "u8" )
            || !strcmp( Token.text , "ptr64" )
            || !strcmp( Token.text , "ptr32" )
            || !strcmp( Token.text , "ptr16" )
            || !strcmp( Token.text , "ptr8" )
            )
    {
        return create_node( TYPE , Token , Parent );
    }
    return NULL;
}

node* parse_func( parser* Parser )
{
    --Parser->pos;
    token node_token = Parser->tokens [ Parser->pos + 1 ];
    node* n = create_node( FUNCTION , node_token , Parser->current_parent );
    node* child = parse_type( Parser->tokens [ Parser->pos ] , n , Parser );
    node* child2 = create_node( TOKENNODE , Parser->tokens [ Parser->pos + 1 ] , n );
    append_child( n , child );
    append_child( n , child2 );
    ++Parser->pos;
    expect( OPENBR , Parser , true );
    node* parent = Parser->current_parent;
    append_child( n , create_node( TOKENNODE , Parser->tokens [ Parser->pos + 1 ] , n ) );
    ++Parser->pos;
    size_t original_pos = Parser->pos;
    if ( !expect( CLOSEBR , Parser , false ) )
    {
        ++Parser->pos;
        node* next_node = parse( Parser );
        if ( next_node && next_node != Parser->root )
        {
            append_child( n , next_node );
        }
        else
        {
            Parser->pos = original_pos;
        }
    }
    expect( CLOSEBR , Parser , true );
    append_child( n , create_node( TOKENNODE , Parser->tokens [ Parser->pos + 1 ] , n ) );
    append_child( Parser->current_parent , n );
    Parser->current_parent = n;
    ++Parser->pos;
    return parse( Parser );
}
// create a node without a real token
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

node* parse_block( parser* Parser )
{
    if ( is_type( BEGINOFBLOCK ) )
    {
        node* b = create_arbitrary_node( BLOCK , Parser->current_parent );
        node* n = create_node
        (
            NORMALBLOCK ,
            Parser->tokens [ Parser->pos ] ,
            b
        );
        append_child( Parser->current_parent , b );
        append_child( b , n );
        Parser->current_parent = n;
        ++Parser->pos;
        return parse( Parser );
    }
    else if ( is_type( ARROW ) )
    {
        node* b = create_arbitrary_node( BLOCK , Parser->current_parent );
        node* n = create_node( ARROW_BLOCK , Parser->tokens [ Parser->pos ] , b );
        append_child( b , n );
        append_child( Parser->current_parent , b );
        ++Parser->pos;
        Parser->current_parent = n;
        return parse( Parser );
    }
    return parse( Parser );
}

node* parse_nl( parser* Parser )
{
    if ( Parser->current_parent->type == ARROW_BLOCK )
    {
        if ( Parser->tokens [ Parser->pos - 1 ].type != SEMI )
        {
            put_error( "Missing semi colon ';'." , 0 , Parser->tokens [ Parser->pos ] );
        }
    }
    if ( Parser->current_parent->type != NORMALBLOCK && Parser->current_parent->type != FUNCTION )
    {
        if
            (
                Parser->tokens [ Parser->pos - 1 ].type != SEMI
                && Parser->tokens [ Parser->pos - 1 ].type != OPENBR
                && Parser->tokens [ Parser->pos - 1 ].type != CLOSEBR
                && Parser->tokens [ Parser->pos - 1 ].type != BEGINOFBLOCK
                && Parser->tokens [ Parser->pos - 1 ].type != ENDOFBLOCK
            )
        {
            put_error( "Missing semi colon ';'." , 0 , Parser->tokens [ Parser->pos ] );
        }
    }
    ++Parser->pos;
    return parse( Parser );
}
#define expect_err(x) \
    expect(x, Parser, true); \
    ++Parser->pos

node* parse_extern( parser* Parser )
{
    node* extern_node = create_arbitrary_node( EXTERN , Parser->current_parent );
    expect_err( OPENBR );
    node* obr_node = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , extern_node );
    expect_err( STRING );
    node* str_node = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , extern_node );
    expect_err( CLOSEBR );
    node* cbr_node = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , extern_node );
    append_child( extern_node , obr_node );
    append_child( extern_node , str_node );
    append_child( extern_node , cbr_node );
    append_child( Parser->current_parent , extern_node );
    return parse( Parser );
}

node* parse_asm( parser* Parser )
{
    node* asm_node = create_arbitrary_node( ASM , Parser->current_parent );
    expect_err( OPENBR );
    node* obr_node = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , asm_node );
    expect_err( STRING );
    node* str_node = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , asm_node );
    expect_err( CLOSEBR );
    node* cbr_node = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , asm_node );
    append_child( asm_node , obr_node );
    append_child( asm_node , str_node );
    append_child( asm_node , cbr_node );
    append_child( Parser->current_parent , asm_node );
    return parse( Parser );
}

node* parse_basic_expression( parser* Parser , node* parent )
{
    node* n = create_arbitrary_node( BASICEXPRESSION , parent );
    node* n1 = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , n );
    append_child( n , n1 );
    if ( parent )
    {
        append_child( parent , n );
    }
    return n;
}

node* parse_factor( parser* Parser )
{
    node* left = parse_basic_expression( Parser , 0 );
    while ( expect( DIV , Parser , false ) || expect( MUL , Parser , false ) )
    {
        node* operator = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , 0 );
        ++Parser->pos;
        ++Parser->pos;
        node* right = parse_basic_expression( Parser , operator );
        append_child( operator, left );
        append_child( operator, right );
        left->parent = operator;
        left = operator;
    }
    return left;
}

node* parse_primary( parser* Parser )
{
    node* left = parse_factor( Parser );
    bool is_complex = false;
    while ( expect( PLUS , Parser , false ) || expect( MINUS , Parser , false ) )
    {
        node* operator = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , 0 );
        ++Parser->pos;
        node* right = parse_factor( Parser );
        append_child( operator, left );
        append_child( operator, right );
        left->parent = operator;
        left = operator;
        is_complex = true;
    }
    if ( !is_complex )
    {
        ++Parser->pos;
        left->parent = Parser->current_parent;
        append_child( Parser->current_parent , left );
        return parse( Parser );
    }
    ++Parser->pos;
    node* n = create_arbitrary_node( EXPRESSION , Parser->current_parent );
    left->parent = n;
    append_child( n , left );
    append_child( Parser->current_parent , n );
    return parse( Parser );
}


node* parse( parser* Parser )
{
    if ( Parser->pos < Parser->n_token )
    {
        if ( parse_type( Parser->tokens [ Parser->pos ] , 0 , Parser ) )
        {
#pragma region function
            if
                (

                    expect( ID , Parser , false )
                    && ++Parser->pos
                    && expect( OPENBR , Parser , false )
                )
            {
                return parse_func( Parser );
            }
#pragma endregion
        }
#pragma region block
        else if ( is_type( BEGINOFBLOCK ) || is_type( ARROW ) )
        {
            return parse_block( Parser );
        }
        else if ( is_type( ENDOFBLOCK ) )
        {
            Parser->current_parent = Parser->current_parent->parent->parent;
            ++Parser->pos;
            return parse( Parser );
        }
#pragma endregion
#pragma region semi colon
        else if ( is_type( SEMI ) )
        {
            if ( Parser->current_parent->type == ARROW_BLOCK )
            {
                Parser->current_parent = Parser->current_parent->parent->parent;
                ++Parser->pos;
                return parse( Parser );
            }
            ++Parser->pos;
        }
#pragma endregion
#pragma region new line
        else if ( is_type( NL ) )
        {
            return parse_nl( Parser );
        }
#pragma endregion
        else if ( is_type( NUMBER ) )
        {
            return parse_primary( Parser );
        }
        else if ( is_type( STRING ) )
        {
            return parse_primary( Parser );
        }
#pragma region identifier
        else if ( is_type( ID ) )
        {
            if ( !strcmp( "extern" , Parser->tokens [ Parser->pos ].text ) )
            {
                return parse_extern( Parser );
            }
            else if ( !strcmp( "asm" , Parser->tokens [ Parser->pos ].text ) )
            {
                return parse_asm( Parser );
            }
            else
            {
                if ( expect( OPENBR , Parser , false ) )
                {
                    ++Parser->pos;
                    return parse( Parser );
                }

                return parse_primary( Parser );
            }
            ++Parser->pos;
            return parse( Parser );
        }
#pragma endregion
        else
        {
            ++Parser->pos;
        }
        return parse( Parser );
    }
    return Parser->root;
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