#include"parser.h"

#define node_type_to_string(x) \
    (((char*[]){ \
"PROGRAM" , \
"FUNCTION" , \
"TYPE" , \
"TOKEN_NODE" , \
"BLOCK" , \
"NORMALBLOCK" , \
"ARROWBLOCK" , \
"EXTERN" , \
"ASM" , \
"BASICEXPRESSION" , \
"EXPRESSION" , \
"BINEXPR" , \
"FUNCTION_CALL" , \
"VARDECL" , \
"ASSIGNMENT" , \
"CONDITIONAL" , \
"IF" , \
"WHILE" , \
"ELSE" , \
        } ) [ x ] )

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
        // exits the program so no need to free.
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

#define expect_err(x) \
    expect(x, Parser, true); \
    ++Parser->pos
#define expect_err_no_inc(x) \
    expect(x, Parser, true)

#define expect_no_err(x) \
    expect(x, Parser, false)

#define conditional_token(x) \
    !strcmp("if", x.text) || !strcmp("else", x.text) || !strcmp("while", x.text) || !strcmp("for", x.text)


bool f_is_type( int type , parser* Parser , bool err )
{
    if ( Parser->tokens [ Parser->pos ].type == type )
    {
        return true;
    }
    if ( err )
    {
        char* msg = "Expected '%s' but got '%s'.";
        char* buffer = malloc( 1024 );
        sprintf( buffer , msg , token_type_to_string( type ) , token_type_to_string( Parser->tokens [ Parser->pos + 1 ].type ) );
        put_error( buffer , 0 , Parser->tokens [ Parser->pos ] );
    }
    return false;
}
#define is_type(x) (f_is_type(x, Parser, false))
#define is_type_err(x) (f_is_type(x, Parser, true))

node* parse_primary( parser* Parser );

#define type_token(x) \
    (( !strcmp(x.text, "struct") || !strcmp(x.text, "i32") || !strcmp(x.text, "i16") || !strcmp(x.text, "i8")) || (!strcmp(x.text, "u32") || !strcmp(x.text, "u16") || !strcmp(x.text, "u8"))) 

node* parse_expression( parser* Parser );
node* parse_assignment( parser* Parser , node* parent )
{
    node* id = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , parent );
    if ( !expect_no_err( EQUAL ) )
    {
        return id;
    }

    // safe to assume its an actual assignment 
    expect_err( EQUAL );
    ++Parser->pos;// skip the equal
    node* next = parse_primary( Parser );
    node* assign = create_arbitrary_node( ASSIGNMENT , parent );
    append_child( assign , id );
    append_child( assign , next );
    --Parser->pos; // go back or weird errors happen??
    return assign;

}
node* parse_function_call( parser* Parser )
{
    if ( !strcmp( "extern" , Parser->tokens [ Parser->pos ].text ) )
    {
        expect_err( OPENBR );
        expect_err( STRING );
        node* arb = create_arbitrary_node( EXTERN , 0 );
        node* tok = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , arb );
        append_child( arb , tok );
        expect_err( CLOSEBR );
        return arb;
    }
    if ( !strcmp( "asm" , Parser->tokens [ Parser->pos ].text ) )
    {
        expect_err( OPENBR );
        expect_err( STRING );
        node* arb = create_arbitrary_node( ASM , 0 );
        node* tok = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , arb );
        append_child( arb , tok );
        expect_err( CLOSEBR );
        return arb;
    }
    node* id_node = create_node( FUNCTION_CALL , Parser->tokens [ Parser->pos ] , Parser->current_parent );
    expect_err( OPENBR );

    ++Parser->pos;
    if ( !is_type( CLOSEBR ) )
    {
        while ( !is_type( CLOSEBR ) )
        {
            if ( is_type( COMMA ) )
                ++Parser->pos;
            node* next = parse_primary( Parser );
            append_child( id_node , next );
        }
    }
    return id_node;
}
node* parse_id( parser* Parser )
{
    if ( !expect_no_err( OPENBR ) && !type_token( Parser->tokens [ Parser->pos ] ) )
    {
        node* id = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , 0 );
        node* assign = parse_assignment( Parser , 0 );
        node* arb_expr = create_arbitrary_node( BASICEXPRESSION , 0 );
        if ( assign->type != TOKENNODE )
        {
            assign->parent = arb_expr;
            append_child( arb_expr , assign );
            return arb_expr;

        }
        append_child( arb_expr , id );
        return arb_expr;
    }
    else if ( type_token( Parser->tokens [ Parser->pos ] ) )
    {

        node* type_node = create_node( TYPE , Parser->tokens [ Parser->pos ] , 0 );
        expect_err( ID );
        if ( !strcmp( Parser->tokens [ Parser->pos - 1 ].text , "struct" ) )
        {
            // hopefully not a struct definition. (while we're still building out the parser)
            type_node = create_node( TYPE , Parser->tokens [ Parser->pos + 1 ] , 0 ); // we just assume it isnt just for testing purposes.
            expect_err( ID );
        }
        if ( !expect_no_err( OPENBR ) )
        {
            node* var_decl = create_arbitrary_node( VARDECL , Parser->current_parent );
            node* id = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , var_decl );
            type_node->parent = var_decl;
            node* assignment = parse_assignment( Parser , var_decl );
            if ( assignment->type == TOKENNODE )
            {
                // its not an assignment
                append_child( var_decl , type_node );
                append_child( var_decl , id );
                return var_decl;
            }
            // safely assume its an assignment
            append_child( var_decl , type_node );
            append_child( var_decl , assignment );
            return var_decl;
        }

        //safe to assume its a function declaration.
        // which is a problem becuase that should've been handled :((((

    }
    else if ( expect_no_err( OPENBR ) )
    {
        return parse_function_call( Parser );
    }
}

