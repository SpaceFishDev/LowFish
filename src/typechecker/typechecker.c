#include<typechecker/typechecker.h>

bool is_node( node* t , node_type type )
{
    return t->type == type;
}

void restructure_parents( node* tree )
{
    if ( tree->n_child == 0 )
    {
        return;
    }
    for ( size_t i = 0; i < tree->n_child; ++i )
    {
        tree->children [ i ]->parent = tree;
        restructure_parents( tree->children [ i ] );
    }
}

bool has_child_recursive( node* check , node_type type )
{
    if ( is_node( check , type ) )
    {
        return true;
    }
    for ( size_t i = 0; i < check->n_child; ++i )
    {
        bool is = has_child_recursive( check->children [ i ] , type );
        if ( is )
        {
            return is;
        }
    }
    return false;
}
// gets first instance...
node* get_child_recursive( node* check , node_type type )
{
    if ( is_node( check , type ) )
    {
        return check;
    }
    for ( size_t i = 0; i < check->n_child; ++i )
    {
        node* is = get_child_recursive( check->children [ i ] , type );
        if ( is )
        {
            return is;
        }
    }
    return 0;
}

node* get_parent( node* check , node_type type )
{
    node* p = check;
    while ( p )
    {
        if ( p->type == type )
        {
            return p;
        }
        p = p->parent;
    }
    return 0;
}

function* get_function( typechecker* type_checker , char* title )
{
    for ( size_t i = 0; i != type_checker->n_func; ++i )
    {
        if ( !strcmp( title , type_checker->functions [ i ].title ) )
        {
            return type_checker->functions + i;
        }
    }
    return 0;
}

variable* get_var( typechecker* type_checker , char* title , size_t scope , function* func )
{
    for ( size_t i = 0; i != type_checker->n_var; ++i )
    {
        if ( !strcmp( title , type_checker->variables [ i ].title ) && type_checker->variables [ i ].scope == scope && type_checker->variables [ i ].func->title == func->title )
        {
            return &type_checker->variables [ i ];
        }
    }
    return 0;
}


