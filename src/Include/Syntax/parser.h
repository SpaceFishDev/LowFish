#include"Lexer.h"
#include<vector>
enum NodeTypes{
	PROGRAM,
	FUNCTION,
	EQUAL,
	FUNCTION_CALL,
	CONSTANT_NODE,
	REFERENCE,
	BLOCK,
};
class Node{
public:
	Token* NodeToken;
	std::vector<Node*> Children;
	int Type;
	Node* Parent;
	Node(Token* t, int T, Node* Parent){
		NodeToken = t;
		Type = T;
		this->Parent = Parent;
	}
private:
};

class Parser{
public:
	std::string Source;
	Lexer lexer;
	int Position = 0;
	std::vector<Token> Tokens;
	std::vector<std::string> Functions;
	Parser(std::string source){
		lexer = Lexer(source);
		while(true)
		{
			Token t = lexer.Tokenize();
			if(t.Type == END){
				Tokens.push_back(t	);
				std::cout << "EOF\n";
				return;
			}
			else{
				Tokens.push_back(t);
				std::cout << "Token:( " << t.Type << ", '" << t.Text << "'" << ")\n";
			}
		}
	}
	bool Expect(int Type){
		if(Position + 1 > Tokens.size()){
			ErrorHandler::PutError(UNEXPECTED_CHARACTER, 'a', -1, -11);
		}
		return Tokens[Position + 1].Type == Type;
	}
	bool ExpectValue(std::string Value){
		if(Position + 1 > Tokens.size()){
			ErrorHandler::PutError(UNEXPECTED_CHARACTER, 'a', -1, -11);
		}
		return Tokens[Position + 1].Text == Value;
	}
	Node* Parse(Node* Parent, Node* Root){
		if(Position > Tokens.size()){
			return Root;
		}
		Token Current = Tokens[Position];
		switch(Current.Type){
			case SYMBOL:{
				if(Current.Text == "{"){
					Node* N = new Node(&Tokens[Position], BLOCK, Parent);
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				if(Current.Text == "}"){
					++Position;
					if(Parent->Parent->Type == FUNCTION){
						return Parse(Parent->Parent->Parent, Root);
					}
					else{
						return Parse(Parent->Parent, Root);
					}
				}
			} break;
			case IDENTIFIER:{
				if(!Expect(SYMBOL) && !ExpectValue("(") && !ExpectValue("=") && !ExpectValue("fn")){
					return nullptr;
				}
				if(ExpectValue("=")){
					int Type = EQUAL;
					Node* V = new Node(&Tokens[Position], Type, Parent);
					++Position;
					if(Expect(CONSTANT) || Expect(STRING)){
						++Position;
						Node* N = new Node(&Tokens[Position], CONSTANT_NODE, V);
						V->Children.push_back(N);
						++Position;
					}
					else if(Expect(IDENTIFIER)){
						++Position;
						Node* N = new Node(&Tokens[Position], REFERENCE, V);
						V->Children.push_back(N);
						++Position;
					}
					else{
						return Root;
					}
					Parent->Children.push_back(V);
					return Parse(Parent, Root);
				}

				if(ExpectValue("fn")){
					++Position;
					int Type = FUNCTION;
					Node* F = new Node(&Tokens[Position - 1], Type, Parent);
					++Position;
					++Position;
					while(true){
						if(Tokens[Position].Text == "{" || Tokens[Position].Text == ")"){
							break;
						}
						if(!(Expect(IDENTIFIER) || ExpectValue(",")) && (!Expect(SYMBOL) && !ExpectValue(")")) && !ExpectValue("{")){
							break;
						}
						if(Tokens[Position].Text == "," ){
							++Position;
							continue;
						}
						if(Tokens[Position].Text != ")"){
							Node* N = new Node(&Tokens[Position], REFERENCE, F);
							F->Children.push_back(N);
						}
						++Position;
					}
					Parent->Children.push_back(F);
					++Position;
					return Parse(F, Root);
				}
				if(ExpectValue("(")){
					int Type = FUNCTION_CALL;
					Node* F = new Node(&Tokens[Position], Type, Parent);
					++Position;
					++Position;
					while(true){
						if(Tokens[Position].Text == "{" || Tokens[Position].Text == ")"){
							break;
						}
						if(!(Expect(IDENTIFIER) || ExpectValue(",")) && (!Expect(SYMBOL) && !ExpectValue(")")) && !ExpectValue("{")){
							break;
						}
						if(Tokens[Position].Text == "," ){
							++Position;
							continue;
						}
						if(Tokens[Position].Text != ")"){
							Node* N = new Node(&Tokens[Position], REFERENCE, F);
							F->Children.push_back(N);
						}
						++Position;
					}
					Parent->Children.push_back(F);
					++Position;
					return Parse(Parent, Root);
				}
			} break;
		}
		return Root;
	}
private:
};
