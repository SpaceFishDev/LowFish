#include<syntax/parser.h>
class Compiler{
public:
  static void WriteFile(std::string txt, std::string path){
    std::ofstream file;
    file.open(path.c_str());
    file << txt.c_str();
    file.close();
  }
  Node* tree;
  Parser parser;
  Compiler(std::string Source, std::string Choice){
    parser = Parser(Source);
    Node* root = new Node(new Token(0,"", 0, 0),PROGRAM, nullptr);
    tree = parser.Parse(root, root);
    if(drawTree)
      printTree(tree, "", true);
    for(Function n : parser.Functions)
    {
      if(n.Name == "main")
      {
        if(Choice == "w")
        {
          CompileWindows(tree);
        }
        TextAsm = BssAsm + DataAsm + Externs + TextAsm; 
        return;
      }
    }
    std::cout << "Main function not defined. Entry point is required\n";
  }
  std::string OutputAsm = "";

  std::string Externs = "";
  std::string DataAsm = "section .data\n";
  std::string BssAsm = "section .bss\n";
  std::string TextAsm = "section .text\n";
  bool inFunc = false;
  int StackPos = 0;
  int StringC = 0;
  struct Variable
  {
    int pos;
    std::string name;
    bool inFunc;
    Variable(int p, std::string n, bool i){
      pos = p;
      name = n;
      inFunc = i;
    }
  };
  std::vector<Variable> Variables;
  int AssemblyIndex = 0;
  void CompileWindows(Node* node)
  {
    if(node == nullptr)
      return;
    switch(node->Type)
    {
      case ASSEMBLY:
      {
        TextAsm += parser.Assembly[AssemblyIndex] + "\n";
        ++AssemblyIndex;
      } break;
      case EXTERN:
      {
        std::string func = node->Children[0]->NodeToken->Text;
        Externs += std::string("[extern ") + func  + "]\n";
      } break;
      case FUNCTION:
      {
        inFunc = true;
        std::string Name = node->Children[0]->NodeToken->Text;
        node->Children[0] = node->Children[0]->Children[0];
        node->Children[0]->Parent = node;
        TextAsm += Name;
        TextAsm += ":\n";
        TextAsm += "push ebp\nmov ebp, esp\n";
      } break;
      case CONSTANT_NODE:{
        if(node->Parent->Type == FUNCTION_CALL){
          if(node->NodeToken->Type == STRING){
            DataAsm += std::string("string") + std::to_string(StringC) + ":\n";
            DataAsm += std::string("\tdd `") + node->NodeToken->Text + "`,0\n";
            TextAsm += "push dword string";
            TextAsm += std::to_string(StringC) + "\n";
          }else{
            TextAsm += std::string("push dword ") + node->NodeToken->Text + "\n";
          }
        }
      }
      case REFERENCE:
      {
          if(node->Parent->Type == FUNCTION_CALL){
            for(Variable var : Variables){
              if(var.name == node->NodeToken->Text){
                if(!var.inFunc){
                  TextAsm += "\tpush ";
                  TextAsm += node->NodeToken->Text;
                  TextAsm += "\n";
                }else{
                  TextAsm += std::string("\tpush dword [ebp +  ") + std::to_string(( var.pos) + 8) + "]\n";
                }
              }
            }
          } break;
        case EQUAL:
        {
          if(node->Parent->Type == VAR){
            if(node->Children[0]->Type == CONSTANT_NODE && node->Children[0]->NodeToken->Type == STRING){
              if(inFunc){
                TextAsm += std::string("\tmov [ebp +") + std::to_string(StackPos + 8) + std::string("]") + ", dword string" + std::to_string(StringC) + "\n";
                Variables.push_back(Variable(StackPos,node->NodeToken->Text,true));
                DataAsm += std::string("string") + std::to_string(StringC) + std::string(":\n\tdd `") + node->Children[0]->NodeToken->Text + "`,0\n";
                ++StringC;
                StackPos += 4;
              }else{
                Variables.push_back(Variable(0,node->NodeToken->Text,false));
                DataAsm += node->NodeToken->Text + std::string(":\n\tdd `") + node->Children[0]->NodeToken->Text + "`,0\n";
              }
            }
          }
        } break;
        if(node->Parent->Type == VAR)
        {
          if(inFunc){
            std::string type = node->Parent->NodeToken->Text;
            std::string name = node->NodeToken->Text;
            TextAsm += "\tpush dword 0\n";
            Variables.push_back(Variable(StackPos, name, true));
            StackPos += 4;
         }
        }
      } break;
    }
    if(node->Type != PROGRAM && node->Parent->Type == FUNCTION_CALL && node->Parent->Children[node->Parent->Children.size() - 1] == node){
      TextAsm += std::string("\tcall ") + node->Parent->NodeToken->Text;
      TextAsm += "\n";
    }
    int i = 0;
    for(Node* child : node->Children)
    {
      if
      (
        node->Type == BLOCK 
        && node->Parent->Type == FUNCTION 
        && i == node->Children.size()
      )
      {
        StackPos = 0;
        inFunc = false;
        TextAsm += "mov esp, ebp\n";
        TextAsm += "pop ebp\nret\n";
      }
      ++i;
      CompileWindows(child);
    }
    if
    (
      node->Type == BLOCK 
      && node->Parent->Type == FUNCTION 
    )
    {
      inFunc = false;
      TextAsm += "mov esp, ebp\n";
      TextAsm += "pop ebp\nret\n";
    }
  }
};