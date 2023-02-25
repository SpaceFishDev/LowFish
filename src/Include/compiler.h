 #include<syntax/parser.h>
class Compiler{
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
    if(!nl){
      DataSection += str;
      return;
    }
    DataSection += str + "\n";
  }
  void Text(std::string str, bool nl = true)
  {
    if(!nl){
      TextSection += str;
      return;
    }
    TextSection += str + "\n";
  }
  void Bss(std::string str, bool nl = true)
  {
    if(!nl){
      BssSection += str;
      return;
    }
    BssSection += str + "\n";
  }
  std::string AsmOut()
  {
    return TextSection + BssSection + DataSection ;
  }
  Compiler(const std::string& Source, std::string Choice)
  {
    parser = Parser(std::move(Source));
    Node* root = new Node(new Token(0,"", 0, 0),PROGRAM, nullptr);
    tree = parser.Parse(root, root);
    if(drawTree)
      printTree(tree, "", true);
    for(Function n : parser.Functions)
    {
      if(n.Name == "main")
      {
        Compile16(tree);
        std::cout << AsmOut() << "\n";
        return;
      }
    }
    std::cout << "Main function not defined. Entry point is required\n";
  }
  int StackPosition = 0;
  std::string CurrentFunction = "GLOBAL";
  int AsmIndex = 0;
  struct Variable{
    int stack_pos = 0;
    int size = 0;
    std::string name;
    std::string function;
  };
  int StrIndex = 0;
  std::vector<Variable> Variables;
  Variable getVariable(std::string name)
  {
    for(Variable v : Variables)
    {
      if(v.name == name && v.function == CurrentFunction)
      {
        return v;
      }
    }
    for(Variable v : Variables)
    {
      if(v.name == name && v.function == "GLOBAL")
      {
        return v;
      }
    }
    return (Variable){0, 0, "", ""};
  }
  void Compile16(Node* root)
  {
    switch(root->Type)
    {
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

    }
    for(Node* child : root->Children){
      Compile16(child);
    }
  }
  void CompileAssembly16(Node* root)
  {
    Text(parser.Assembly[AsmIndex++]);
  }
  void CompileReference16(Node* root)
  { 
    if(root->Parent->Type == FUNCTION_CALL)
    {
      Variable v = getVariable(root->NodeToken->Text);
      if(v.name == ""){
        ErrorHandler::PutError(VAR_DOESNT_EXIST, root->NodeToken->Text, root->NodeToken->Line, root->NodeToken->Column);
      }
      
      Text(std::string("mov ") + std::string((v.size == 1) ? "al," : "ax,") + ((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " " "[bp + " + " " + std::to_string(v.stack_pos) + "]");
      Text("push eax");
      if(root->Parent->Children[root->Parent->Children.size() - 1] == root)
      {
        Text("call " + root->Parent->NodeToken->Text);
      }
    }
  }

  void CompileReturn16(Node* root)
  {
    if(root->Children[0]->Type == CONSTANT_NODE && root->Children[0]->NodeToken->Type == CONSTANT)
    {
      Text("mov ax, " + root->Children[0]->NodeToken->Text);
      Text("mov sp, bp");
      Text("pop bp");
      Text("ret");
    }
  }
  void CompileVar16(Node* root)
  {
    int sz = 0;
    if(root->NodeToken->Text == "int")
    {
      sz = 4;
    }
    if(root->NodeToken->Text == "char"){
      sz = 1;
    }
    if(root->NodeToken->Text == "short"){
      sz = 2;
    }
    if(root->NodeToken->Text == "string"){
      sz = 1;
    }
    Variable v = (Variable){StackPosition,sz, root->Children[0]->NodeToken->Text, CurrentFunction};
    Variables.push_back(v);
    StackPosition += sz;
  }
  void CompileEqual16(Node* root)
  {
    Variable v = getVariable(root->NodeToken->Text);
    if(v.name != root->NodeToken->Text)
    {
      ErrorHandler::PutError(VAR_DOESNT_EXIST, root->NodeToken->Text, root->NodeToken->Line, root->NodeToken->Column);
    }
    if(root->Children[0]->Type == CONSTANT_NODE && root->Children[0]->NodeToken->Type == CONSTANT)
      Text("mov [bp +" + std::to_string(v.stack_pos) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " " + root->Children[0]->NodeToken->Text);
    if(root->Children[0]->Type == CONSTANT_NODE && root->Children[0]->NodeToken->Type == STRING && root->Children[0]->NodeToken->Text.length() == 1)
      Text("mov [bp +" + std::to_string(v.stack_pos) + "], " +((std::string[5]){"", "byte", "word", "","dword"})[v.size] + " '" + root->Children[0]->NodeToken->Text + "'");
    if(root->Children[0]->Type == CONSTANT_NODE && root->Children[0]->NodeToken->Type == STRING && root->Children[0]->NodeToken->Text.length() != 1){
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
    if(root->Parent->Parent->Type == NAME || root->Parent->Parent->Type == FUNCTION)
    {
      CurrentFunction = "GLOBAL";
      if(root->Parent->Parent->NodeToken->Text != "main"){
        StackPosition = 0;
        Text("mov sp, bp");
        Text("pop bp");
        Text("ret");
      }
    }
  }

};

