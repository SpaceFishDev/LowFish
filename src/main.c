/***
 *     .::::::. ::::::::::.   :::.       .,-:::::  .,::::::  .-:::::'::: .::::::.   ::   .:  :::::::-.  .,::::::  :::      .::.
 *    ;;;`    `  `;;;```.;;;  ;;`;;    ,;;;'````'  ;;;;''''  ;;;'''' ;;;;;;`    `  ,;;   ;;,  ;;,   `';,;;;;''''  ';;,   ,;;;'
 *    '[==/[[[[,  `]]nnn]]'  ,[[ '[[,  [[[          [[cccc   [[[,,== [[['[==/[[[[,,[[[,,,[[[  `[[     [[ [[cccc    \[[  .[[/
 *      '''    $   $$$""    c$$$cc$$$c $$$          $$""""   `$$$"`` $$$  '''    $"$$$"""$$$   $$,    $$ $$""""     Y$c.$$"
 *     88b    dP   888o      888   888,`88bo,__,o,  888oo,__  888    888 88b    dP 888   "88o  888_,o8P' 888oo,__    Y88P
 *      "YMmMY"    YMMMb     YMM   ""`   "YUMMMMMP" """"YUMMM "MM,   MMM  "YMmMY"  MMM    YMM  MMMMP"`   """"YUMMM    MP
 */

#include<lexer/lexer.h>
#include<util.h>
#include<parser/parser.h>

int main( void )
{
    char* input_file = read_file( "test.lf" );
    printf( "Hello, world!\n \n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n%s\n" , input_file );
    lexer Lexer = ( lexer ) { input_file, 0,0,0 };
    token* Tokens = malloc( sizeof( token ) );
    size_t n_token = 1;
    for ( ;;)
    {
        token T = lex( &Lexer );
        printf( "TOKEN(%s,%s)\n" , T.text , token_type_to_string( T.type ) );
        Tokens = realloc( Tokens , n_token * sizeof( token ) );
        Tokens [ n_token - 1 ] = T;
        ++n_token;
        if ( T.type == BAD || T.type == END_OF_FILE )
        {
            break;
        }
    }
    parser* Parser = create_parser( Tokens , n_token - 1 );
    node* root = parse( Parser );
    print_tree( root , 0 );
    return 0;
}