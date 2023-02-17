#pragma once

#include<iostream>

struct Token{
	int Type;
	std::string Text;
	int Line;
	int Column;
	Token(int t, std::string txt, int l, int c){
		Type = t;
		Text = txt;
		Line = l;
		Column = c;
	}
	Token(){
	}
};


enum ErrorTypes{
	NEVER_ENDING_STRING,
	UNEXPECTED_CHARACTER
};

class ErrorHandler{
public:
	static void PutError(int type, char c,int line, int column){
		switch(type){
			case NEVER_ENDING_STRING:{
				std::cout << "String never ends. LN: " << line << " COL: " << column << "\n";
				exit(-1);
			}
			case UNEXPECTED_CHARACTER:{
				std::cout << "Unexpected character '" << c <<"' in input. LN: " << line << " COL: " << column << "\n";
				exit(-1);
			}
		}
		return;
	}
};

enum LexerTypes{
	END,
	CONSTANT,
	STRING,
	IDENTIFIER,
	SEMI,
	EQ,
	SYMBOL,
	ASM,
};


class Lexer{
public:
	std::string Source;
	int Position;
	int Line;
	int Column;
	Lexer(std::string src){
		Source = src;
		Position = 0;
		Line = 0;
		Column = 0;
	}
	Lexer(){

	}
	Token Tokenize(){
		if(Source[Position] == '$'){
			++Position;
			std::string A = "";
			while(Source[Position] != '$' && Source[Position] != 0){
				A += Source[Position];
				++Position;
			}
			return Token(ASM, A, Line, Column);
		}
		if(Source[Position] == '#'){
			while(Source[Position] != '\n' && Source[Position] != '\0'){
				++Position;
			}
			++Position;
			return Tokenize();
		}
		switch(Source[Position]){
			case '(':
			case ')':
			case '{':
			case ',':
			case '}':{
				std::string o = " ";
				o[0] = Source[Position];
				++Position;
				return Token(SYMBOL, o, Line, Column);
			}
			case '\'':
			case '"':{
				std::string out = "";
				++Position;
				++Column;
				while(Position < Source.length() && Source[Position] != '\'' && Source[Position] != '"'){
					if(Source[Position] == '\n'){
						ErrorHandler::PutError(NEVER_ENDING_STRING,0, Line, Column);
					}
					else{
						out += Source[Position];
					}
					++Position;
					++Column;
				}
				++Position;
				return Token(STRING, out, Line, Column);
			}
			case '\n':{
				++Line;
				++Position;
				Column = 0;
				return Tokenize();
			}
			case '\t':
			case ' ':{
				++Position;
				return Tokenize();
			}
			case ';':{
				++Position;
				++Column;
				return Token(SEMI, ";", Line, Column);
			}
			case '=':{
				++Position;
				++Column;
				return Token(EQ, "=", Line,Column);
			}
			case '\0':{
				++Position;
				return Token(END, "", Line, Column);
			}
		}
		if(IsLetter(Source[Position])){
			std::string out = "";
			while(IsLetter(Source[Position]) || Source[Position] == '_' || IsDigit(Source[Position])){
				out += Source[Position];
				++Position;
				++Column;
			}
			return Token(IDENTIFIER, out, Line, Column);
		}
		if(IsDigit(Source[Position])){
			std::string out = "";
			while(IsDigit(Source[Position])){
				out += Source[Position];
				++Position;
				++Column;
			}
			++Position; ++Column;
			return Token(CONSTANT, out, Line, Column);
		}
		ErrorHandler::PutError(UNEXPECTED_CHARACTER, Source[Position], Line, Column);
		return Token(END, "", Line, Column);
	}
private:
	bool IsDigit(char c){
		return c >= '0' && c <= '9';
	}
	bool IsLetter(char c){
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
};
