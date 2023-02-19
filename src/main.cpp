#include <fstream>
#include<syntax/parser.h>

std::string NodeTypeToString(int type) {
    switch(type) {
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
        default:
                return "UNKNOWN";
    }
}

void printTree(Node* root, std::string prefix = "", bool isLastChild = true) {
    std::cout << prefix;
    if (isLastChild) {
        std::cout << "\\---->";
        prefix += "  ";
    } else {
        std::cout << "|---->";
        prefix += "| ";
    }
    std::cout << "Type: " << NodeTypeToString(root->Type) << ", Token: " << root->NodeToken->Text << std::endl;

    for (int i = 0; i < root->Children.size(); i++) {
        printTree(root->Children[i], prefix, i == root->Children.size() - 1);
    }
}
char* ReadFile(std::string Path){
    std::ifstream t(Path);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	char* buffer = (char*)malloc(size);
	t.seekg(0);
	t.read(buffer, size);
    buffer[size] = 0;   
    return buffer;
}

int main(){
    std::string str = ReadFile("Test.LF");
	Parser parser = Parser(str);
	Token t = Token(0,"",0,0);
	Node root = Node(&t, PROGRAM, nullptr);
	Node* N = parser.Parse(&root, &root);
	printTree(N);
}
// void printTree(Node* n, int level){
// 	for(int i = 0; i != level; ++i){
// 		std::cout << "\t";
// 	}
// 	std::cout << "Node:( " <<  n->Type << " , " << n->NodeToken->Text << ")\n";
// 	if(n->Children.size() == 0){
// 		return;
// 	}
// 	++level;
// 	for(Node* N : n->Children){
// 		printTree(N, level);
// 	}
// }
