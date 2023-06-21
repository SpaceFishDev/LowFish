#include <lexer/lexer.h>

bool is_digit(char c)
{
    return c >= '0' && c <= '9';
}

token lex(lexer *Lexer)
{
    if (Lexer->pos > strlen(Lexer->src))
    {
        return create_token(Lexer->column, Lexer->line, "\0", END_OF_FILE);
    }
    switch (Lexer->src[Lexer->pos])
    {
    case '\0':
    {
        return create_token(Lexer->column, Lexer->line, "\0", END_OF_FILE);
    }
    break;
#pragma region SYMBOLS

    case '{':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "{", BEGINOFBLOCK);
    }
    case '}':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "}", ENDOFBLOCK);
    }
    case '(':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "(", OPENBR);
    }
    case ')':
    {
        next;
        return create_token(Lexer->column, Lexer->line, ")", CLOSEBR);
    }

    case '&':
    {
        next;
        if (Lexer->src[Lexer->pos] == '&')
        {
            next;
            return create_token(Lexer->column, Lexer->line, "&&", BOOLAND);
        }
        return create_token(Lexer->column, Lexer->line, "&", LOGICALAND);
    }
    case '|':
    {
        next;
        if (Lexer->src[Lexer->pos] == '|')
        {
            next;
            return create_token(Lexer->column, Lexer->line, "||", BOOLOR);
        }
        return create_token(Lexer->column, Lexer->line, "|", LOGICALOR);
    }
    case '=':
    {
        next;
        if (Lexer->src[Lexer->pos] == '=')
        {
            next;
            return create_token(Lexer->column, Lexer->line, "==", BOOLEQUAL);
        }
        return create_token(Lexer->column, Lexer->line, "=", EQUAL);
    }
    case '!':
    {
        next;
        if (Lexer->src[Lexer->pos] == '=')
        {
            next;
            return create_token(Lexer->column, Lexer->line, "!=", BOOLNOTEQUAL);
        }
        return create_token(Lexer->column, Lexer->line, "!", EXCLAMATION);
    }
    case ',':
    {
        next;
        return create_token(Lexer->column, Lexer->line, ",", COMMA);
    }
    case '[':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "[", INDEXBROPEN);
    }
    case ']':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "[", INDEXBROPEN);
    }
    case ';':
    {
        next;
        return create_token(Lexer->column, Lexer->line, ";", SEMI);
    }
    case '-':
    {
        next;
        if (Lexer->src[Lexer->pos] == '>')
        {
            next;
            return create_token(Lexer->column, Lexer->line, "->", ARROW);
        }
        return create_token(Lexer->column, Lexer->line, "-", MINUS);
    }
    case '+':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "+", PLUS);
    }
    case '/':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "/", DIV);
    }
    case '*':
    {
        next;
        return create_token(Lexer->column, Lexer->line, "*", MUL);
    }
    case '>':
    {
        next;
        if (Lexer->src[Lexer->pos] == '=')
        {
            next;
            return create_token(Lexer->column, Lexer->line, ">=", MOREEQUAL);
        }
        return create_token(Lexer->column, Lexer->line, ">", MORE);
    }
    case '<':
    {
        next;
        if (Lexer->src[Lexer->pos] == '=')
        {
            next;
            return create_token(Lexer->column, Lexer->line, "<=", MOREEQUAL);
        }
        return create_token(Lexer->column, Lexer->line, "<", MORE);
    }
#pragma endregion
#pragma region COMMENT
    case '#':
    {
        next;
        while (Lexer->src[Lexer->pos] != '\n' && Lexer->src[Lexer->pos] != '\r' && Lexer->src[Lexer->pos] != '#')
        {
            next;
        }
        return lex(Lexer);
    }
#pragma endregion
#pragma region WS
    case '\n':
    {
        size_t c = Lexer->column;
        size_t l = Lexer->line;
        ++Lexer->line;
        ++Lexer->pos;
        Lexer->column = 0;
        return lex(Lexer);
    }
    case ' ':
    case '\t':
    case '\r':
    {
        next;
        return lex(Lexer);
    }
#pragma endregion
#pragma region STRING
    case '"':
    {
        next;
        size_t start = Lexer->pos;
        size_t column = Lexer->column;
        size_t line = Lexer->line;
        char *value_of_string;
        while (Lexer->src[Lexer->pos] != '"')
        {
            next;
        }
        size_t len = Lexer->pos - start;

        value_of_string = malloc(len + 1);
        memset(value_of_string, 0, len + 1);
        Lexer->pos = start;
        while (Lexer->src[Lexer->pos] != '"')
        {
            value_of_string[Lexer->pos - start] = Lexer->src[Lexer->pos];
            next;
        }
        next;
        return create_token(column, line, value_of_string, STRING);
    }
#pragma endregion
    }
#pragma region ID

    if (
        (Lexer->src[Lexer->pos] >= 'A' && Lexer->src[Lexer->pos] <= 'Z') || (Lexer->src[Lexer->pos] >= 'a' && Lexer->src[Lexer->pos] <= 'z') || (Lexer->src[Lexer->pos] == '_'))
    {
        size_t start = Lexer->pos;
        size_t column = Lexer->column;
        size_t line = Lexer->line;
        while (
            Lexer->src[Lexer->pos] >= 'A' && Lexer->src[Lexer->pos] <= 'Z' || Lexer->src[Lexer->pos] >= 'a' && Lexer->src[Lexer->pos] <= 'z' || Lexer->src[Lexer->pos] == '_' || is_digit(Lexer->src[Lexer->pos]))
        {
            next;
        }
        size_t len = Lexer->pos - start;
        char *result = malloc(len + 1);
        result[len] = 0;
        Lexer->pos = start;
        while (
            Lexer->src[Lexer->pos] >= 'A' && Lexer->src[Lexer->pos] <= 'Z' || Lexer->src[Lexer->pos] >= 'a' && Lexer->src[Lexer->pos] <= 'z' || Lexer->src[Lexer->pos] == '_' || is_digit(Lexer->src[Lexer->pos]))
        {
            result[Lexer->pos - start] = Lexer->src[Lexer->pos];
            next;
        }
        return create_token(column, line, result, ID);
    }
#pragma endregion
#pragma region NUMBER
    if (is_digit(Lexer->src[Lexer->pos]))
    {
        char *result;
        size_t start = Lexer->pos;
        size_t column = Lexer->column;
        size_t line = Lexer->line;

        while (is_digit(Lexer->src[Lexer->pos]))
        {
            next;
        }
        if (Lexer->src[Lexer->pos] == '.')
        {
            next;
        }
        while (is_digit(Lexer->src[Lexer->pos]))
        {
            next;
        }
        size_t len = Lexer->pos - start;
        result = malloc(len);
        result[len] = 0;
        Lexer->pos = start;
        while (is_digit(Lexer->src[Lexer->pos]))
        {
            result[Lexer->pos - start] = Lexer->src[Lexer->pos];
            next;
        }
        if (Lexer->src[Lexer->pos] == '.')
        {
            result[Lexer->pos - start] = '.';
            next;
        }
        while (is_digit(Lexer->src[Lexer->pos]))
        {
            result[Lexer->pos - start] = Lexer->src[Lexer->pos];
            next;
        }
        return create_token(column, line, result, NUMBER);
    }
#pragma endregion
    return create_token(Lexer->column, Lexer->line, "BAD", BAD);
}