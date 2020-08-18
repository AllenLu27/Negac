#include "tokens.hpp" // Get the class declarations
#include "grammar.hh" // Get the TokenKind definitions

namespace negatron{

using TokenKind = negatron::Parser::token;
using Lexeme = negatron::Parser::semantic_type;

static std::string tokenKindString(int tokKind){
	switch(tokKind){
		case TokenKind::END: return "EOF";
		case TokenKind::REF: return "REF";
		case TokenKind::BOOL: return "BOOL";
		case TokenKind::INT: return "INT";
		case TokenKind::VOID: return "VOID";
		case TokenKind::FALSE: return "FALSE";
		case TokenKind::STRUCT: return "STRUCT";
		case TokenKind::NULLREF: return "NULLREF";
		case TokenKind::OUTPUT: return "OUTPUT";
		case TokenKind::INPUT: return "INPUT";
		case TokenKind::IF: return "IF";
		case TokenKind::ELSE: return "ELSE";
		case TokenKind::WHILE: return "WHILE";
		case TokenKind::RETURN: return "RETURN";
		case TokenKind::ID: return "ID";
		case TokenKind::INTLITERAL: return "INTLIT";
		case TokenKind::STRLITERAL: return "STRINGLIT";
		case TokenKind::LCURLY: return "LCURLY";
		case TokenKind::RCURLY: return "RCURLY";
		case TokenKind::LPAREN: return "LPAREN";
		case TokenKind::RPAREN: return "RPAREN";
		case TokenKind::SEMICOLON: return "SEMICOLON";
		case TokenKind::COMMA: return "COMMA";
		case TokenKind::DOT: return "DOT";
		case TokenKind::DASHDASH: return "DASHDASH";
		case TokenKind::DASH: return "DASH";
		case TokenKind::STAR: return "STAR";
		case TokenKind::SLASH: return "SLASH";
		case TokenKind::NOT: return "NOT";
		case TokenKind::NOTEQUALS: return "NOTEQUALS";
		case TokenKind::LESS: return "LESS";
		case TokenKind::LESSEQ: return "LESSEQ";
		case TokenKind::ASSIGN: return "ASSIGN";
		case TokenKind::AND: return "AND";
		case TokenKind::OR: return "OR";
		default:	
			return "OTHER";
	}
	
}

Token::Token(size_t lineIn, size_t columnIn, int kindIn)
  : myLine(lineIn), myCol(columnIn), myKind(kindIn){
}

std::string Token::toString(){
	return tokenKindString(kind())
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

size_t Token::line() const { 
	return this->myLine; 
}

size_t Token::col() const { 
	return this->myCol; 
}

int Token::kind() const { 
	return this->myKind; 
}

IDToken::IDToken(size_t lIn, size_t cIn, std::string vIn)
  : Token(lIn, cIn, TokenKind::ID), myValue(vIn){ 
}

std::string IDToken::toString(){
	return tokenKindString(kind()) + ":"
	+ this->myValue
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

const std::string IDToken::value() const { 
	return this->myValue; 
}

StrToken::StrToken(size_t lIn, size_t cIn, std::string sIn)
  : Token(lIn, cIn, TokenKind::STRLITERAL), myStr(sIn){
}

std::string StrToken::toString(){
	return tokenKindString(kind()) + ":"
	+ this->myStr
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

const std::string StrToken::str() const {
	return this->myStr;
}

IntLitToken::IntLitToken(size_t lIn, size_t cIn, int numIn)
  : Token(lIn, cIn, TokenKind::INTLITERAL), myNum(numIn){
}

std::string IntLitToken::toString(){
	return tokenKindString(kind()) + ":"
	+ std::to_string(this->myNum)
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

int IntLitToken::num() const {
	return this->myNum;
}

} //End namespace negatron
