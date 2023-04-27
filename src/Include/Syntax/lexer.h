#pragma once

#include <iostream>

struct Token
{
  int Type;
  std::string Text;
  int Line;
  int Column;
  Token(int t, std::string txt, int l, int c)
  {
    Type = t;
    Text = txt;
    Line = l;
    Column = c;
  }
  Token()
  {
  }
};

enum ErrorTypes
{
  NEVER_ENDING_STRING,
  UNEXPECTED_CHARACTER,
  REDEFINITION_OF_FUNCTION,
  REDEFINITION_OF_STRUCT,
  EXPECT_IDENTIFIER,
  TYPE_HAS_NO_MEMBER,
  TYPENAME_DOESNT_EXIST,
  CONTAINER,
  FUNCTION_NOT_EXIST,
  TYPE_DOESNT_MATCH,
  VAR_DOESNT_EXIST,
  REDEFINITION_OF_VARIABLE,
  MISSING_SEMI,
};

class ErrorHandler
{
public:
  static void PutError(int type, std::string c, int line, int column, std::string d = "")
  {
    switch (type)
    {
    case MISSING_SEMI:
    {
      std::cout << "Missing semicolon ';' LN:" << line << " COL: " << column << "\n";
      exit(-1); 
    }
    case REDEFINITION_OF_VARIABLE:
    {
      std::cout << "Variable '" << c << "' redefined LN: " << line << " COL: " << column << "\n";
      exit(-1);
    }
    case VAR_DOESNT_EXIST:
    {
      std::cout << "Variable " << c << " does not exist in current context LN: " << line << " COL: " << column << "\n";
      exit(-1);
    }
    case NEVER_ENDING_STRING:
    {
      std::cout << "String never ends. LN: " << line << " COL: " << column << "\n";
      exit(-1);
    }
    case UNEXPECTED_CHARACTER:
    {
      std::cout << "Unexpected character '" << c << "' in input. LN: " << line << " COL: " << column << "\n";
      exit(-1);
    }
    case REDEFINITION_OF_FUNCTION:
    {
      std::cout << "Function '" << c << "' already defined. LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case REDEFINITION_OF_STRUCT:
    {
      std::cout << "Type '" << c << "' already defined. LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case EXPECT_IDENTIFIER:
    {
      std::cout << "Identifier expected for struct definition. LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case TYPE_HAS_NO_MEMBER:
    {
      std::cout << "Type '" << c << "' does not include the member '" << d << "'. LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case TYPENAME_DOESNT_EXIST:
    {
      std::cout << "Type '" << c << "' does not exist. LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case CONTAINER:
    {
      std::cout << "Container '" << c << "' does not exist LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case FUNCTION_NOT_EXIST:
    {
      std::cout << "Function '" << c << "' does not exist LN:" << line << " COL:" << column << "\n";
      exit(-1);
    }
    case TYPE_DOESNT_MATCH:
    {
      std::cout << "Assigning variable '" << c << "' to variable '" << d << "' but their types do not match. LN:" << line << " COL:" << column << "\n";
      // exit(-1);
    }
    break;
    defualt:
    {
      std::cout << c << "' in input. LN: " << line << " COL: " << column << "\n";
      exit(-1);
    }
    }
    return;
  }
};

enum LexerTypes
{
  END,
  CONSTANT,
  STRING,
  IDENTIFIER,
  SYMBOL,
};

class Lexer
{
public:
  std::string Source;
  int Position;
  int Line;
  int Column;
  Lexer(std::string src, int l)
  {
    src[l] = 0;
    for (char c : src)
    {
      if (c == 0)
      {
        break;
      }
      Source += c;
    }
    Position = 0;
    Line = 1;
    Column = 0;
  }
  Lexer() {}
  Token LastToken;
  Token Tokenize()
  {
    if(Source[Position] == '\n')
    {
      std::string eol_possible[] = {"else", "if", "while", "unless", "from", "import", ")" , "{" , "}", "(", ";"};
      bool found = false;
      for(int i = 0; i != 11; ++i)
      {
        if(LastToken.Text == eol_possible[i])
        {
          found = true;
        }
      }
      if(!found ){
        ErrorHandler::PutError(MISSING_SEMI,"", Line, Column, "");
      }
    } 
    if (Source[Position] == '#')
    {
      while (Source[Position] != '\n' && Source[Position] != '\0')
      {
        ++Position;
      }
      ++Position;
      return Tokenize();
    }
    switch (Source[Position])
    {
      
    case '[':
    case ']':
    {
      std::string o = " ";
      o[0] = Source[Position];
      ++Position;
      LastToken = Token(SYMBOL, o, Line, Column);
      return Token(SYMBOL, o, Line, Column);
    }
    case '+':
    case '-':
    case '/':
    case '*':
    case '(':
    case ')':
    case '{':
    case ',':
    case '.':
    case '}':
    {
      if (!(Source[Position] == '-' && IsDigit(Source[Position + 1])))
      {
        std::string o = " ";
        o[0] = Source[Position];
        ++Position;
        LastToken = Token(SYMBOL, o, Line, Column);
        return Token(SYMBOL, o, Line, Column);
      }

    }
    break;
    case '\'':
    case '"':
    {
      char c = Source[Position];
      std::string out = std::string(1024 * 16, ' ');
      ++Position;
      ++Column;
      int i = 0;
      while (Position < Source.length() && Source[Position] != c)
      {
        if (Source[Position] == '\n')
        {
          ErrorHandler::PutError(NEVER_ENDING_STRING, "", Line, Column);
        }
        out[i] = Source[Position];
        ++Position;
        ++Column;
        ++i;
      }
      out.erase(i, 1024 * 16 - i);
      Position++;
      Column++;
      LastToken = Token(STRING, std::move(out), Line, Column);
      return Token(STRING, std::move(out), Line, Column);
    }
    case '\n':
    {
      ++Line;
      ++Position;
      Column = 0;
      return Tokenize();
    }
    case ':':
    case '\\':
    case '\t':
    case ' ':
    {
      ++Position;
      ++Column;
      return Tokenize();
    }
    case ';':
    {
      ++Position;
      ++Column;
      LastToken = Token(SYMBOL, ";", Line, Column);
      return Token(SYMBOL, ";", Line, Column);
    }
    case '<':
    case '>':
    {
      char c = Source[Position];
      ++Position;
      ++Column;
      if (Source[Position] == '=')
      {
        Position++;
        Column++;
        LastToken = Token(SYMBOL, std::string(std::string("") + c) + "=", Line, Column);
        return Token(SYMBOL, std::string(std::string("") + c) + "=", Line, Column);
      }
      LastToken = Token(SYMBOL, std::string("") + c, Line, Column);
      return Token(SYMBOL, std::string("") + c, Line, Column);
    }
    case '!':
    {
      ++Position;
      ++Column;
      if (Source[Position] == '=')
      {
        Position++;
        Column++;
        LastToken = Token(SYMBOL, "!=", Line, Column);
        return Token(SYMBOL, "!=", Line, Column);
      }
      ErrorHandler::PutError(-1, "'!' has to be followed by a '=' for bolean expressions.", Line, Column);
    }
    break;
    case '|':
    {
      ++Position;
      ++Column;
      if (Source[Position] == '|')
      {
        Position++;
        Column++;
        LastToken = Token(SYMBOL, "||", Line, Column);
        return Token(SYMBOL, "||", Line, Column);
      }
    }
    case '&':
    {
      ++Position;
      ++Column;
      if (Source[Position] == '&')
      {
        Position++;
        Column++;
        LastToken = Token(SYMBOL, "&&", Line, Column);
        return Token(SYMBOL, "&&", Line, Column);
      }
      LastToken = Token(SYMBOL, "&", Line, Column);
      return Token(SYMBOL, "&", Line, Column);
    }
    case '=':
    {
      ++Position;
      ++Column;
      if (Source[Position] == '=')
      {
        Position++;
        Column++;
        LastToken = Token(SYMBOL, "==", Line, Column);
        return Token(SYMBOL, "==", Line, Column);
      }
      return Token(SYMBOL, "=", Line, Column);
    }
    case '\0':
    {
      return Token(END, "", Line, Column);
    }
    }
    if (IsLetter(Source[Position]))
    {
      std::string out = "";
      while (IsLetter(Source[Position]) || Source[Position] == '_' || IsDigit(Source[Position]))
      {
        out += Source[Position];
        ++Position;
        ++Column;
      }
      LastToken = Token(IDENTIFIER, out, Line, Column);
      return Token(IDENTIFIER, out, Line, Column);
    }
    if (IsDigit(Source[Position]) || Source[Position] == '-')
    {
      std::string out = "";
      out += Source[Position];
      ++Position;
      while (IsDigit(Source[Position]))
      {
        out += Source[Position];
        ++Position;
        ++Column;
      }
      LastToken = Token(CONSTANT, out, Line, Column);
      return Token(CONSTANT, out, Line, Column);
    }
    if (Position >= Source.length())
    {
      return Token(END, "", Line, Column);
    }
    ErrorHandler::PutError(UNEXPECTED_CHARACTER, std::string("") + Source[Position], Line, Column);
    return Token(END, "", Line, Column);
  }

private:
  bool IsDigit(char c)
  {
    return c >= '0' && c <= '9';
  }
  bool IsLetter(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }
};
