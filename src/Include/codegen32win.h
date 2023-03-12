#include<syntax/parser.h>
#include<codegen16.h>

class CodegenWin32
{
  public:
    Node* tree;
    Parser parser;
    std::string DataSection = "";
    std::string TextSection = "";
    std::string BssSection = "";
    std::string CurrentFunction = "GLOBAL";
    struct Variable
    {
      std::string Type;
      std::string Name;
      Variable(std::string t, std::string n)
      {
        Type = t;
        Name = n;
      }
    };
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
    CodegenWin32(const std::string& Source)
    {
      Text("section .text");
      Bss("section .bss");
      Data("section .data");
      parser = Parser(std::move(Source));
      Node* root = new Node(new Token(0,"", 0, 0),PROGRAM, nullptr);
      tree = parser.Parse(root, root);
      if(drawTree)
        printTree(tree, "", true);
      Compile(tree);
      if(drawTree)
        std::cout << AsmOut() << "\n";
      return;
    }
    void Compile(Node* root)
    {
      switch(root->Type)
      {
        case FUNCTION:
        {
          CompileFunction(root);
        } break;
        case BLOCKEND:
        {
          CompileBlockEnd(root);
        } break;
      }
      for(Node* child : root->Children)
      {
        Compile(child);
      }
    }
    void CompileFunction(Node* root)
    {
      Text(root->Children[0]->NodeToken->Text + ":");
      Text("push ebp");
      Text("mov ebp, esp");
    }
    void CompileBlockEnd(Node* root)
    {
      if
      (
        root->Parent->Type == NAME
        || root->Parent->Parent->Type == FUNCTION
      )
      {
          CurrentFunction = "GLOBAL";
          if(root->Parent->Parent->NodeToken->Text != "main"){
            Text("mov esp, ebp");
            Text("pop ebp");
            Text("ret");
          }
      }
    }
  private:
};