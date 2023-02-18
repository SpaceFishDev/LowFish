#include<Syntax/Lexer.h>
#include<iostream>    
#include<vector> 
#include<algorithm>
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
	WHILENODE,
	STRUCT,
	MATH,
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

/*
*/

struct MathToken{
	std::string left;
	std::string right;
	int linel;
	int liner;
	int coll;
	int colr;
	int colo;
	int lineo;
	int op;
	MathToken(std::string l, std::string o, int l1, int l2, int cl1, int cl2){
		left = l;
		switch(o[0]){
			case '/':
			op = 0;
			break;
			case '*':
			op = 1;
			break;
			case '+':
			op = 2;
			break;
			case '-':
			op = 3;
			break;
			default:
			op = 100;
		}
		linel = l1;
		lineo = l2;
		coll = cl1;
		colo = cl2;
	}
};

class Parser{
public:
	std::string Source;
	Lexer lexer;
	int Position = 0;
	std::vector<Token> Tokens;
	std::vector<std::string> Functions;
	std::vector<std::string> Types;

	std::vector<Token> ReorderTokens(std::vector<Token> tokens){
		std::string math = "/*+-";
		std::vector<Token> Tokens;
		for(int i = 0; i != tokens.size(); ++i){
			if(tokens[i].Type != CONSTANT && tokens[i].Type != SYMBOL){
				Tokens.push_back(tokens[i]);
				continue;
			}
			std::vector<Token> MathExpr;
			while(true){
				if(tokens[i].Text != "+" 
				&& tokens[i].Text != "-"
				&& tokens[i].Text != "/"
				&& tokens[i].Text != "*"
				&& tokens[i].Type != CONSTANT
				)
				{
					break;	
				}
				MathExpr.push_back(tokens[i]);
				++i;
			}
			std::vector<MathToken> MathTokens;
			for(int x = 0; x < MathExpr.size();){
				if(MathExpr[x].Type == CONSTANT && x + 3 < MathExpr.size()){
					MathToken t = MathToken(
						MathExpr[x].Text, 
						MathExpr[x + 1].Text, 
						MathExpr[x].Line, 
						MathExpr[x + 1].Line, 
						MathExpr[x].Column, 
						MathExpr[x + 1].Column
					);
					MathTokens.push_back(t);
					x+=2;
				}
				if(x + 3 == MathExpr.size()){
					break;
				}
			}
			MathToken end = MathToken(
				MathExpr[MathExpr.size() - 3].Text, 
				MathExpr[MathExpr.size() - 2].Text, 
				MathExpr[MathExpr.size() - 3].Line, 
				MathExpr[MathExpr.size() - 2].Line, 
				MathExpr[MathExpr.size() - 3].Column, 
				MathExpr[MathExpr.size() - 2].Column
			);
			end.right = MathExpr[MathExpr.size() - 1].Text;
			MathTokens.push_back(MathToken(end));

			for (size_t i = 0; i < MathTokens.size() - 1; ++i) {
				for (size_t j = 0; j < MathTokens.size() - i - 1; ++j) {
					if (MathTokens.at(j).op > MathTokens.at(j + 1).op)
						std::swap(MathTokens.at(j), MathTokens.at(j + 1));
				}
			}
			int f = 0;
			std::vector<MathToken> copy;
			for(MathToken t : MathTokens) {
				copy.push_back(t);
			}
			
			for(MathToken t : MathTokens){
				std::string arr = "/*+-";
				if(t.right != ""){
					Tokens.push_back(Token(CONSTANT ,t.right, t.liner, t.colr));
					Tokens.push_back(Token(SYMBOL, std::string("") + arr[t.op], t.lineo, t.colo));
					Tokens.push_back(Token(CONSTANT ,t.left, t.linel, t.coll));
					break;
				}
				Tokens.push_back(Token(CONSTANT ,t.left, t.linel, t.coll));
				Tokens.push_back(Token(SYMBOL, std::string("") + arr[t.op], t.lineo, t.colo));
			}
		}
		return Tokens;
	}

