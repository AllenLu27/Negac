%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace{negatron}
%define parser_class_name {Parser}
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	#include "ast.hpp"
	namespace negatron {
		class Scanner;
	}

//The following definition is required when
// we don't have the %locations directive
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { negatron::Scanner &scanner }
%parse-param { negatron::ProgramNode** root }

%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   #include "ast.hpp"
   #include "tokens.hpp"

  //Request tokens from our scanner member, not
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}

%union {
   bool  									transBool;
   negatron::Token* 						transToken;
   negatron::IDToken* 						transIDToken;
   negatron::IntLitToken* 					transIntToken;
   negatron::StrToken* 						transStrToken;
   negatron::ProgramNode* 					transProgram;
   std::list<negatron::DeclNode *> * 		transDeclList;
   negatron::DeclNode * 					transDecl;
   negatron::VarDeclNode * 					transVarDecl;
   std::list<negatron::FormalDeclNode *> * 	transFormals;
   negatron::FormalDeclNode * 				transFormal;
   negatron::TypeNode * 					transType;
   negatron::IDNode * 						transID;
   std::list<negatron::VarDeclNode *> * 	transFields;
   negatron::FnDeclNode * 					transFn;
   std::list<negatron::VarDeclNode *> * 	transVarDecls;
   std::list<negatron::StmtNode *> *		transStmts;
   negatron::StmtNode *						transStmt;
   negatron::ExpNode *						transExp;
   negatron::AssignExpNode *				transAssignExp;
   negatron::CallExpNode *					transCallExp;
   std::list<negatron::ExpNode *> * 		transActuals;
}

%define parse.assert

%token                   END	   0 "end file"
%token	<transToken>     BOOL
%token	<transToken>     INT
%token	<transToken>     VOID
%token	<transToken>     FALSE
%token	<transToken>     OUTPUT
%token	<transToken>     INPUT
%token	<transToken>     IF
%token	<transToken>     ELSE
%token	<transToken>     WHILE
%token	<transToken>     RETURN
%token	<transIDToken>   ID
%token	<transIntToken>  INTLITERAL
%token	<transStrToken>  STRLITERAL
%token	<transToken>     LCURLY
%token	<transToken>     RCURLY
%token	<transToken>     LPAREN
%token	<transToken>     RPAREN
%token	<transToken>     SEMICOLON
%token	<transToken>     COMMA
%token	<transToken>     DOT
%token	<transToken>     DASHDASH
%token	<transToken>     DASH
%token	<transToken>     STAR
%token	<transToken>     SLASH
%token	<transToken>     NOT
%token	<transToken>     NOTEQUALS
%token	<transToken>     LESS
%token	<transToken>     LESSEQ
%token	<transToken>     ASSIGN
%token	<transToken>     AND
%token	<transToken>     OR

%type <transProgram>    program
%type <transDeclList>  	globals
%type <transDecl>    	decl
%type <transVarDecl>   	varDecl
%type <transType>		type
%type <transID>    		id
%type <transFn>    		fnDecl
%type <transFormals>  	formals
%type <transFormals>  	formalsList
%type <transFormal>  	formalDecl
%type <transStmts>    	fnBody
%type <transStmts>   	stmtList
%type <transStmt>   	stmt
%type <transAssignExp> 	assignExp
%type <transExp> 		exp
%type <transExp> 		term
%type <transCallExp> 	callExp
%type <transActuals> 	actualsList

/* NOTE: Make sure to add precedence and associativity
 * declarations
 */
%right ASSIGN
%left DOT
%left OR
%left AND
%nonassoc LESS LESSEQ NOTEQUALS
%left DASH
%left STAR SLASH
%left NOT

%%

program 	: globals
		  {
		  $$ = new ProgramNode($1);
		  *root = $$;
		  }

globals 	: globals decl
	  	  {
	  	  $$ = $1;
	  	  DeclNode * declNode = $2;
		  $$->push_back(declNode);
	  	  }
		| /* epsilon */
		  {
		  $$ = new std::list<DeclNode * >();
		  }

decl 		: varDecl SEMICOLON
		  { $$ = $1; }
		| fnDecl
		  { $$ = $1; }

varDecl 	: type id
		  {
		  size_t line = $1->line();
		  size_t col = $1->col();
		  $$ = new VarDeclNode(line, col, $1, $2);
		  }

type 		: INT
	  	  {
		  $$ = new IntTypeNode($1->line(), $1->col());
		  }
		| BOOL
		  {
		  $$ = new BoolTypeNode($1->line(), $1->col());
		  }
		| VOID
		  {
		  $$ = new VoidTypeNode($1->line(), $1->col());
		  }

fnDecl 		: type id formals fnBody
		  {
		  $$ = new FnDeclNode($1->line(), $1->col(),
		    $1, $2, $3, $4);
		  }

formals 	: LPAREN RPAREN
		  {
		  $$ = new std::list<FormalDeclNode *>();
		  }
		| LPAREN formalsList RPAREN
		  {
		  $$ = $2;
		  }


