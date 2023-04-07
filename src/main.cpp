// Copyright (C) 2023 Liam Gatter
// See license.md in root directory for license details.
#include<iostream>  
#include<cstring>
#include <fstream>
#include<vector> 
#define VERSION 0.4
#include<sstream>

char* ReadFile(std::string file) {
    const char* filename = file.c_str();
    FILE *file_ptr;
    char ch;
    int size = 0;
    char *buffer = NULL;

    file_ptr = fopen(filename, "r");

    if (file_ptr == NULL) {
        printf("Unable to open file.\n");
        return NULL;
    }

    // Calculate the size of the file
    fseek(file_ptr, 0L, SEEK_END);
    size = ftell(file_ptr);
    rewind(file_ptr);

    // Allocate memory for the buffer
    buffer = (char*)malloc(size + 1);

    // Read the file into the buffer
    int i = 0;
    while ((ch = fgetc(file_ptr)) != EOF) {
        buffer[i] = ch;
        i++;
    }
    buffer[i] = '\0'; // Add null-terminator at the end

    fclose(file_ptr);
    return buffer;
}
std::string NodeTypeToString(int type);
class Node;
void printTree(Node* root, std::string prefix, bool isLastChild);
bool drawTree = false;

#include<IR.h>  


void printTree(Node* root, std::string prefix = "", bool isLastChild = true) {
  std::cout << prefix;
  if (isLastChild && root->Type != PROGRAM) {
  std::cout << "\\---->";
  prefix += "     ";
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
    case INDEX:
    return "INDEX";
    case INDEXEND:
    return "INDEXEND";
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
  std::string out = "a.exe";
  std::string in = "main.lf ";
  std::string ld_args = "";
  bool bin = false;
  while(argc--){
    if(strcmp("-o", argv[i]) == 0){
      out = argv[i + 1];
      ++i;
      --argc;
    }
    else if(strcmp("-t", argv[i]) == 0 || strcmp("--tree", argv[i]) == 0){
      drawTree = !drawTree;
    }
    else if(strcmp("-v", argv[i]) == 0){
      std::cout << "LowFish: V" << VERSION << "\n";
      return 0;
    }
    else if(strcmp("-h", argv[i]) == 0){
      std::cout << "Idk man, learn them .\n";
      return 0;
    }
    else if(strcmp("-b", argv[i]) == 0){
      bin = true;
    }
    else if(argv[i][0] == '$')
    {
      argv[i][0] = ' ';
      ld_args += argv[i];
    }
    else{
      if(argv[i][0] == '-'){
        std::cout << "Command '" << argv[i] << "' doesn't exist! Use '-h' to get a list of commands\n";
        return -1;
      }
      in = argv[i];
    }
    ++i;
  }
  std::string str = ReadFile(in);
  IR ir = IR(str);
  ir.Compile(ir.tree);
  std::cout << ir.IRCode << "\n";
  // std::cout << ir.IRCode << "\n";
  std::ofstream outFile("out.lfir");
  outFile << ir.IRCode;
  outFile.close();
  system("lf_ir");
  system("del out.asm > nul 2> nul");
  system("del out.obj > nul 2> nul");
  // system("del out.lfir > nul 2> nul");
  system((std::string("del ") + out + " > nul 2> nul").c_str());
  system(("rename \".\\out.exe\" \"" + out + "\" > nul 2> nul").c_str());
  std::cout << "Output to file " << out << "\n";
}

