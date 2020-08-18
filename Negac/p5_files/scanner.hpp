#ifndef __NEGA_SCANNER_HPP__
#define __NEGA_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "grammar.hh"
#include "errors.hpp"

namespace negatron{

class Scanner : public yyFlexLexer{
public:
   
   Scanner(std::istream *in) : yyFlexLexer(in)
   {
	lineNum = 1;
	charNum = 1;
   };
   virtual ~Scanner() {
   };

   //get rid of override virtual function warning
   using FlexLexer::yylex;

   // YY_DECL defined in the flex negatron.l
   virtual int yylex( negatron::Parser::semantic_type * const lval);

   int makeBareToken(int tagIn){
        this->yylval->transToken = new Token(
	  this->lineNum, this->charNum, tagIn);
        charNum += static_cast<size_t>(yyleng);
        return tagIn;
   }

   void warn(int lineNumIn, int charNumIn, std::string msg){
	std::cerr << lineNumIn << ":" << charNumIn 
		<< " ***WARNING*** " << msg << std::endl;
   }

   void error(int lineNumIn, int charNumIn, std::string msg){
	std::cerr << lineNumIn << ":" << charNumIn 
		<< " ***ERROR*** " << msg << std::endl;
   }

   static std::string tokenKindString(int tokenKind);

   void outputTokens(std::ostream& outstream);

private:
   negatron::Parser::semantic_type *yylval = nullptr;
   size_t lineNum;
   size_t charNum;
};

} /* end namespace */

#endif /* END __NEGA_SCANNER_HPP__ */