formalsList	: formalDecl
		  {
		  $$ = new std::list<FormalDeclNode *>();
		  $$->push_back($1);
		  }
		| formalDecl COMMA formalsList
		  {
		  $$ = $3;
		  $$->push_front($1);
		  }

formalDecl 	: type id
		  {
		  $$ = new FormalDeclNode($1->line(), $1->col(),
		    $1, $2);
		  }

fnBody		: LCURLY stmtList RCURLY
		  {
		  $$ = $2;
		  }

stmtList 	: /* epsilon */
	   	  {
		  $$ = new std::list<StmtNode *>();
		  //$$->push_back($1);
	   	  }
		| stmtList stmt
	  	  {
		  $$ = $1;
		  $$->push_back($2);
	  	  }

stmt		: varDecl SEMICOLON
		  {
		  $$ = $1;
		  }
		| assignExp SEMICOLON
		  {
		  $$ = new AssignStmtNode($1->line(), $1->col(), $1);
		  }
		| id DASHDASH SEMICOLON
		  {
		  $$ = new PostDecStmtNode($2->line(), $2->col(), $1);
		  }
		| INPUT id SEMICOLON
		  {
		  $$ = new InputStmtNode($1->line(), $1->col(), $2);
		  }
		| OUTPUT exp SEMICOLON
		  {
		  $$ = new OutputStmtNode($1->line(), $1->col(), $2);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY
		  {
		  $$ = new IfStmtNode($1->line(), $1->col(), $3, $6);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
		  {
		  $$ = new IfElseStmtNode($1->line(), $1->col(), $3,
		    $6, $10);
		  }
		| WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
		  {
		  $$ = new WhileStmtNode($1->line(), $1->col(), $3, $6);
		  }
		| RETURN exp SEMICOLON
		  {
		  $$ = new ReturnStmtNode($1->line(), $1->col(), $2);
		  }
		| RETURN SEMICOLON
		  {
		  $$ = new ReturnStmtNode($1->line(), $1->col(), nullptr);
		  }
		| callExp SEMICOLON
		  { $$ = new CallStmtNode($1->line(), $1->col(), $1); }

exp		: assignExp
		  { $$ = $1; }
		| exp DASH exp
	  	  {
		  $$ = new MinusNode($2->line(), $2->col(), $1, $3);
		  }
		| exp STAR exp
	  	  {
		  $$ = new TimesNode($2->line(), $2->col(), $1, $3);
		  }
		| exp SLASH exp
	  	  {
		  $$ = new DivideNode($2->line(), $2->col(), $1, $3);
		  }
		| exp AND exp
	  	  {
		  $$ = new AndNode($2->line(), $2->col(), $1, $3);
		  }
		| exp OR exp
	  	  {
		  $$ = new OrNode($2->line(), $2->col(), $1, $3);
		  }
		| exp NOTEQUALS exp
	  	  {
		  $$ = new NotEqualsNode($2->line(), $2->col(), $1, $3);
		  }
		| exp LESS exp
	  	  {
		  $$ = new LessNode($2->line(), $2->col(), $1, $3);
		  }
		| exp LESSEQ exp
	  	  {
		  $$ = new LessEqNode($2->line(), $2->col(), $1, $3);
		  }
		| NOT exp
	  	  {
		  $$ = new NotNode($1->line(), $1->col(), $2);
		  }
		| DASH term
	  	  {
		  $$ = new NegNode($1->line(), $1->col(), $2);
		  }
		| term
	  	  { $$ = $1; }

assignExp	: id ASSIGN exp
		  {
		  $$ = new AssignExpNode($2->line(), $2->col(), $1, $3);
		  }

callExp		: id LPAREN RPAREN
		  {
		  std::list<ExpNode *> * noargs =
		    new std::list<ExpNode *>();
		  $$ = new CallExpNode($1->line(), $1->col(), $1, noargs);
		  }
		| id LPAREN actualsList RPAREN
		  {
		  $$ = new CallExpNode($1->line(), $1->col(), $1, $3);
		  }

actualsList	: exp
		  {
		  std::list<ExpNode *> * list =
		    new std::list<ExpNode *>();
		  list->push_back($1);
		  $$ = list;
		  }
		| actualsList COMMA exp
		  {
		  $$ = $1;
		  $$->push_back($3);
		  }

term 		: id
		  { $$ = $1; }
		| INTLITERAL
		  { $$ = new IntLitNode($1->line(), $1->col(), $1->num()); }
		| STRLITERAL
		  { $$ = new StrLitNode($1->line(), $1->col(), $1->str()); }
		| FALSE
		  { $$ = new FalseNode($1->line(), $1->col()); }
		| LPAREN exp RPAREN
		  { $$ = $2; }
		| callExp
		  { $$ = $1; }

id		: ID
		  {
		  $$ = new IDNode($1->line(), $1->col(), $1->value());
		  }

%%

void negatron::Parser::error(const std::string& msg){
	std::cout << msg << std::endl;
	std::cerr << "syntax error" << std::endl;
}
