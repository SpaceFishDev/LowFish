#include<syntax/parser.h>
class Codegen16{
public:
  static void WriteFile(std::string txt, std::string path)
  {
    std::ofstream file;
    file.open(path.c_str());
    file << txt.c_str();
    file.close(); 
  }
  Node* tree;
  Parser parser;
  std::string DataSection = "";
  std::string TextSection = "";
  std::string BssSection = "";
  std::string output = "";

  void Data(std::string str, bool nl = true)
  {
    if(!nl)
    {
      DataSection += str;
      return;
    }
    DataSection += str + "\n";
  }
  void Text(std::string str, bool nl = true)
  {
    if(!nl)
    {
      TextSection += str;
      return;
    }
    TextSection += str + "\n";
  }
  void Bss(std::string str, bool nl = true)
  {
    if(!nl)
    {
      BssSection += str;
      return;
    }
    BssSection += str + "\n";
  }
  std::string AsmOut()
  {
    return TextSection + BssSection + DataSection ;
  }
  Codegen16(const std::string& Source, std::string Choice)
  {
    parser = Parser(std::move(Source));
    Node* root = new Node(new Token(0,"", 0, 0),PROGRAM, nullptr);
    tree = parser.Parse(root, root);
    if(drawTree)
      printTree(tree, "", true);
    Compile16(tree);
    if(drawTree)
      std::cout << AsmOut() << "\n";
    return;
  }
  int StackPosition = 0;
  int LoopIndex = 0;
  std::string CurrentFunction = "GLOBAL";
  int AsmIndex = 0;
  struct Variable{
    int stack_pos = 0;
    int size = 0;
    std::string name;
    std::string function;
    bool pointer;
  };
  int StrIndex = 0;
  std::vector<Variable> Variables;
  Variable getVariable(std::string name)
  {
    for(Variable v : Variables)
    {
      if
      (
        v.name == name 
        && v.function == CurrentFunction
      )
      {
        return v;
      }
    }
    for(Variable v : Variables)
    {
      if
      (
        v.name == name 
        && v.function == "GLOBAL"
      )
      {
        return v;
      }
    }
    return (Variable){0, 0, "", "", false};
  }
  void Compile16(Node* root)
  {
    // std::cout << NodeTypeToString(root->Type) << "\n";
    switch(root->Type)
    {
      case POINTERVAR:
      {
        CompilePointerVar16(root);
      }break;
      case FUNCTION:
      {
        CompileFunction16(root);
      } break;
      case BLOCKEND:
      {
        CompileBlockEnd16(root);
      } break;
      case VAR:
      {
        CompileVar16(root);
      } break;
      case EQUAL:
      {
        CompileEqual16(root);
      } break;
      case RETURNNODE:
      {
        CompileReturn16(root);  
      } break;
      case ASSEMBLY:
      {
        CompileAssembly16(root);
      } break;
      case REFERENCE:
      {
        CompileReference16(root);
      } break;
      case FUNCTION_CALL:
      {
        CompileFunctionCall16(root);
      } break;
      case BOOLEXPR:
      {
        CompileBoolExpr16(root);
      } break;
      case WHILENODE:
      {
        CompileWhile16(root);
      } break;
      case IFNODE:
      {
        CompileIf16(root);
      } break;
      case MATH:
      {
        CompileMath16(root);
      } break;
      case UNLESSNODE:
      {
        CompileUnless16(root);
      } break;
      case INDEX:
      {
        CompileIndex16(root);
      } break;
    }
    for(Node* child : root->Children){
      Compile16(child);
    }
  }
  void CompileIndex16(Node* root)
  {
    if(root->Parent->Type == REFERENCE)
    {
      Variable v = getVariable(root->Parent->NodeToken->Text);

      int ptr = std::stoi(root->Children[0]->NodeToken->Text) + v.stack_pos;

      Node* n = root->Children[0];
      int i = 0;
      while
      (
        i < root->Children.size() 
        && n->Type != INDEXEND
      )
      {
        n = root->Children[i];
        ++i;
      }
    }
    if(root->Parent->Type == VAR && CurrentFunction != "GLOBAL")
    {
      if(root->Children[0]->NodeToken->Type == CONSTANT)
      {
        StackPosition += std::stoi(root->Children[0]->NodeToken->Text);
      }

    }
  }
  void CompileMath16(Node* root)
  {
    if(root->Parent->Type != MATH)
    {
      std::string c1 = "dx,";
      std::string c2 = "cx,";
      if(root->Children[0]->NodeToken->Type == CONSTANT)
      {
        Text("mov dl, " + root->Children[0]->NodeToken->Text);
        c2 = "dl, ";
      }
      if(root->Children[0]->Type == REFERENCE)
      {
        Variable v = getVariable(root->Children[0]->NodeToken->Text);
        Text
        (
          "mov " 
          + std::string((v.size == 1) ? "dl, " : "dx, ") 
          + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] 
          + "[bp + " + std::to_string(v.stack_pos) 
          + "]"
        );
        if(v.size == 1)
          c1 = "dl, ";
      }
      if(root->Children[1]->Type == REFERENCE)
      {
        Variable v = getVariable(root->Children[1]->NodeToken->Text);
        Text
        (
          "mov " + (v.size == 1) ? "cl, " : "cx, " 
          + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] 
          + "[bp + " 
          + std::to_string(v.stack_pos) 
          + "]"
        );
        if(v.size == 1)
          c2 = "cl";    
      }
      if
      (
        root->Children[1]->Type == CONSTANT_NODE 
        && root->Children[1]->NodeToken->Type == CONSTANT
      )
      {
        Text("mov cl, " + root->Children[1]->NodeToken->Text);
        c2 = "cl";
      }
      if(c1 == "dl, ")
      {
        c2 = "cl";
      }
      if(c2 == "cl")
      {
        c1 = "dl, ";
      }
      switch(root->NodeToken->Text[0])
      {
        case '+':
        {
          Text("add " + c1 + c2);
        } break;
        case '-':
        {
          Text("sub " + c1 + c2);
        } break;
      }
      if(root->Parent->Type == EQUAL)
      {
        Variable v = getVariable(root->Parent->NodeToken->Text);
        if(c1 == "dl, ")
        {
          c1 = "dl";
        }
        if(c1 == "dx, ")
        {
          c1 = "dx";
        }
        Text("mov [ebp + " + std::to_string(v.stack_pos) + "], " + c1);
      }
    }
  }
  void CompileUnless16(Node* root)
  {
    Text("LO" + std::to_string(LoopIndex) + ":");
    root->ID = LoopIndex++;
  }
  void CompileWhile16(Node* root)
  {
    Text("LO" + std::to_string(LoopIndex) + ":");
    root->ID = LoopIndex++;
  }
  void CompileIf16(Node* root)
  {
    root->ID = LoopIndex++;
  }
  void CompileBoolExpr16(Node* root)
  {
    if(root->Parent->Type == UNLESSNODE)
    {
      if(root->NodeToken->Text == "==")
      {
        std::string c1 = "dx,";
        std::string c2 = "cx";
        if(root->Children[0]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[0]->NodeToken->Text);
          if(v.name == "")
          {
            ErrorHandler::PutError
            (
              VAR_DOESNT_EXIST, 
              root->Children[0]->NodeToken->Text, 
              root->Children[0]->NodeToken->Line, 
              root->Children[0]->NodeToken->Column
            );
          }
          if(v.size == 1 )
            c1 = "dl,";
          Text("mov " + c1 + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if(root->Children[1]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[1]->NodeToken->Text);
          if(v.size == 1)
            c2 = "cl";
          if(v.name == "")
          {
            ErrorHandler::PutError
            (
              VAR_DOESNT_EXIST,
              root->Children[0]->NodeToken->Text,
              root->Children[0]->NodeToken->Line,
              root->Children[0]->NodeToken->Column
            );
          }
          Text("mov " + c2 + "," + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if
        (
          root->Children[1]->Type == CONSTANT_NODE 
          && root->Children[1]->NodeToken->Type == CONSTANT
        )
        {
          Text(std::string("mov ") + "cl" + ", "  + "byte " + root->Children[1]->NodeToken->Text);
          c2 = "cl";
        }
        
        Text("cmp "+ c1 + c2);
        Text("jne LOEND" + std::to_string(root->Parent->ID));
      } 
      if(root->NodeToken->Text == "!=")
      {
        std::string c1 = "dx,";
        std::string c2 = "cx";
        if(root->Children[0]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[0]->NodeToken->Text);
          if(v.name == "")
          {
            ErrorHandler::PutError
            (
              VAR_DOESNT_EXIST, 
              root->Children[0]->NodeToken->Text, 
              root->Children[0]->NodeToken->Line, 
              root->Children[0]->NodeToken->Column
            );
          }
          if(v.size == 1 )
            c1 = "dl,";
          Text("mov " + c1 + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if(root->Children[1]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[1]->NodeToken->Text);
          if(v.size == 1)
            c2 = "cl";
          if(v.name == "")
          {
            ErrorHandler::PutError
            (
              VAR_DOESNT_EXIST, 
              root->Children[0]->NodeToken->Text, 
              root->Children[0]->NodeToken->Line, 
              root->Children[0]->NodeToken->Column
            );
          }
          Text("mov " + c2 + "," + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if
        (
          root->Children[1]->Type == CONSTANT_NODE 
          && root->Children[1]->NodeToken->Type == CONSTANT
        )
        {
          Text(std::string("mov ") + "cl" + ", "  + "byte " + root->Children[1]->NodeToken->Text);
          c2 = "cl";
        }
        
        Text("cmp "+ c1 + c2);
        Text("je LOEND" + std::to_string(root->Parent->ID));
      }
    }
    if
    (
      root->Parent->Type == IFNODE 
      || root->Parent->Type == WHILENODE
    )
    {
      if(root->NodeToken->Text == "==")
      {
        std::string c1 = "dx,";
        std::string c2 = "cx";
        if(root->Children[0]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[0]->NodeToken->Text);
          if(v.name == "")
          {
            ErrorHandler::PutError(VAR_DOESNT_EXIST, root->Children[0]->NodeToken->Text, root->Children[0]->NodeToken->Line, root->Children[0]->NodeToken->Column);
          }
          if(v.size == 1 )
            c1 = "dl,";
          Text("mov " + c1 + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if(root->Children[1]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[1]->NodeToken->Text);
          if(v.size == 1)
            c2 = "cl";
          if(v.name == "")
          {
            ErrorHandler::PutError(VAR_DOESNT_EXIST, root->Children[0]->NodeToken->Text, root->Children[0]->NodeToken->Line, root->Children[0]->NodeToken->Column);
          }
          Text("mov " + c2 + "," + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if
        (
          root->Children[1]->Type == CONSTANT_NODE 
          && root->Children[1]->NodeToken->Type == CONSTANT
        )
        {
          Text(std::string("mov ") + "cl" + ", "  + "byte " + root->Children[1]->NodeToken->Text);
          c2 = "cl";
        }
        
        Text("cmp "+ c1 + c2);
        Text("jne LOEND" + std::to_string(root->Parent->ID));
      } 
      if(root->NodeToken->Text == "!=")
      {
        std::string c1 = "dx,";
        std::string c2 = "cx";
        if(root->Children[0]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[0]->NodeToken->Text);
          if(v.name == "")
          {
            ErrorHandler::PutError(VAR_DOESNT_EXIST, root->Children[0]->NodeToken->Text, root->Children[0]->NodeToken->Line, root->Children[0]->NodeToken->Column);
          }
          if(v.size == 1 )
            c1 = "dl,";
          Text("mov " + c1 + 
          ((std::string[5])
          {"",
            "byte",
            "word",
            "",
            "dword"
          })
          [v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if(root->Children[1]->Type == REFERENCE)
        {
          Variable v = getVariable(root->Children[1]->NodeToken->Text);
          if(v.size == 1)
            c2 = "cl";
          if(v.name == "")
          {
            ErrorHandler::PutError(VAR_DOESNT_EXIST, root->Children[0]->NodeToken->Text, root->Children[0]->NodeToken->Line, root->Children[0]->NodeToken->Column);
          }
          Text("mov " + c2 + "," + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " [bp + " + std::to_string(v.stack_pos) + "]");
        }
        if
        (
          root->Children[1]->Type == CONSTANT_NODE 
          && root->Children[1]->NodeToken->Type == CONSTANT
        )
        {
          Text(std::string("mov ") + "cl" + ", "  + "byte " + root->Children[1]->NodeToken->Text);
          c2 = "cl";
        }
        
        Text("cmp "+ c1 + c2);
        Text("je LOEND" + std::to_string(root->Parent->ID));
      }     
    }
  }
  void CompileAssembly16(Node* root)
  {
    Text(parser.Assembly[AsmIndex++]);
  }
  void CompileFunctionCall16(Node* root)
  {
    if(root->Children.size() == 0)
    {
      Text("call " + root->NodeToken->Text);
      if(root->Parent->Type == EQUAL)
      {
        std::string name = root->Parent->NodeToken->Text;
        Variable v = getVariable(name);
        if(v.name == "")
        {
          ErrorHandler::PutError(VAR_DOESNT_EXIST, name, root->Parent->NodeToken->Line,root->Parent->NodeToken->Column);
        }
        Text("mov [bp +" + std::to_string(v.stack_pos) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + std::string((v.size == 1) ? " al" : " ax"));
      }
    }
  }
  void CompileReference16(Node* root)
  { 
    if(root->Parent->Type == VAR)
    {
      return;
    }
    if(root->Parent->Type == FUNCTION_CALL)
    {
      Variable v = getVariable(root->NodeToken->Text);
      if(v.name == "")
      {
        ErrorHandler::PutError(VAR_DOESNT_EXIST, root->NodeToken->Text, root->NodeToken->Line, root->NodeToken->Column);
      }
      
      Text
      (
        std::string("mov ") + 
        std::string(
        (v.size == 1) ? "al," : "ax,") +
        ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " " + 
        "[bp + " + " " + std::to_string(v.stack_pos) + "]"
      );
      Text("push byte eax");
      if(root->Parent->Children[root->Parent->Children.size() - 1] == root)
      {
        Text("call " + root->Parent->NodeToken->Text);
        if(root->Parent->Parent->Type == EQUAL)
        {
          std::string name = root->Parent->Parent->NodeToken->Text;
          Variable v = getVariable(name);
          if(v.name == "")
          {
            ErrorHandler::PutError(VAR_DOESNT_EXIST, name, root->Parent->Parent->NodeToken->Line,root->Parent->Parent->NodeToken->Column);
          }
          Text("mov [bp +" + std::to_string(v.stack_pos) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " eax");
        }
      }
    }
    if(root->Children.size() == 0)
    if(root->Children[0]->Type == INDEX)
      return;
    
  }

  void CompileReturn16(Node* root)
  {
    if
    (
      root->Children[0]->Type == CONSTANT_NODE 
      && root->Children[0]->NodeToken->Type == CONSTANT
    )
    {
      Text("mov ax, " + root->Children[0]->NodeToken->Text);
      Text("mov sp, bp");
      Text("pop bp");
      Text("ret");
    }
  }
  void CompilePointerVar16(Node* root)
  {
    int sz = 1;
    if
    (
      root->Children.size() > 0 
      && root->Children[0]->Type == INDEX
    )
    {
      Node* n = root->Children[0]->Children[0];
      int i = 0;
      while
      (
        i < root->Children[0]->Children.size() 
        && n->Type != INDEXEND
      )
      {
        n = root->Children[0]->Children[i];
        ++i;
      }
      Variable v = (Variable){StackPosition, sz, n->Children[0]->NodeToken->Text, CurrentFunction};
      Variables.push_back(v);
      StackPosition += sz;
    }
    Variable v = (Variable)
    {
      StackPosition,
      sz, 
      root->Children[0]->NodeToken->Text, 
      CurrentFunction, 
      true
    };
    Variables.push_back(v);
    
    StackPosition += sz;
  }
  void CompileVar16(Node* root)
  {
    int sz = 0;
    if(root->NodeToken->Text == "int")
    {
      sz = 4;
    }
    if(root->NodeToken->Text == "char")
    {
      sz = 1;
    }
    if(root->NodeToken->Text == "short")
    {
      sz = 2;
    }
    if(root->NodeToken->Text == "string")
    {
      sz = 1;
    }
    if
    (
      root->Children.size() > 0 
      && root->Children[0]->Type == INDEX
    )
    {
      Node* n = root->Children[0]->Children[0];
      int i = 0;
      while
      (
        i < root->Children[0]->Children.size() 
        && n->Type != INDEXEND
      )
      {
        n = root->Children[0]->Children[i];
        ++i;
      }
      Variable v = (Variable){StackPosition, sz, n->Children[0]->NodeToken->Text, CurrentFunction, false};
      Variables.push_back(v);
      StackPosition += sz;
    }
    Variable v = (Variable){StackPosition,sz, root->Children[0]->NodeToken->Text, CurrentFunction, false};
    Variables.push_back(v);
    
    StackPosition += sz;
  }
  void CompileEqual16(Node* root)
  {
    Variable v = getVariable(root->NodeToken->Text);
    
    if(root->Children[0]->Type == REFERENCE)
    {
      if(root->Children[0]->Children.size() != 0)
      {
        if
        (
          root->Children[0]->Children[0]->Type == INDEX 
          && root->Children[0]->Children[0]->Children[0]->NodeToken->Type == CONSTANT
        )
        {
          Variable v2 = getVariable(root->Children[0]->NodeToken->Text);
          Text("mov bx, bp");
          Text("add bx, "+ std::to_string(v2.stack_pos + 0));
          Text("mov [bp + " + std::to_string(v.stack_pos + std::stoi(root->Children[0]->Children[0]->Children[0]->NodeToken->Text) + 0) + "], bx");
        }
      }
    }
    if(root->Parent->Type == INDEXEND)
    {
      Node* n = root;
      while
      (
        n->Parent && n->Type != REFERENCE 
        && n->Type != POINTERREFERENCE
      )
      {
        n = n->Parent;
      }
      v = getVariable(n->NodeToken->Text);
      int ptr = v.stack_pos + std::stoi(root->Parent->Parent->Children[0]->NodeToken->Text) + 0;
      if(v.size == 0) v.size = 1;
      if(v.pointer)
      {
        Text("mov bx, bp");
        Text("add bx, " + std::to_string(ptr - v.stack_pos));
        if
        (
          root->Children[0]->Type == CONSTANT_NODE 
          && root->Children[0]->NodeToken->Type == CONSTANT
        )
          Text("mov [bx], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " " + root->Children[0]->NodeToken->Text);
        if
        (
          root->Children[0]->Type == CONSTANT_NODE 
          && root->Children[0]->NodeToken->Type == STRING 
          && root->Children[0]->NodeToken->Text.length() == 1
        )
          Text("mov [bx], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " '" + root->Children[0]->NodeToken->Text + "'");
        if
        (
          root->Children[0]->Type == CONSTANT_NODE 
          && root->Children[0]->NodeToken->Type == STRING 
          && root->Children[0]->NodeToken->Text.length() != 1
        )
        {
          Data(std::string("string") + std::to_string(StrIndex++) + ":");
          Data("db `" + root->Children[0]->NodeToken->Text + "`,0");
          Text("mov [bx], " + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " string" + std::to_string(StrIndex - 1));
        }
        return;
      }
      if
      (
        root->Children[0]->Type == CONSTANT_NODE 
        && root->Children[0]->NodeToken->Type == CONSTANT
      )
        Text("mov [bp +" + std::to_string(ptr) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " " + root->Children[0]->NodeToken->Text);
      if
      (
        root->Children[0]->Type == CONSTANT_NODE 
        && root->Children[0]->NodeToken->Type == STRING 
        && root->Children[0]->NodeToken->Text.length() == 1
      )
        Text("mov [bp +" + std::to_string(ptr) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " '" + root->Children[0]->NodeToken->Text + "'");
      if
      (
        root->Children[0]->Type == CONSTANT_NODE 
        && root->Children[0]->NodeToken->Type == STRING 
        && root->Children[0]->NodeToken->Text.length() != 1
      )
      {
        Data(std::string("string") + std::to_string(StrIndex++) + ":");
        Data("db `" + root->Children[0]->NodeToken->Text + "`,0");
        Text("mov [bp + " + std::to_string(ptr) + "], " + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " string" + std::to_string(StrIndex - 1));
      }
      return;
    }
    if(v.name != root->NodeToken->Text)
    {
      ErrorHandler::PutError(VAR_DOESNT_EXIST, root->NodeToken->Text, root->NodeToken->Line, root->NodeToken->Column);
    }
    if(v.pointer == true)
    {
      if(root->Children[0]->Type == REFERENCE)
      {
        Variable v2 = getVariable(root->Children[0]->NodeToken->Text);
        Text("mov bx, bp");
        Text("add bx, "+ std::to_string(v2.stack_pos + 0));
        Text("mov [bp + " + std::to_string(v.stack_pos + 0) + "], [bx]");
      }
    }
    if
    (
      root->Children[0]->Type == CONSTANT_NODE 
      && root->Children[0]->NodeToken->Type == CONSTANT
    )
      Text("mov [bp +" + std::to_string(v.stack_pos) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " " + root->Children[0]->NodeToken->Text);
    if
    (
      root->Children[0]->Type == CONSTANT_NODE 
      && root->Children[0]->NodeToken->Type == STRING 
      && root->Children[0]->NodeToken->Text.length() == 1
    )
      Text("mov [bp +" + std::to_string(v.stack_pos) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " '" + root->Children[0]->NodeToken->Text + "'");
    if
    (
      root->Children[0]->Type == CONSTANT_NODE 
      && root->Children[0]->NodeToken->Type == STRING 
      && root->Children[0]->NodeToken->Text.length() != 1
    )
    {
      Data(std::string("string") + std::to_string(StrIndex++) + ":");
      Data("db `" + root->Children[0]->NodeToken->Text + "`,0");
      Text("mov [bp + " + std::to_string(v.stack_pos) + "], " + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " string" + std::to_string(StrIndex - 1));
    }
  }
  void CompileFunction16(Node* root)
  {
    Text(root->Children[0]->NodeToken->Text, false);
    CurrentFunction = root->Children[0]->NodeToken->Text;
    Text(":");
    Text("push bp");
    Text("mov bp, sp");
    StackPosition += 4;
  }
  void CompileBlockEnd16(Node* root)
  {
    if
    (
      root->Parent->Parent->Type == NAME 
      || root->Parent->Parent->Type == FUNCTION
    )
    {
      if(root->Parent->Parent->Parent->Type == BLOCK)
      {
        CurrentFunction = "GLOBAL";
        if(root->Parent->Parent->NodeToken->Text != "main"){
          StackPosition -= 4;
          Text("mov sp, bp");
          Text("pop bp");
          Text("ret");
        }
      }
      CurrentFunction = "GLOBAL";
      if(root->Parent->Parent->NodeToken->Text != "main")
      {
        StackPosition = 0;
        Text("mov sp, bp");
        Text("pop bp");
        Text("ret");
      }
    }
    if(root->Parent->Parent->Type == WHILENODE)
    {
      Text("jmp LO" + std::to_string(root->Parent->Parent->ID));
      Text("LOEND" + std::to_string(root->Parent->Parent->ID) + ":");
    }
    if(root->Parent->Parent->Type == IFNODE)
    {
      Text("LOEND" + std::to_string(root->Parent->Parent->ID) + ":");
    }
  }
};

