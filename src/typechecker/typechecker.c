#include<typechecker/typechecker.h>

bool is_node(node* t, node_type type)
{
	return t->type == type;
}
bool has_child_recursive(node* check, node_type type)
{
	if(check->n_child == 0)
	{
		return is_node(check, type);
	}
	for(size_t i = 0; i < check->n_child; ++i)
	{
		bool is = has_child_recursive(check->children[i], type);	
		if(is)
		{
			return is;
		}
		return has_child_recursive(check->children[i], type);
	}
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
                if ( !type_checker->functions )
                {
                    type_checker->functions = malloc( sizeof( function ) );
                    *type_checker->functions = ( function ) { current->children [ 0 ]->node_token.text, "unk", -1 };
                }
                else
                {
                    type_checker->functions = realloc( type_checker->functions , ( ++type_checker->n_func ) * sizeof( function ) );
                    type_checker->functions [ type_checker->n_func - 1 ] = ( function ) { current->children [ 0 ]->node_token.text, "unk", -1 };
                }
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
                if ( !type_checker->functions )
                {
                    type_checker->functions = malloc( sizeof( function ) );
                    *type_checker->functions = ( function ) { current->children [ 1 ]->node_token.text, current->children [ 0 ]->node_token.text,current->n_child - 3 };
                }
                else
                {
                    type_checker->functions = realloc( type_checker->functions , ( ++type_checker->n_func ) * sizeof( function ) );
                    type_checker->functions [ type_checker->n_func - 1 ] = ( function ) { current->children [ 1 ]->node_token.text, current->children [ 0 ]->node_token.text, current->n_child - 3 };
                }
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

                if ( current->n_child > type_checker->functions [ index ].n_args )
                {
                    char* buffer = malloc( 1024 );
                    memset( buffer , 0 , 1024 );
                    sprintf( buffer , "To many args for Function '%s'." , current->node_token.text );
                    put_error( buffer , 0 , current->node_token );
                    free( buffer );
                }
                if ( current->n_child < type_checker->functions [ index ].n_args )
                {
                    char* buffer = malloc( 1024 );
                    memset( buffer , 0 , 1024 );
                    sprintf( buffer , "To few args for Function '%s'." , current->node_token.text );
                    put_error( buffer , 0 , current->node_token );
                    free( buffer );
                }
            } break;
		case ASSIGNMENT:{
							printf("here ig\n");
						} break;
    }
    for ( size_t i = 0; i != current->n_child; ++i )
    {
        type_check_tree( current->children [ i ] , type_checker );
    }
    return true;
}