bool type_check_tree( node* current , typechecker* type_checker )
{
    if ( !current )
    {
        return true;
    }
    switch ( current->type )
    {
        case EXTERN:
            {
                // printf( "%s\n" , current->children [ 0 ]->node_token.text );
                for ( size_t i = 0; i < type_checker->n_func; ++i )
                {
                    if ( !strcmp( type_checker->functions [ i ].title , current->children [ 0 ]->node_token.text ) )
                    {
                        char* msg = ( char* ) malloc( 1024 );
                        memset( msg , 0 , 1024 );
                        strcat( msg , "Function '" );
                        strcat( msg , type_checker->functions [ i ].title );
                        strcat( msg , "' has been redefined." );
                        put_error( msg , 0 , current->children [ 1 ]->node_token );
                    }
                }
					printf(
							"WARNING: UNSAFE FUNCTION DEFINITION FOR '%s': The type checker is unable to check functions used from extern make sure that you match the argument count and type.\n", 
							current->children[0]->node_token.text
					);
                    type_checker->functions = realloc( type_checker->functions , ( ++type_checker->n_func ) * sizeof( function ) );
                    type_checker->functions [ type_checker->n_func - 1 ] = ( function ) { current->children [ 0 ]->node_token.text, "unk", -1 };
            } break;
        case FUNCTION:
            {
                for ( size_t i = 0; i < type_checker->n_func; ++i )
                {
                    if ( !strcmp( type_checker->functions [ i ].title , current->children [ 1 ]->node_token.text ) )
                    {
                        char* msg = ( char* ) malloc( 1024 );
                        memset( msg , 0 , 1024 );
                        strcat( msg , "Function '" );
                        strcat( msg , type_checker->functions [ i ].title );
                        strcat( msg , "' has been redefined." );
                        put_error( msg , 0 , current->children [ 1 ]->node_token );
                    }
                }
				type_checker->functions = realloc( type_checker->functions , ( ++type_checker->n_func ) * sizeof( function ) );
				type_checker->functions [ type_checker->n_func - 1 ] = ( function ) { current->children [ 1 ]->node_token.text, current->children [ 0 ]->node_token.text, current->n_child - 3 };
            } break;
        case FUNCTION_CALL:
            {
                bool found = false;
                size_t index = 0;
                for ( size_t i = 0; i < type_checker->n_func; ++i )
                {
                    if ( !strcmp( type_checker->functions [ i ].title , current->node_token.text ) )
                    {
                        found = true;
                        index = i;
                    }
                }
                if ( !found )
                {
                    char* buffer = malloc( 1024 );
                    memset( buffer , 0 , 1024 );
                    sprintf( buffer , "Function '%s' doesn't exist." , current->node_token.text );
                    put_error( buffer , 0 , current->node_token );
                    free( buffer );
                }

				if(type_checker->functions[index].n_args == -1)
				{
					goto end;
				}	
                if ( current->n_child > type_checker->functions [ index ].n_args )
                {
                    char* buffer = malloc( 1024 );
                    memset( buffer , 0 , 1024 );
                    sprintf( buffer , "To many args for Function '%s'." , current->node_token.text );
                    put_error( buffer , 0 , current->node_token );
                    free( buffer );
                } 
                if ( current->n_child <type_checker->functions [ index ].n_args )
                {
                    char* buffer = malloc( 1024 );
                    memset( buffer , 0 , 1024 );
                    sprintf( buffer , "To few args for Function '%s'." , current->node_token.text );
                    put_error( buffer , 0 , current->node_token );
                    free( buffer );
                }
            } break;
        case VARDECL:
            {
                node* type = get_child_recursive( current , TYPE );
                node* title = get_child_recursive( current , TOKENNODE );
                node* func = get_parent( current , FUNCTION )->children [ 1 ];
                if ( !func )
                {
                    printf( "HOW IS THIS POSSIBLE???\n" );
                    exit( 9001 ); // hehe over 9000
                }
                variable v = ( variable ) { title->node_token.text, type->node_token.text, type_checker->scope, get_function( type_checker,func->node_token.text ) };
                if ( !type_checker->variables )
                {
                    type_checker->variables = malloc( sizeof( variable ) );
                    *type_checker->variables = v;
                    type_checker->n_var = 1;
                }
                else
                {
                    type_checker->variables = realloc( type_checker->variables , sizeof( variable ) * ( type_checker->n_var + 1 ) );
                    type_checker->variables [ type_checker->n_var ] = v;
                    type_checker->n_var++;
                }
            } break;
        case ASSIGNMENT:
            {
                if ( has_child_recursive( current , FUNCTION_CALL ) )
                {
                    node* call = get_child_recursive( current , FUNCTION_CALL );
                    node* title = get_child_recursive( current , TOKENNODE );
                    node* node_func = get_parent( current , FUNCTION )->children [ 1 ];
                    function* func = get_function( type_checker , node_func->node_token.text );
                    variable* var = get_var( type_checker , title->node_token.text , type_checker->scope , func );
                    if ( func )
                    {
                        if ( var )
                        {
                            if ( strcmp( var->type , func->type ) )
                            {
                                put_error( "Incompatible types." , 0 , title->node_token );
                            }
                        }
                        else
                        {
                            printf( ":(\n" );
                        }
                    }
                    // if the function doesnt exist then later on when the type checker actually gets to the function call an error will be shown.
                }
                if ( has_child_recursive( current , BASICEXPRESSION ) )
                {
                    node* basic_expr = get_child_recursive( current , BASICEXPRESSION )->children [ 0 ];
                    if ( basic_expr->node_token.type == ID )
                    {
                        node* title = get_child_recursive( current , TOKENNODE );
                        node* node_func = get_parent( current , FUNCTION )->children [ 1 ];
                        function* func = get_function( type_checker , node_func->node_token.text );
                        variable* var = get_var( type_checker , title->node_token.text , type_checker->scope , func );
                        variable* var_get = get_var( type_checker , basic_expr->node_token.text , type_checker->scope , func );
                        if ( !var_get )
                        {
                            char* buffer = malloc( 1024 );
                            sprintf( buffer , "The variable you are trying to use '%s' does not exist." , basic_expr->node_token.text );
                            put_error( buffer , 0 , basic_expr->node_token );
                        }
                    }
                }
            } break;
    }
end:
    for ( size_t i = 0; i != current->n_child; ++i )
    {
        type_check_tree( current->children [ i ] , type_checker );
    }
    return true;
=
