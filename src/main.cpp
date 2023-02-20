#include<iostream>  
#include<cstring>
#include <fstream>
#include<vector> 
// #include<algorithm>
#include<sstream>
const char* ReadFile(std::string Path){
  std::vector<char> v;
  if (FILE *fp = fopen(Path.c_str(), "r"))
  {
    char buf[1024];
    while (size_t len = fread(buf, 1, sizeof(buf), fp))
      v.insert(v.end(), buf, buf + len);
    fclose(fp);
  }
  std::string r = "";
  for(char c : v){
    r += c;
  }
  return r.c_str();
}
std::string NodeTypeToString(int type);
class Node;
void printTree(Node* root, std::string prefix, bool isLastChild);
bool drawTree = false;

#include<compiler.h>

void printTree(Node* root, std::string prefix = "", bool isLastChild = true) {
  std::cout << prefix;
  if (isLastChild && root->Type != PROGRAM) {
  std::cout << "\\---->";
  prefix += "  ";
  }
  else if(!isLastChild){
  std::cout << "|---->";
  prefix += "| ";
  }
  std::cout << "Type: " << NodeTypeToString(root->Type) << ", Token: " << root->NodeToken->Text << std::endl;

  for (int i = 0; i < root->Children.size(); i++) {
  printTree(root->Children[i], prefix, i == root->Children.size() - 1);
  }
}

std::string NodeTypeToString(int type) {
  switch(type) {
  case RETURNNODE:
  return "RETURN";
  case PROGRAM:
  return "PROGRAM";
  case FUNCTION:
  return "FUNCTION";
  case EQUAL:
  return "EQUAL";
  case FUNCTION_CALL:
  return "FUNCTION_CALL";
  case CONSTANT_NODE:
  return "CONSTANT_NODE";
  case REFERENCE:
  return "REFERENCE";
  case BLOCK:
  return "BLOCK";
  case VAR:
  return "VAR";
  case BOOLEXPR:
  return "BOOLEXPR";
  case IFNODE:
  return "IFNODE";
  case WHILENODE:
  return "WHILENODE";
  case STRUCT:
  return "STRUCT";
  case MATH:
  return "MATH";
  case MEMBER:
  return "MEMBER";
  case UNLESSNODE:
  return "UNLESSNODE";
  case ELSENODE:
  return "ELSENODE";
  case NAME:
  return "NAME";
  case POINTERVAR:
  return "POINTER";
  case POINTERREFERENCE:
  return "PTRREFERENCE";
  case ASSEMBLY:
  return "ASSEMBLY";
  case CONTAINERNODE:
  return "CONTAINER";
  case EXTERN:
  return "EXTERN";
  default:
  return "UNKNOWN";
  }
}

std::string strip_extention(std::string str){
  int count = 0;
  int i = 0;
  while(i < str.length()){
    if(str[i] == '.'){
      ++count;
    }
    ++i;
  }
  i = 0;
  int copy = count;
  count = 0;
  int pos = 0;
  while(i < str.length()){
    if(str[i] == '.' && count != copy){
      ++count;
    }else if(count == copy){
      pos = i;
      break;
    }
    ++i;
  }
  std::string s = "";
  i = 0;
  while(i < pos - 1){
    s += str[i];
    ++i;
  }
  return s;
}
int main(int argc, char** argv){
  int i = 1;
  --argc;
  std::string out = "a.o";
  std::string in = "main.lf ";
  while(argc--){
    if(strcmp("-o", argv[i]) == 0){
      out = argv[i + 1];
      ++i;
      --argc;
    }
    else if(strcmp("-t", argv[i]) == 0){
      drawTree = !drawTree;
    }
    else{
      in = argv[i];
    }
    ++i;
  }
  std::string str = ReadFile(in);
  Compiler compiler(str, "w");
  Compiler::WriteFile(compiler.TextAsm, strip_extention(out) + ".asm");
  std::cout << "LOWFISH: Output to executable file '" << strip_extention(out) << ".exe" << "' into working directory.\n"; 
  system(
    (
      std::string("nasm ") + 
      std::string(strip_extention(out) + ".asm") + 
      " -f win32 -o" + std::string(strip_extention(out) 
      + ".obj")
    ).c_str()
  );
  system(
    (
      std::string("golink /entry:main /console msvcrt.dll ") + 
      std::string(strip_extention(out) + ".obj")
    ).c_str()
  );
}

