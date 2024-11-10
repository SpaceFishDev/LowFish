#include "lexer.h"

char *mkstr(char *str)
{
	char *new = malloc(strlen(str) + 1);
	memset(new, 0, strlen(str) + 1);
	strcpy(new, str);
	return new;
}

bool is_digit(char c) { return c >= '0' && c <= '9'; }

token lex(lexer *Lexer)
{
	if (Lexer->pos > strlen(Lexer->src))
	{
		return create_token(Lexer->column, Lexer->line, mkstr("\0"),
							END_OF_FILE);
	}
	switch (Lexer->src[Lexer->pos])
	{
	case '\0':
	{
		return create_token(Lexer->column, Lexer->line, mkstr("\0"),
							END_OF_FILE);
	}
	break;
#pragma region SYMBOLS

	case '{':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("{"),
							BEGINOFBLOCK);
	}
	case '}':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("}"),
							ENDOFBLOCK);
	}
	case '(':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("("), OPENBR);
	}
	case ')':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr(")"),
							CLOSEBR);
	}

	case '&':
	{
		next;
		if (Lexer->src[Lexer->pos] >= 'a' &&
				Lexer->src[Lexer->pos] <= 'z' ||
			Lexer->src[Lexer->pos] >= 'A' &&
				Lexer->src[Lexer->pos] <= 'Z' ||
			Lexer->src[Lexer->pos] == '_')
		{
			size_t start = Lexer->pos;
			size_t column = Lexer->column;
			size_t line = Lexer->line;
			while (Lexer->src[Lexer->pos] >= 'A' &&
					   Lexer->src[Lexer->pos] <= 'Z' ||
				   Lexer->src[Lexer->pos] >= 'a' &&
					   Lexer->src[Lexer->pos] <= 'z' ||
				   Lexer->src[Lexer->pos] == '_' ||
				   is_digit(Lexer->src[Lexer->pos]))
			{
				next;
			}
			size_t len = Lexer->pos - start;
			char *result = malloc(len + 1);
			result[len] = 0;
			Lexer->pos = start;
			while (Lexer->src[Lexer->pos] >= 'A' &&
					   Lexer->src[Lexer->pos] <= 'Z' ||
				   Lexer->src[Lexer->pos] >= 'a' &&
					   Lexer->src[Lexer->pos] <= 'z' ||
				   Lexer->src[Lexer->pos] == '_' ||
				   is_digit(Lexer->src[Lexer->pos]))
			{
				result[Lexer->pos - start] = Lexer->src[Lexer->pos];
				next;
			}
			return create_token(column, line, result, REF);
		}
		if (Lexer->src[Lexer->pos] == '&')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr("&&"),
								BOOLAND);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("&"),
							LOGICALAND);
	}
	case '|':
	{
		next;
		if (Lexer->src[Lexer->pos] == '|')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr("||"),
								BOOLOR);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("|"),
							LOGICALOR);
	}
	case '=':
	{
		next;
		if (Lexer->src[Lexer->pos] == '=')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr("=="),
								BOOLEQUAL);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("="), EQUAL);
	}
	case '!':
	{
		next;
		if (Lexer->src[Lexer->pos] == '=')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr("!="),
								BOOLNOTEQUAL);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("!"),
							EXCLAMATION);
	}
	case ',':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr(","), COMMA);
	}
	case '[':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("["),
							INDEXBROPEN);
	}
	case ']':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("]"),
							INDEXBROPEN);
	}
	case ';':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr(";"), SEMI);
	}
	case '-':
	{
		next;
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
			char *res2 = malloc(len + 2);
			memset(res2, 0, len + 2);
			sprintf(res2, "-%s", result);
			free(result);
			return create_token(column, line, res2, NUMBER);
		}
		if (Lexer->src[Lexer->pos] == '>')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr("->"),
								ARROW);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("-"), MINUS);
	}
	case '+':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("+"), PLUS);
	}
	case '/':
	{
		next;
		return create_token(Lexer->column, Lexer->line, mkstr("/"), DIV);
	}
	case '*':
	{
		next;
		if (Lexer->src[Lexer->pos] >= 'a' &&
				Lexer->src[Lexer->pos] <= 'z' ||
			Lexer->src[Lexer->pos] >= 'A' &&
				Lexer->src[Lexer->pos] <= 'Z' ||
			Lexer->src[Lexer->pos] == '_')
		{
			size_t start = Lexer->pos;
			size_t column = Lexer->column;
			size_t line = Lexer->line;
			while (Lexer->src[Lexer->pos] >= 'A' &&
					   Lexer->src[Lexer->pos] <= 'Z' ||
				   Lexer->src[Lexer->pos] >= 'a' &&
					   Lexer->src[Lexer->pos] <= 'z' ||
				   Lexer->src[Lexer->pos] == '_' ||
				   is_digit(Lexer->src[Lexer->pos]))
			{
				next;
			}
			size_t len = Lexer->pos - start;
			char *result = malloc(len + 1);
			result[len] = 0;
			Lexer->pos = start;
			while (Lexer->src[Lexer->pos] >= 'A' &&
					   Lexer->src[Lexer->pos] <= 'Z' ||
				   Lexer->src[Lexer->pos] >= 'a' &&
					   Lexer->src[Lexer->pos] <= 'z' ||
				   Lexer->src[Lexer->pos] == '_' ||
				   is_digit(Lexer->src[Lexer->pos]))
			{
				result[Lexer->pos - start] = Lexer->src[Lexer->pos];
				next;
			}
			return create_token(column, line, result, DEREF);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("*"), MUL);
	}
	case '>':
	{
		next;
		if (Lexer->src[Lexer->pos] == '=')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr(">="),
								MOREEQUAL);
		}
		return create_token(Lexer->column, Lexer->line, mkstr(">"), MORE);
	}
	case '<':
	{
		next;
		if (Lexer->src[Lexer->pos] == '=')
		{
			next;
			return create_token(Lexer->column, Lexer->line, mkstr("<="),
								MOREEQUAL);
		}
		return create_token(Lexer->column, Lexer->line, mkstr("<"), MORE);
	}
