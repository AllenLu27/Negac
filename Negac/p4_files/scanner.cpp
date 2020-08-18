#include <fstream>
#include "scanner.hpp"

using namespace negatron;

using TokenKind = negatron::Parser::token;
using Lexeme = negatron::Parser::semantic_type;

void Scanner::outputTokens(std::ostream& outstream){
	Lexeme lexeme;
	int tokenKind;
	while(true){
		tokenKind = this->yylex(&lexeme);
		if (tokenKind == TokenKind::END){
			outstream << "EOF" 
			  << " [" << this->lineNum 
			  << "," << this->charNum << "]"
			  << std::endl;
			return;
		} else {
			outstream << lexeme.transToken->toString()
			  << std::endl;
		}
	}
}