node* parse_basic_expression( parser* Parser )
{
    if ( is_type( ID ) )
    {
        node* expr = parse_id( Parser );
        return expr;
    }
    node* arb_expr = create_arbitrary_node( BASICEXPRESSION , 0 );
    node* expr = create_node( TOKENNODE , Parser->tokens [ Parser->pos ] , arb_expr );
    append_child( arb_expr , expr );
    return arb_expr;
}

node* parse_factor( parser* Parser )
{

    node* left = parse_basic_expression( Parser );

    while
        (
             expect_no_err( DIV )
             || expect_no_err( MUL )
             || expect_no_err( BOOLEQUAL )
             || expect_no_err( BOOLNOTEQUAL )
             || expect_no_err( LESS )
             || expect_no_err( MORE )
             || expect_no_err( MOREEQUAL )
             || expect_no_err( LESSEQUAL )
        )
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
    if ( left->type == BLOCK )
    {
        Parser->current_parent = left;
    }
    while ( expect_no_err( PLUS ) || expect_no_err( MINUS ) || expect_no_err( BOOLAND ) || expect_no_err( BOOLOR ) )
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
    if ( expect_no_err( SEMI ) )
    {
        ++Parser->pos;
    }
    if ( left->n_child != 0 && ( left->type == TOKENNODE ) )
    {
        node* binexpr_node = create_arbitrary_node( BINEXPR , 0 );
        left->parent = binexpr_node;
        append_child( binexpr_node , left );
        ++Parser->pos;
        return binexpr_node;
    }
    ++Parser->pos;
    return left;
}

