#include<Syntax/Lexer.h>

enum NodeTypes{
  PROGRAM,
  FUNCTION,
  EQUAL,
  FUNCTION_CALL,
  CONSTANT_NODE,
  REFERENCE,
  POINTERREFERENCE,
  VAR,
  POINTERVAR,
  BLOCK,
  BOOLEXPR,
  IFNODE,
  WHILENODE,
  STRUCT,
  MATH,
  UNLESSNODE,
  ELSENODE,
  MEMBER,
  NAME,
  ASSEMBLY,
  CONTAINERNODE,  
  EXTERN,
  RETURNNODE,
};
class Node
{
public:
  Token* NodeToken;
  std::vector<Node*> Children;
  int Type;
  Node* Parent; 
  Node(Token* t, int T, Node* Parent)
  {
    NodeToken = t;
    Type = T;
    this->Parent = Parent;
  }
private:
};

struct Type
{
  std::string name;
  std::vector<std::string> members;
  Type(std::string name)
  {
    this->name = name;
  }
};
struct Var
{
  std::string name;
  std::string TypeName;
  bool Pointer = false;
  Var(std::string n, std::string t, bool Pointer)
  {
    name = n;
    TypeName = t;
  }
};
struct Function
{
  int num_args;
  std::string Name;
  Function(std::string nm)
  {
	  num_args  =0;
	  Name = nm;
  }
};



class Parser
{
public:
  std::string Source;
  Lexer lexer;
  int Position = 0;
  std::vector<Token> Tokens;
  std::vector<Function> Functions;
  std::vector<Type> Types = {
    Type("string"), 
    Type("long"), 
    Type("char"), 
    Type("int"), 
    Type("short")
  };
  std::vector<Var> Variables;
  std::vector<Token> ReorderTokens(std::vector<Token> tokens)
  {
    return tokens;
  }
  Parser(){}
  Parser(std::string source)
  {
      lexer = Lexer(source);
      while(true)
      {
        Token t = lexer.Tokenize();
        Tokens.push_back(t);
        if(t.Type == END || lexer.Position > lexer.Source.length())
        {
          Tokens = ReorderTokens(Tokens);
          return;				
        }
      }
    }