#pragma endregion
#pragma region COMMENT
	case '#':
	{
		next;
		while (Lexer->src[Lexer->pos] != '\n' &&
			   Lexer->src[Lexer->pos] != '\r' &&
			   Lexer->src[Lexer->pos] != '#')
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
		char *val = malloc(len + 3);
		sprintf(val, "'%s'", value_of_string);
		next;
		return create_token(column, line, val, STRING);
	}
#pragma endregion
	}
#pragma region ID

	if ((Lexer->src[Lexer->pos] >= 'A' && Lexer->src[Lexer->pos] <= 'Z') ||
		(Lexer->src[Lexer->pos] >= 'a' && Lexer->src[Lexer->pos] <= 'z') ||
		(Lexer->src[Lexer->pos] == '_'))
	{
		size_t start = Lexer->pos;
		size_t column = Lexer->column;
		size_t line = Lexer->line;
		while (Lexer->src[Lexer->pos] >= 'A' && Lexer->src[Lexer->pos] <= 'Z' ||
			   Lexer->src[Lexer->pos] >= 'a' && Lexer->src[Lexer->pos] <= 'z' ||
			   Lexer->src[Lexer->pos] == '_' ||
			   is_digit(Lexer->src[Lexer->pos]))
		{
			next;
		}
		size_t len = Lexer->pos - start;
		char *result = malloc(len + 1);
		result[len] = 0;
		Lexer->pos = start;
		while (Lexer->src[Lexer->pos] >= 'A' && Lexer->src[Lexer->pos] <= 'Z' ||
			   Lexer->src[Lexer->pos] >= 'a' && Lexer->src[Lexer->pos] <= 'z' ||
			   Lexer->src[Lexer->pos] == '_' ||
			   is_digit(Lexer->src[Lexer->pos]))
		{
			result[Lexer->pos - start] = Lexer->src[Lexer->pos];
			next;
		}
		if (Lexer->src[Lexer->pos] == ':')
		{
			next;
			return create_token(column, line, result, LABEL);
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
	return create_token(Lexer->column, Lexer->line, mkstr("BAD"), BAD);
}
