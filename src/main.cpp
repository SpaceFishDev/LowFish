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
        default:
            return "UNKNOWN";
    }
}

// void printTree(Node* root, std::string prefix = "", bool isLastChild = true) {
//     std::cout << prefix;
//     if (isLastChild) {
//         std::cout << "\\---- ";
//         prefix += "  ";
//     } else {
//         std::cout << "|---- ";
//         prefix += "| ";
//     }
//     std::cout << "Type: " << NodeTypeToString(root->Type) << ", Token: " << root->NodeToken->Text << std::endl;

//     for (int i = 0; i < root->Children.size(); i++) {
//         printTree(root->Children[i], prefix, i == root->Children.size() - 1);
//     }
// }
void printTree(Node* Tree, int level);
std::string ReadFile(std::string Path){
	std::ifstream t(Path);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	std::string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);
	return buffer;
}

int main(){
	Parser parser = Parser(ReadFile("Test.LF"));
	Token t = Token(0,"",0,0);
	Node root = Node(&t, PROGRAM, nullptr);
	Node* N = parser.Parse(&root, &root);
	printTree(N, 0  );
}
void printTree(Node* n, int level){
	for(int i = 0; i != level; ++i){
		std::cout << "\t";
	}
	std::cout << "Node:( " <<  n->Type << " , " << n->NodeToken->Text << ")\n";
	if(n->Children.size() == 0){
		return;
	}
	++level;
	for(Node* N : n->Children){
		printTree(N, level);
	}
}
