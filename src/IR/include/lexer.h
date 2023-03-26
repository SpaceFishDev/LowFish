#include<stdio.h>
#include<stdlib.h>

enum TokenType
{
    CONSTANT,
    STRING,
    REGISTER,
    IDENTIFIER,
    NEWLINE,
    SYMBOL,
};

typedef struct 
{
    int type;
    char* text;
} token;

typedef struct
{
    char* src;
    int pos;     
} lexer;

token* initToken(int type, char* string)
{
    token* T = malloc(sizeof(token));
    *T = {type, string};
    return T;
}

lexer* initLexer(char* src)
{
    lexer* L = malloc(sizeof(lexer));
    *L = {src, 0};
    return L;
}
#define current Lexer->src[Lexer->pos]
token* lex(lexer* Lexer)
{
    if(current >= 'a' && current <= 'z' || current >= 'A' && current <= 'Z')
    {
        
    }
    switch(current)
    {
        case '\n':
        case ' ':
        {
            ++Lexer->pos;
            return lex(lexer);
        }
        case '"':
        {
            char* text;
            size_t length = 0;
            ++Lexer->pos;
            while(current != '"' && current)
            {
                ++length;
                ++Lexer->pos;
            }
            Lexer->pos -= length;
            char* string = malloc(length + 1);
            string[length] = 0;
            for(int i = 0; i != length; ++i)
            {
                string[i] = current;
                ++Lexer->pos;
            }
            return initToken(STRING, string);
        }
        case '%':
        {
            char* text;
            size_t length = 0;
            ++Lexer->pos;
            while(current != ' ' && current)
            {
                ++length;
                ++Lexer->pos;
            }
            Lexer->pos -= length;
            char* string = malloc(length + 1);
            string[length] = 0;
            for(int i = 0; i != length; ++i)
            {
                string[i] = current;
                ++Lexer->pos;
            }
            return initToken(REGISTER, string);
        }
    }
}
