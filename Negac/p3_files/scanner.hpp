#ifndef __NEGA_SCANNER_HPP__
#define __NEGA_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "grammar.hh"

/*
Note that errors can be called from the generated yylex
*/
#include "errors.hpp"

namespace negatron{

class Scanner : public yyFlexLexer{
public:
	Scanner(std::istream *in) : yyFlexLexer(in){
		this->lineNum = 1;
		this->charNum = 1;
	}
	virtual ~Scanner(){
	}

	//TODO: try moving this to the top?
	//get rid of overridea virtual function warning
	using FlexLexer::yylex;

	virtual int yylex( negatron::Parser::semantic_type * const lval);

	int makeBareToken(int tagIn){
		this->yylval->transToken = new Token(
		  this->lineNum, this->charNum, tagIn
		);
		charNum += static_cast<size_t>(yyleng);
		return tagIn;
	}

	static std::string tokenKindString(int tokenKind);

	void outputTokens(std::ostream& outstream);
private:
	negatron::Parser::semantic_type *yylval = nullptr;
	size_t lineNum;
	size_t charNum;
};

}

#endif