node* parse_conditional( parser* Parser )
{
    node* cond = create_arbitrary_node( CONDITIONAL , Parser->current_parent );
    // Parser->current_parent = cond;
    if ( !strcmp( "if" , Parser->tokens [ Parser->pos ].text ) )
    {
        append_child( Parser->current_parent , cond );
        node* arb_if = create_arbitrary_node( IF , cond );
        expect_err( OPENBR );
        ++Parser->pos;
        node* expr = parse_primary( Parser );
        ++Parser->pos;
        // expect_err( CLOSEBR );
        expr->parent = arb_if;
        append_child( cond , arb_if );
        append_child( arb_if , expr );
        Parser->current_parent = arb_if;

        return parse( Parser );
    }
    if ( !strcmp( "while" , Parser->tokens [ Parser->pos ].text ) )
    {
        append_child( Parser->current_parent , cond );
        node* arb_while = create_arbitrary_node( WHILE , cond );
        expect_err( OPENBR );
        ++Parser->pos;
        node* expr = parse_primary( Parser );
        ++Parser->pos;
        // expect_err( CLOSEBR );
        expr->parent = arb_while;
        append_child( cond , arb_while );
        append_child( arb_while , expr );
        Parser->current_parent = arb_while;

        return parse( Parser );
    }
    if ( !strcmp( "else" , Parser->tokens [ Parser->pos ].text ) )
    {
        if ( expect_no_err( BEGINOFBLOCK ) )
        {
            // normal else
            if ( Parser->current_parent->n_child && Parser->current_parent->children [ Parser->current_parent->n_child - 1 ]->type == CONDITIONAL )
            {
                node* arb_else = create_arbitrary_node( ELSE , cond );
                ++Parser->pos;
                append_child( Parser->current_parent->children [ Parser->current_parent->n_child - 1 ] , arb_else );
                Parser->current_parent = arb_else;
                return parse( Parser );
            }
            put_error( "Floating else statement." , 0 , Parser->tokens [ Parser->pos ] );
        }
        // the comment under this explains the current state.
    }
    // I'll do something when I'm not lazy, for now just a seg fault
}
node* parse_func( parser* Parser )
{
    node* type_node = create_node( TYPE , Parser->tokens [ Parser->pos - 1 ] , 0 );
    expect_err( OPENBR );
    node* func = create_arbitrary_node( FUNCTION , 0 );
    node* id = create_node( TOKENNODE , Parser->tokens [ Parser->pos - 1 ] , func );
    append_child( func , type_node );
    append_child( func , id );
    ++Parser->pos;
    while ( !is_type( CLOSEBR ) ) // eg i32 b( something ) instead of i32 b()
    {
        if ( is_type( COMMA ) )
            ++Parser->pos;
        node* next = parse_primary( Parser );
        append_child( func , next );
    }
    --Parser->pos;
    expect_err( CLOSEBR );
    ++Parser->pos;
    append_child( Parser->current_parent , func );
    Parser->current_parent = func;
    return parse( Parser );
}
node* parse_full_id( parser* Parser )
{
    node* arb_expr = create_arbitrary_node( EXPRESSION , Parser->current_parent );
    if ( conditional_token( Parser->tokens [ Parser->pos ] ) )
    {
        return parse_conditional( Parser );
    }
    if ( type_token( Parser->tokens [ Parser->pos ] ) )
    {
        expect_err( ID );
        // make sure its not a var decl
        if ( !expect_no_err( OPENBR ) )
        {
            --Parser->pos;
            node* expr = parse_primary( Parser );
            expr->parent = arb_expr;
            append_child( arb_expr , expr );
            append_child( Parser->current_parent , arb_expr );
            return parse( Parser );
        }
        return parse_func( Parser );
    }
    node* expr = parse_primary( Parser );
    expr->parent = arb_expr;
    append_child( arb_expr , expr );
    append_child( Parser->current_parent , arb_expr );
    return parse( Parser );
}
node* parse_expression( parser* Parser )
{
    node* arb_expr = create_arbitrary_node( EXPRESSION , Parser->current_parent );
    switch ( Parser->tokens [ Parser->pos ].type )
    {
        case ID:
            {
                return parse_full_id( Parser );
            }
        case NUMBER:
            {
                node* expr = parse_primary( Parser );
                expr->parent = arb_expr;
                append_child( arb_expr , expr );
                append_child( Parser->current_parent , arb_expr );
                return parse( Parser );
            }
        case SEMI:
            {
                ++Parser->pos;
                return parse( Parser );
            }
        case BEGINOFBLOCK:
            {
                ++Parser->pos;
                node* block = create_arbitrary_node( BLOCK , Parser->current_parent );
                append_child( Parser->current_parent , block );
                Parser->current_parent = block;
                return parse( Parser );
            }
        case ENDOFBLOCK:
            {
                ++Parser->pos;
                node* p = Parser->current_parent->parent;
                while ( p && p->type != PROGRAM && p->type != BLOCK )
                {
                    p = p->parent;
                }
                Parser->current_parent = p;
                return parse( Parser );
            }
        case COMMA:
            {
                ++Parser->pos;
                return parse( Parser );
            }
        case END_OF_FILE:
            {
                return Parser->root;
            }
        default:
            {
                char* msg = "Unexpected token '%s'.";
                char* buffer = malloc( 1024 );
                sprintf( buffer , msg , Parser->tokens [ Parser->pos ].text );
                put_error( buffer , 0 , Parser->tokens [ Parser->pos + 1 ] );

            }
    }
}

node* parse( parser* Parser )
{
    if ( Parser->pos > Parser->n_token )
    {
        return Parser->root;
    }
    return parse_expression( Parser );
}

void print_tree( node* n , int indent )
{
    print_n_times( "    " , indent );
    printf( "<NODE( '%s', %s )>\n" , n->node_token.text , node_type_to_string( n->type ) );
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