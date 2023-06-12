#include<typechecker/typechecker.h>



bool type_check_tree( node* current )
{
    if ( !current )
    {
        return true;
    }
    switch ( current.type )
    {
        case FUNCTION_CALL:
            {

            }
    }
    for ( size_t i = 0; i != current->n_child; ++i )
    {
        type_check_tree( current->children [ i ] );
    }
    return true;
}