%{
#include <string>
#include <limits.h>

/* Connect our custom yyFlexScanner subclass */
#include "scanner.hpp"
#undef  YY_DECL
#define YY_DECL int negatron::Scanner::yylex( negatron::Parser::semantic_type * const lval )

/* typedef to make the reference to token kinds shorter */
using TokenKind = negatron::Parser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( TokenKind::END )

/* Exclude unistd.h for Visual Studio compatability. */
#define YY_NO_UNISTD_H


// TODO: You will have to fill out the rules section for the
//       negatron language

%}

%option debug
%option nodefault
%option yyclass="negatron::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"-"		{
		// Keep track of the "kind" of token we matched. In this case,
		// it's a CROSS (aka the plus symbol). The full list of token
		// kinds is declared around line 197 of grammar.hh. The testing
		// functionScanner::outputTokens also indicates what each token
		// represents (declared in scanner.cpp line 9)
		int tokenKind = TokenKind::DASH;

		//Instatiate a token. There are 4 classes of tokens defined based on
		// - a literal string (StringLitToken)
		// - a literal integer (IntLitToken)
		// - an identifier (IDToken)
		// - a NoArg token (any other type)
		// The token object is a handy place to keep information about the
		// token (i.e. token metadata) such as the line and column on which
		// the token character started (which can be used later in debug
		// messages, etc).
		Token * token = new Token(lineNum, charNum, tokenKind);

		//Assign the token to a instance variable of the scanner.
		// (When Flex processes a .l file in C++ mode, it creates
		// a class called Scanner, which is a subclass of yyFlexScanner)
		// By assigning a value to this field, we put the token
		// metadata in a place where scanner's client program can find it.
		this->yylval->transToken = token;

		//Since we consumed 1 character, increase the column number by 1
		charNum++;

		//Let the scanner's client program know that a CROSS token
		// has been matched
		return tokenKind;
		}

"*" {
		//Essentially the same thing as CROSS, but without the verboes
		// comments so it doesn't look so scary.
		int tokenKind = TokenKind::STAR;
		this->yylval->transToken = new Token(lineNum, charNum, tokenKind);
		charNum++;
		return tokenKind;
		}

{DIGIT}+	{
		//TODO: The following computation of the integer
		// value does not check for overflow. It should do so.
		int intVal = atoi(yytext);

    if(intVal > INT_MAX)
    {
      intVal = INT_MAX;
      std::string msg = "Integer literal too large ";
      Report::fatal(lineNum,charNum,msg);
    }
		//Note that the IntLitToken does not take a TokenKind argument
		// since it's implicitly known that the token represents an
		// INTLITERAL
		  yylval->transToken = new IntLitToken(lineNum, charNum, intVal);

		  //TODO: update the charNum count according to the number of
		  // characters consumed.
      charNum += yyleng;
		  //As before, alert the client program that an INTLITERAL
		  // token was matched
		  return TokenKind::INTLITERAL;
		}

\n		{
		//Update the line number, reset the columnNumber
		lineNum++;
		charNum = 0;
		}


.   {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for.
		std::string msg = "Illegal character ";
		msg += yytext;
		Report::fatal(lineNum,charNum,msg);
		charNum += yyleng;
    }
%%
