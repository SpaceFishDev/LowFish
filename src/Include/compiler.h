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
        // if(Choice == "w")
        // {
          // CompileWindows(tree);
        // }
        // TextAsm = BssAsm + DataAsm + Externs + TextAsm; 
        // if(drawTree)
        // {
          // std::cout << TextAsm << "\n";
        // }
        return;
      }
    }
    std::cout << "Main function not defined. Entry point is required\n";
  }
};