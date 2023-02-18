#include"Lexer.h"
#include<vector>
enum NodeTypes{
	PROGRAM,
	FUNCTION,
	EQUAL,
	FUNCTION_CALL,
	CONSTANT_NODE,
	REFERENCE,
	VAR,
	BLOCK,
	BOOLEXPR,
	IFNODE,
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
				Tokens.push_back(t);
				return;
			}
			else{
				Tokens.push_back(t);
			}
		}
	}
	bool Expect(int Type){
		if(Position + 1 > Tokens.size()){
			ErrorHandler::PutError(-1, "Not enough tokens in input." , 0, 0);
		}
		return Tokens[Position + 1].Type == Type;
	}
	bool ExpectValue(std::string Value){
		if(Position + 1 > Tokens.size()){
			ErrorHandler::PutError(-1, "Not enough tokens in input." , 0, 0);
		}
		return Tokens[Position + 1].Text == Value;
	}
	Node* Parse(Node* Parent, Node* Root){
		if(Position > Tokens.size()){
			return Root;
		}
		Token Current = Tokens[Position];
		switch(Current.Type){
			case STRING:
			case CONSTANT:{
				Node* N = new Node(&Tokens[Position], CONSTANT_NODE, Parent);
				++Position;
				Parent->Children.push_back(N);
				return Parse(Parent, Root);
			}
			case SYMBOL:{
				if(Current.Text == "{"){
					Node* N = new Node(&Tokens[Position], BLOCK, Parent);
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				if(Current.Text == "}"){
					++Position;
					Node* N = Parent;

					while(N->Type != PROGRAM){
						N = N->Parent;
					}
					return Parse(N, Root);
				}
				if(Current.Text == ";" || Current.Text == ")"){
					++Position;
					Node* N = Parent;
					if(N->Type == IFNODE){
						return Parse(N, Root);
					}
					if(Parent->Type != BLOCK && Parent->Type != FUNCTION && Parent->Type != IFNODE){
						while(N->Type != FUNCTION && N->Type != PROGRAM && N->Type != BLOCK && N->Type != IFNODE){
							N = N->Parent;
						}
					}
					return Parse(N, Root);
				}
			} break;
			case IDENTIFIER:{
				
				if(Current.Text == "if"){
					if(!ExpectValue("(")){
						ErrorHandler::PutError(-1, "If statements require '(' expression ')' block. " , Current.Line, Current.Column);
					}
					Node* N = new Node(&Tokens[Position], IFNODE, Parent);
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				if(ExpectValue(";") || ExpectValue(")")){
					Node* N = new Node(&Tokens[Position], REFERENCE, Parent);
					++Position;
					Parent->Children.push_back(N);
					return Parse(Parent, Root);
				}

				if(ExpectValue("==")){
					Node* N = new Node(&Tokens[Position + 1], BOOLEXPR, Parent);
					N->Children.push_back(new Node(&Tokens[Position], REFERENCE, N));
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}

		
				if
				(
					Current.Text == "int" 
					|| Current.Text == "char" 
					|| Current.Text == "string" 
					|| Current.Text == "long" 
					|| Current.Text == "short"
				)
				{
					Node* E = new Node(&Tokens[Position], VAR, Parent);
					++Position;
					Parent->Children.push_back(E);
					return Parse(E, Root);
				}
				if(ExpectValue("=")){
					Node* N = new Node(&Tokens[Position], EQUAL, Parent);
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				int Type = 0;
				if(ExpectValue("fn")){
					Type = FUNCTION;
					Node* F = new Node(&Tokens[Position], Type, Parent);
					++Position;
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
					 Type = FUNCTION_CALL;
					 Node* F = new Node(&Tokens[Position], Type, Parent);
					++Position;
					++Position;
					Parent->Children.push_back(F);
					return Parse(F, Root);
				}
				
			} break;
		}
		return Root;
	}
private:
};