    bool Expect(int Type)
    {
      if(Position + 1 > Tokens.size())
        ErrorHandler::PutError(-1, "Not enough tokens in input." , 0, 0);
      return Tokens[Position + 1].Type == Type;
    }
    bool ExpectValue(std::string Value)
    {
      if(Position + 1 > Tokens.size())
        ErrorHandler::PutError(-1, "Not enough tokens in input." , 0, 0);
      return Tokens[Position + 1].Text == Value;
    }
    bool CheckForMember(std::string member, std::string var)
    {
      bool correct = false;
      for(Var v : Variables)
      {
        if(v.name == var){
          for(Type t : Types)
          {
            if(t.name == v.TypeName)
            {
              for(std::string m : t.members)
              {
                if(m == member)
                {
                  correct = true;
                }
              }
              if(!correct)
              {
                ErrorHandler::PutError
                (
                  TYPE_HAS_NO_MEMBER, 
                  t.name, Tokens[Position].Line, 
                  Tokens[Position].Column, 
                  member
                );
              }		
            }
          }
        }
      }
      return correct;
    }
    struct Container
    {
      std::string name;
      Node** begin;
      Container(const std::string& name, Node** begin)
      {
        this->name = name;
        this->begin = begin;
      }
    };
    std::vector<Container> cn;
    void GetContainers(Node* n)
    {
      for(Node* c : n->Children)
      {
        if(c->Type == CONTAINERNODE)
        {
          cn.push_back(Container(c->Children[0]->NodeToken->Text, &c));
        }
        GetContainers(c);
      }
    }
    std::vector<Function> cntF;
    std::vector<std::string> Assembly;
    void GetFunctions(Node* container)
    {
      if(container->Type == FUNCTION)
      {
        Function n = Function(container->Children[0]->NodeToken->Text);
        cntF.push_back(n);
      }
      if(container->Type == ASSEMBLY && container->NodeToken->Type == STRING)
      {
        Assembly.push_back(container->NodeToken->Text);
      }
      for(Node* child : container->Children)
      {
        GetFunctions(child);
      }
    }
    Node* GetContainerContent(Node* N)
    {
      return N->Children[1];
    }
    Node* ParseConstant(Node* Parent, Node* Root)
    {
      std::string math = "/*+-";
          for(char c : math)
          {
            if(ExpectValue(std::string("") + c))
            {
              Node* N = new Node(&Tokens[Position + 1], MATH, Parent);
              N->Children.push_back(new Node(&Tokens[Position], CONSTANT_NODE, N));
              Position += 2;	
              Parent->Children.push_back(N);
              return Parse(N, Root);
            }
          }
          Node* N = new Node(&Tokens[Position], CONSTANT_NODE, Parent);
          ++Position;
          Parent->Children.push_back(N);
          return Parse(Parent, Root);
    }
    Node* ParseSymbol(Node* Parent, Node* Root)
    {
      Token Current = Tokens[Position];
      if(Current.Text == "&")
      {
        if(!Expect(IDENTIFIER))
          ErrorHandler::PutError(UNEXPECTED_CHARACTER, "&", Current.Line, Current.Column);
        ++Position;
        Node* N = new Node(&Tokens[Position], POINTERREFERENCE, Parent);
        Parent->Children.push_back(N);
        ++Position;
        return Parse(N, Root);
      }
      if(Current.Text == "*" && Parent->Type == VAR)
      {
        ++Position;
        Parent->Type = POINTERVAR;
        Variables[Variables.size() - 1].Pointer = true;
        return Parse(Parent, Root);
      }
      if(Current.Text == ",")
      {
        ++Position;
        return Parse(Parent, Root);
      }
      if(Current.Text == "{")
      {
        Node* N = new Node(&Tokens[Position], BLOCK, Parent);
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      if(Current.Text == "}")
      {
        ++Position;
        Node* N = Parent;
        if(N->Type == BLOCK && N->Parent->Type == IFNODE){
            N = N->Parent;
            while(N->Type != BLOCK && N->Type != PROGRAM){
              N = N->Parent;
            }	
            return Parse(N, Root);
        }
        while(N->Type != PROGRAM){
          N = N->Parent;
        }
        return Parse(N, Root);
      }
      if(
        Current.Text == ";" 
        || Current.Text == ")"
      )
      {
        ++Position;
        Node* N = Parent;
        if(N->Type == IFNODE || N->Type == NAME){
          return Parse(N, Root);
        }
        if(Parent->Type != BLOCK && Parent->Type != FUNCTION && Parent->Type != IFNODE){
          while(N->Type != FUNCTION && N->Type != PROGRAM && N->Type != BLOCK && N->Type != IFNODE){
            N = N->Parent;
          }
        }
        return Parse(N, Root);
      }
      if(
        Current.Text == "+" 
        || Current.Text == "-"
        || Current.Text == "/"
        || Current.Text == "*"
      )
      {
        Node* N = new Node(&Tokens[Position], MATH, Parent->Parent);
        N->Children.push_back(Parent);
        Parent->Parent->Children[Parent->Parent->Children.size() - 1] = N;
        ++Position;
        return Parse(N, Root);
      }
      if(
        Current.Text == "==" 
        || Current.Text == "!="
        || Current.Text == "<"
        || Current.Text == ">"
        || Current.Text == ">="
        || Current.Text == "<="
        || Current.Text == "&&"
        || Current.Text == "||"
      )
      {
        Node* N = new Node(&Tokens[Position], BOOLEXPR, Parent);
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      return Root;
    }
    Node* ParseIdentifier(Node* Parent, Node* Root, Token Current)
    {
      if(Current.Text == "extern")
      {
        if(Expect(STRING))
        {
          Node* N = new Node(&Tokens[Position], EXTERN, Parent);
          ++Position;
          Parent->Children.push_back(N);
          return Parse(N, Root);
        }
      }
      if(Current.Text == "asm")
      {
        if(Expect(STRING))
        {
          Node* N = new Node(&Tokens[Position + 1], ASSEMBLY, Parent);
          ++Position;
          ++Position;
          Parent->Children.push_back(N);
          Assembly.push_back(Tokens[Position + 1].Text  );
          return Parse(N, Root);
        }
      }
      if(Current.Text == "container")
      {
        if(!Expect(IDENTIFIER))
        {
          ErrorHandler::PutError(EXPECT_IDENTIFIER, " ", Current.Line, Current.Column);
        }
        Node* N = new Node(&Tokens[Position], CONTAINERNODE, Parent);
        Node* n = new Node(&Tokens[Position + 1], NAME, N);
        N->Children.push_back(n);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(n, Root);
      }
      if(Current.Text == "return")
      {
        if(!Expect(IDENTIFIER) && !Expect(CONSTANT) && !Expect(STRING))
        {
          ErrorHandler::PutError(EXPECT_IDENTIFIER, " ", Current.Line, Current.Column);
        }
        Node* N = new Node(&Tokens[Position], RETURNNODE, Parent);
        Node* n;
        if(Expect(IDENTIFIER))
          n = new Node(&Tokens[Position + 1], REFERENCE, N);
        else
          n = new Node(&Tokens[Position + 1], CONSTANT_NODE, N);
        N->Children.push_back(n);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(n, Root);
      }
      if(Current.Text == "include")
      {
        if(Expect(STRING))
        {
          std::string Path = Tokens[Position + 1].Text;
          Parser temp = Parser(ReadFile(Path));
          temp.Parse(Parent, Root);
          Position++;
          Position++;
          for(Function f : temp.Functions)
          {
            Functions.push_back(f);
          }
          for(Type t : temp.Types)
          {
            for(Type t2 : Types)
            {
              if(t2.name == t.name)
              {
                continue;
              }
            }
            Types.push_back(t);
          }
          return Parse(Parent,Root);
        }
        if(Expect(IDENTIFIER))
        {
          ++Position;
          std::string from = Tokens[Position].Text;
          if(ExpectValue("from"))
          {
            ++Position;
            std::string Path = Tokens[Position + 1].Text;
            Node* N = new Node(new Token(0,"",0,0), PROGRAM, Parent);
            Parser temp = Parser(ReadFile(Path));
            temp.Parse(N,N);
            cn = std::vector<Container>();
            GetContainers(N);
            Position += 2;
            for(Container cnt: cn)
            {
              if(cnt.name == from)
              {
                Parent->Children.push_back(N->Children[0]->Children[0]->Children[0]);
                cntF = std::vector<Function>();
                GetFunctions(N);
                for(Function f : cntF)
                {
                  Functions.push_back(f);
                }    
                return Parse(Parent, Root);
              }
            }
            ErrorHandler::PutError(CONTAINER, from, Current.Line, Current.Column);
          }
        }
      }
      if(Current.Text == "struct")
      {
        if(!Expect(IDENTIFIER)){
          ErrorHandler::PutError(EXPECT_IDENTIFIER, " ", Current.Line, Current.Column);
        }
        for(Type T : Types)
        {
          if(T.name == Tokens[Position + 1].Text)
          {
            ErrorHandler::PutError
            (
              REDEFINITION_OF_STRUCT, 
              Tokens[Position+1].Text, Tokens[Position].Line, 
              Tokens[Position].Column
            );
          }
        }
        Type t = Tokens[Position + 1].Text;
        Types.push_back(t);
        Node* N = new Node(&Tokens[Position + 1],STRUCT, Parent);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      if(Current.Text == "if")
      {
        if(!ExpectValue("("))
        {
          ErrorHandler::PutError(-1, "If statements require '(' expression ')' block. " , Current.Line, Current.Column);
        }
        Node* N = new Node(&Tokens[Position], IFNODE, Parent);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      if(Current.Text == "else")
      {
        Node* N = new Node(&Tokens[Position], ELSENODE, Parent);
        if(!ExpectValue("if"))
        {
          ++Position;
        }
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      if(Current.Text == "while")
      {
        if(!ExpectValue("("))
        {
          ErrorHandler::PutError(-1, "while statements require '(' expression ')' block. " , Current.Line, Current.Column);
        }
        Node* N = new Node(&Tokens[Position], WHILENODE, Parent);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      if(Current.Text == "unless")
      {
        if(!ExpectValue("("))
        {
          ErrorHandler::PutError(-1, "while statements require '(' expression ')' block. " , Current.Line, Current.Column);
        }
        Node* N = new Node(&Tokens[Position], UNLESSNODE, Parent);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      if(ExpectValue("."))
      {
        std::string var = Current.Text;
        ++Position;
        ++Position;
        std::string member = Tokens[Position].Text;
        bool correct = CheckForMember(member, var);
        Node* N = new Node(&Tokens[Position - 2], REFERENCE, Parent);
        Parent->Children.push_back(N);
        Node* n = new Node(&Tokens[Position - 1], MEMBER, N);
        N->Children.push_back(n);
        return Parse(n, Root);			
      }
      for(Type t : Types){
        if(Current.Text == t.name)
        {
          if(Expect(IDENTIFIER) && Tokens[Position + 2].Text == "(")
          {
            ++Position;
            int Type = FUNCTION;
            Node* F = new Node(&Tokens[Position - 1], Type, Parent);
            F->Children.push_back(new Node(&Tokens[Position], NAME, F));
            for(Function func : Functions)
            {
              if(func.Name == Tokens[Position].Text)
              {
                ErrorHandler::PutError(REDEFINITION_OF_FUNCTION, func.Name, Tokens[Position].Line, Tokens[Position].Column);
              }
            }
            Functions.push_back(Function(Tokens[Position].Text));
            ++Position;
            ++Position;
            Parent->Children.push_back(F);
            return Parse(F->Children[0], Root);
          }
          if(Parent->Parent->Type == STRUCT){
            Types[Types.size() - 1].members.push_back(Tokens[Position + 1].Text);
          }
          if(Parent->Type == FUNCTION){
            Functions[Functions.size() - 1].num_args++;
          }
          Node* E = new Node(&Tokens[Position], VAR, Parent);
          std::string type = Tokens[Position].Text ;
          Variables.push_back(Var(Tokens[++Position].Text,type, false));
          Parent->Children.push_back(E);
          return Parse(E, Root);
        }
      }
      if(ExpectValue(";") 
      || ExpectValue(")")
      || ExpectValue(",")
      || ExpectValue("+")
      || ExpectValue("-")
      || ExpectValue("/")
      || ExpectValue("*"))
      {
        if(Parent->Parent->Type == VAR)
        {
          Var v = Var(Tokens[Position].Text, Parent->NodeToken->Text, Parent->Type == POINTERVAR);
          Variables.push_back(v);
        }
        if(Parent->Type == EQUAL)
        {
          for(Var v : Variables)
          {
            if(Parent->NodeToken->Text == v.name) 
            {
              for(Var v2 : Variables)
              {
                if(v2.name == Tokens[Position].Text)
                {
                  if(v2.TypeName != v.TypeName)
                  {
                    ErrorHandler::PutError(TYPE_DOESNT_MATCH, v.name, Current.Line, Current.Column,v2.name);
                  }
                }
              }
            }
          }
        }
        Node* N = new Node(&Tokens[Position], REFERENCE, Parent);
        ++Position;
        Parent->Children.push_back(N);
        return Parse(Parent, Root);
      }
      
      if(
      ExpectValue("==") 
      || ExpectValue("!=")
      || ExpectValue("<")
      || ExpectValue(">")
      || ExpectValue(">=")
      || ExpectValue("<=")
      || ExpectValue("&&")
      )
      {
        Node* N = new Node(&Tokens[Position + 1], BOOLEXPR, Parent);
        N->Children.push_back(new Node(&Tokens[Position], REFERENCE, N));
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      
      if(ExpectValue("="))
      {
        Node* N = new Node(&Tokens[Position], EQUAL, Parent);
        ++Position;
        ++Position;
        Parent->Children.push_back(N);
        return Parse(N, Root);
      }
      int Type = 0;
      
      if(ExpectValue("("))
      {
        Type = FUNCTION_CALL;
        for(Function Func : Functions)
        {
          if(Func.Name == Tokens[Position].Text)
          {
            Node* F = new Node(&Tokens[Position], Type, Parent);
            ++Position;
            ++Position;
            Parent->Children.push_back(F);
            return Parse(F, Root);
          }
        }
        ErrorHandler::PutError(FUNCTION_NOT_EXIST, Tokens[Position].Text, Current.Line, Current.Column);
      }
      return Root;
    }
    Node* Parse(Node* Parent, Node* Root)
    {
      if(Position > Tokens.size())
      {
        return Root;
      }
      Token Current = Tokens[Position];
      switch(Current.Type){
        case STRING:
        case CONSTANT:
        {
          return ParseConstant(Parent, Root);
        }
        case SYMBOL:
        {
          return ParseSymbol(Parent, Root);
        } break;
        case IDENTIFIER:
        {
          return ParseIdentifier(Parent, Root, Current);
        } break;
      }
      return Root;
  }
private:
};
