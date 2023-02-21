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
  Compiler(const std::string& Source, std::string Choice){
    parser = Parser(std::move(Source));
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
        if(drawTree){
          std::cout << TextAsm << "\n";
        }
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
    std::string type;
    Variable(int p, std::string n, bool i, std::string t){
      pos = p;
      name = n;
      inFunc = i;
      type = t;
    }
  };
  std::vector<Variable> Variables;
  int AssemblyIndex = 0;
  int LoopIndex = 0;

  void CompileWindows(Node* node)
  {
    if(node == nullptr)
      return;
    switch(node->Type)
    {
      case FUNCTION_CALL:
      {
        if(node->Children.size() == 0)
        {
          TextAsm += "call " + node->NodeToken->Text + "\n";
        }
      } break;
      case IFNODE:{
        Node* n = node->Children[0];
        Node* prev = node->Children[0];
        while(true){
          prev = n;
          if(n->Type == BOOLEXPR && (n->NodeToken->Text == "==" || n->NodeToken->Text == "!="))
          {
            if(n->Children[0]->Type == REFERENCE && n->Children[1]->Type == REFERENCE)
            {
              bool end = false;
              for(Variable v1 : Variables)
              {
                if(v1.name == n->Children[0]->NodeToken->Text)
                {
                  for(Variable v2 : Variables)
                  {
                    if(v2.name == n->Children[1]->NodeToken->Text)
                    {
                      if(v2.type == "enumurable" && v1.type == "enumurable"){
                        // adam hates this massive nesting.
                        // but I dont care that much.
                        if(v1.inFunc && v2.inFunc){
                          TextAsm += "push eax\n";
                          TextAsm += "mov eax, [ ebp + " + std::to_string(v1.pos) + "]\n" ;
                          TextAsm += "mov edx, [ ebp + " + std::to_string(v2.pos) + "]\n" ;
                          TextAsm += "cmp edx, eax\n";
                          if(n->NodeToken->Text == "==")
                            TextAsm += "jne LO" + std::to_string(LoopIndex) + "\n";
                          else
                            TextAsm += "je LO" + std::to_string(LoopIndex) + "\n";
                        }
                        end = true;
                      }
                    }
                  }
                }
              }
              if(end){
                n = n->Children[1];
                continue;
              }
            }
          } 
          if(prev = n){
            break;
          }
        }
      } break;
      case ASSEMBLY:
      {
        TextAsm += parser.Assembly[AssemblyIndex] + "\n";
        ++AssemblyIndex;
      } break;
      case EXTERN:
      {
        std::string func = node->Children[0]->NodeToken->Text;
        if(func == "printf"){
          DataAsm += "strPrnt:\ndd \"%d\",0\n";
        }
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
            ++StringC;
          }else{
            TextAsm += std::string("push dword ") + node->NodeToken->Text + "\n";
          }
        }
      }
      case REFERENCE:
      {
          if(node->Parent->Type == FUNCTION_CALL)
          {
            for(Variable var : Variables)
            {
              if(var.name == node->NodeToken->Text)
              {
                if(!var.inFunc)
                {
                  TextAsm += "\tpush ";
                  TextAsm += node->NodeToken->Text;
                  TextAsm += "\n";
                }else
                {
                  TextAsm += std::string("\tpush dword [ebp +  ") + std::to_string(( var.pos) + 8) + "]\n";
                }
              }
            }
          } 
          if(node->Parent->Type == VAR && node->Parent->Parent->Type != FUNCTION)
          {
            if(inFunc){
              std::string type = node->Parent->NodeToken->Text;
              if(type != "string"){
                type = "enumurable";
              }
              std::string name = node->NodeToken->Text;
              TextAsm += "\tpush dword 0\n";
              Variables.push_back(Variable(StackPos, name, true, type));
              StackPos += 4;
            }
          }
      }break;
        case EQUAL:
        { 
          if(node->Parent->Type == VAR)
          {
            if(node->Children[0]->Type == CONSTANT_NODE && node->Children[0]->NodeToken->Type == STRING)
            {
              if(inFunc)
              {
                TextAsm += std::string("\tmov [ebp +") + std::to_string(StackPos + 8) + std::string("]") + ", dword string" + std::to_string(StringC) + "\n";
                Variables.push_back(Variable(StackPos,node->NodeToken->Text,true, "string"));
                DataAsm += std::string("string") + std::to_string(StringC) + std::string(":\n\tdd `") + node->Children[0]->NodeToken->Text + "`,0\n";
                ++StringC;
                StackPos += 4;
              }else
              {
                DataAsm += node->NodeToken->Text + std::string(":\n\tdd `") + node->Children[0]->NodeToken->Text + "`,0\n";
                Variables.push_back(Variable(0,node->NodeToken->Text,false, "string"));
              }
            }else if(node->Children[0]->NodeToken->Type == CONSTANT)
            {
              if(inFunc)
              {
                TextAsm += "mov [ebp + " + std::to_string(StackPos) + "], dword " + node->Children[0]->NodeToken->Text + "\n";
                Variables.push_back(Variable(StackPos, node->NodeToken->Text, true, "enumurable"));
                StackPos += 4;
              }
            }
          }
        } break;
    }
    
    if
    (
      node->Type != PROGRAM 
      && node->Parent->Type == FUNCTION_CALL 
      && node->Parent->Children[node->Parent->Children.size() - 1] == node
    )
    {
      TextAsm += std::string("\tcall ") + node->Parent->NodeToken->Text;
      TextAsm += "\n";
      if(node->Parent->Parent->Type == EQUAL)
      {
        if(node->Parent->Parent->Parent->Type == VAR)
        {
          TextAsm += "\tpush dword eax\n";
          Variables.push_back(Variable(StackPos,node->Parent->Parent->NodeToken->Text, true, node->Parent->Parent->Parent->NodeToken->Text));
          StackPos += 4;
        }
      }
    }
    if(node->Type == BLOCKEND)
    {
      if(node->Parent->Parent->Type == IFNODE)
      {
        TextAsm += "LO" + std::to_string(LoopIndex) + ":\n";
      }
    }

    int i = 0;
    skip:
    for(Node* child : node->Children)
    {
      if
      (
        node->Type == BLOCKEND
        && node->Parent->Parent->Type == FUNCTION 
        && i == node->Children.size()
      )
      {
        StackPos = 0;
        inFunc = false;
        TextAsm += "mov esp, ebp\n";
        TextAsm += "pop ebp\nret\n";
      }
      if
      (
        node->Type != PROGRAM
        && node == node->Parent->Children[node->Parent->Children.size() - 1]
        && node->Parent->Type != FUNCTION_CALL
        && node->Type != FUNCTION_CALL
      )
      {
        bool isInBlock = false;
        Node* n = node;
        while(n->Type != PROGRAM)
        {
          if(n->Type == BLOCK)
          {
            isInBlock = true;
            break;
          }
          n = n->Parent;
        }
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