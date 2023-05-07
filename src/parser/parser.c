#include"parser.h"

#define node_type_to_string(x) \
    (((char*[]){"PROGRAM", "FUNCTION", "TYPE", "TOKEN_NODE","BLOCK", "NORMALBLOCK", "ARROWBLOCK"})[x])

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
        append_child( b , n );
        append_child( Parser->current_parent , b );
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


node* parse( parser* Parser )
{
    if ( Parser->pos < Parser->n_token )
    {
        if ( parse_type( Parser->tokens [ Parser->pos ] , 0 , Parser ) )
        {
            // [type] [name] '(' * ')'
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
        else if ( is_type( NL ) )
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
        }
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