	Parser(std::string source){
		Types.push_back("string");
		Types.push_back("int");
		Types.push_back("long");
		Types.push_back("char");
		Types.push_back("short");
		lexer = Lexer(source);

		while(true)
		{
			Token t = lexer.Tokenize();
			Tokens.push_back(t);
			if(t.Type == END){
				Tokens = ReorderTokens(Tokens);
				break;				
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
				std::string math = "/*+-";
				for(char c : math){
					if(ExpectValue(std::string("") + c)){
						Node* N = new Node(&Tokens[Position + 1], MATH, Parent);

						N->Children.push_back(new Node(&Tokens[Position], CONSTANT_NODE, N));
						Position += 2;	
						Parent->Children.push_back(N);
						return Parse(N, Root);
					}
				}
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
					if(N->Type == BLOCK && N->Parent->Type == IFNODE){
						N = N->Parent;
						while(N->Type != BLOCK){
							N = N->Parent;
						}	
						return Parse(N, Root);
					}
					while(N->Type != PROGRAM){
						N = N->Parent;
					}
					return Parse(N, Root);
				}
				if(Current.Text == ";" || Current.Text == ")"){
					++Position;
					Node* N = Parent;
					if(N->Type == IFNODE || N->Type == REFERENCE){
						return Parse(N, Root);
					}
					if(Parent->Type != BLOCK && Parent->Type != FUNCTION && Parent->Type != IFNODE){
						while(N->Type != FUNCTION && N->Type != PROGRAM && N->Type != BLOCK && N->Type != IFNODE){
							N = N->Parent;
						}
					}
					return Parse(N, Root);
				}
				if(Current.Text == "==" 
				|| Current.Text == "!="
				|| Current.Text == "<"
				|| Current.Text == ">"
				|| Current.Text == ">="
				|| Current.Text == "<="
				|| Current.Text == "&&"
				){
					Node* N = new Node(&Tokens[Position], BOOLEXPR, Parent);
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
			} break;
			case IDENTIFIER:{
				
				if(Current.Text == "struct"){
				
					if(!Expect(IDENTIFIER)){
						ErrorHandler::PutError(EXPECT_IDENTIFIER, " ", Current.Line, Current.Column);
					}
					for(std::string T : Types){
						if(T == Tokens[Position + 1].Text){
							ErrorHandler::PutError(REDEFINITION_OF_STRUCT, Tokens[Position+1].Text, Tokens[Position].Line, Tokens[Position].Column);
						}
					}
					Types.push_back(Tokens[Position + 1].Text);
					Node* N = new Node(&Tokens[Position + 1],STRUCT, Parent);
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
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
				if(Current.Text == "while"){
					if(!ExpectValue("(")){
						ErrorHandler::PutError(-1, "while statements require '(' expression ')' block. " , Current.Line, Current.Column);
					}
					Node* N = new Node(&Tokens[Position], WHILENODE, Parent);
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				if(ExpectValue(";") 
				|| ExpectValue(")")
				|| ExpectValue("+")
				|| ExpectValue("-")
				|| ExpectValue("/")
				|| ExpectValue("*")){
					Node* N = new Node(&Tokens[Position], REFERENCE, Parent);
					++Position;
					Parent->Children.push_back(N);
					return Parse(Parent, Root);
				}

				if(ExpectValue("==") 
				|| ExpectValue("!=")
				|| ExpectValue("<")
				|| ExpectValue(">")
				|| ExpectValue(">=")
				|| ExpectValue("<=")
				|| ExpectValue("&&")
				){
					Node* N = new Node(&Tokens[Position + 1], BOOLEXPR, Parent);
					N->Children.push_back(new Node(&Tokens[Position], REFERENCE, N));
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				for(std::string t : Types){
					if(Current.Text == t)
					{
						if(Expect(IDENTIFIER) && Tokens[Position + 2].Text == "("){
							++Position;
							int Type = FUNCTION;
							Node* F = new Node(&Tokens[Position - 1], Type, Parent);
							F->Children.push_back(new Node(&Tokens[Position], REFERENCE, F));
							for(std::string func : Functions){
								if(func == Tokens[Position].Text){
									ErrorHandler::PutError(REDEFINITION_OF_FUNCTION, func, Tokens[Position].Line, Tokens[Position].Column);
								}
							}
							Functions.push_back(Tokens[Position].Text);
							++Position;
							++Position;
							Parent->Children.push_back(F);
							return Parse(F->Children[0], Root);
						}
						Node* E = new Node(&Tokens[Position], VAR, Parent);
						++Position;
						Parent->Children.push_back(E);
						return Parse(E, Root);
					}
				}
				if(ExpectValue("=")){
					Node* N = new Node(&Tokens[Position], EQUAL, Parent);
					++Position;
					++Position;
					Parent->Children.push_back(N);
					return Parse(N, Root);
				}
				int Type = 0;
				
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
