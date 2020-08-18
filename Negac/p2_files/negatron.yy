%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%define api.namespace {negatron}
%define parser_class_name {Parser}
%define parse.assert
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

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { negatron::Scanner  &scanner  }
%parse-param { negatron::ProgramNode** root }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   /* include for interoperation between scanner/parser */
   #include "scanner.hpp"

#undef yylex
#define yylex scanner.yylex
}

/*
The %union directive is a way to specify the
set of possible types that might be used as
translation attributes in the syntax-directed
translations.
*/
%union {
  bool                                  transBool;
  negatron::Token *                     transToken;
  negatron::IntLitToken*                transIntToken;
  negatron::StrToken*                   transStrToken;
  negatron::IDToken *                   transIDToken;
  negatron::ProgramNode *               transProgram;
  std::list<negatron::DeclNode *> *     transDeclList;
  std::list<negatron::VarDeclNode *> *  s_body;
  std::list<negatron::FormalDeclNode *>* f_args;
  std::list<negatron::StmtNode *> *     s_list;
  std::list<negatron::ExpNode * > *      e_list;
  std::list<IDNode * >*                 Id_list;
  negatron::DeclNode *                  transDecl;
  negatron::VarDeclNode *               transVarDecl;
  negatron::TypeNode *                  transType;
  negatron::IDNode *                    transID;
  negatron::FormalDeclNode *            f_decl;
  negatron::StmtNode *                  s_node;
  negatron::LocNode *                   my_loc;
  negatron::ExpNode *                   m_exp;
  negatron::AssignExpNode*              m_assign;
  negatron::FnBodyNode*                 f_node;
  negatron::CallExpNode*                c_fn;
}

/* Nonterminals
*  TODO: You will need to add more nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type). Note that the specifier in brackets
*  indicates the type of the translation attribute using
*  the names defined in the %union directive above
*/
/*    (attribute type)    (nonterminal)    */
%type <transProgram>      program
%type <transDeclList>     globals
%type <transDecl>         decl
%type <transVarDecl>      varDecl
%type <transType>         type
%type <transID>           id
%type <transBool>         maybeRef
%type <transDecl>         structDecl
%type <transDecl>         fnDecl
%type <s_body>            structBody
%type <f_decl>            formalDecl
%type <f_args>            formalsList
%type <f_args>            formals
%type <f_node>            fnBody
%type <s_list>            stmtList
%type <s_node>            stmt
%type <Id_list>           loc
%type <m_exp>             term
%type <m_exp>             exp
%type <m_assign>          assignExp
%type <e_list>            actualsList
%type <c_fn>              fncall
/* TODO you will have to
   add an attribute type for
   a couple of the tokens below
*/
%right ASSIGN
%left AND OR
%nonassoc NOTEQUALS LESS LESSEQ
%left DASH
%left STAR SLASH
%precedence NOT

%token                 END    0     "end of file"
%token <transToken>    REF
%token <transToken>    BOOL
%token <transToken>    INT
%token <transToken>    VOID
%token <transToken>    FALSE
%token <transToken>    STRUCT
%token <transToken>    NULLREF
%token <transToken>    OUTPUT
%token <transToken>    INPUT
%token <transToken>    IF
%token <transToken>    ELSE
%token <transToken>    WHILE
%token <transToken>    RETURN
%token <transIDToken>  ID
%token <transIntToken> INTLITERAL
%token <transStrToken> STRLITERAL
%token <transToken>    LCURLY
%token <transToken>    RCURLY
%token <transToken>    LPAREN
%token <transToken>    RPAREN
%token <transToken>    SEMICOLON
%token <transToken>    COMMA
%token <transToken>    DOT
%token <transToken>    DASHDASH
%token <transToken>    DASH
%token <transToken>    STAR
%token <transToken>    SLASH
%token <transToken>    NOT
%token <transToken>    NOTEQUALS
%token <transToken>    LESS
%token <transToken>    LESSEQ
%token <transToken>    ASSIGN
%token <transToken>    AND
%token <transToken>    OR



/* TODO: Make sure to add precedence and associativity
 * declarations!
*/

%%

/* TODO: fill out the rest of the productions/SDD rules */
program 	: globals
	{
	$$ = new ProgramNode($1);
	*root = $$;
	}

globals 	: globals decl
	{
	$$ = $1;
	DeclNode * aGlobalDecl = $2;
	$1->push_back(aGlobalDecl);
	}
	| /* epsilon */
	{
	std::list<DeclNode *> * startingGlobals;
	startingGlobals = new std::list<DeclNode *>();
	$$ = startingGlobals;
	}

decl : varDecl
	{
	   $$ = $1;
	}
  | structDecl SEMICOLON
  {
    $$ = $1;
  }
  | fnDecl
  {
    $$ = $1;
  }

varDecl : type id SEMICOLON
	{
	size_t typeLine = $1->line();
	size_t typeCol = $1->col();
	$$ = new VarDeclNode(typeLine, typeCol, $1, $2);
	}

  type 	: INT maybeRef
  	{
  	bool isRef = $2;
  	$$ = new IntTypeNode($1->line(), $1->col(), isRef);
  	}
    |  BOOL maybeRef
      {
        bool isRef = $2;
      	$$ = new BoolTypeNode($1->line(), $1->col(), isRef);
      }
    |  VOID
       {
         $$ = new VoidTypeNode($1->line(), $1->col());
       }
    |  STRUCT id maybeRef
      {
        IDNode* m_id = $2;
        bool isRef = $3;
      	$$ = new StructTypeNode($1->line(), $1->col(), m_id, isRef);
      }
  	/*
  	Note that there are a lot more types for you to fill in!
  	*/

  /* This production is a good example of recognizing an optional token:
     the programmer could declare int a; or int & b; matching the type
     production above. In the former declaration, maybeRef produces epsilon
     and has its attribute translation set to false. .
     In the latter declaration, maybeRef matches the REF token and has its
     translation attribute set to true.
  */
  maybeRef : REF
  	{ $$ = true; }
  	| /* epsilon */
  	{ $$ = false; }

structDecl : STRUCT id LCURLY structBody RCURLY
{
  IDNode* s_id = $2;
  VarDeclListNode* s_list = new VarDeclListNode($4);
  $$ = new StructDeclNode(s_id, s_list);
}

structBody : structBody varDecl
          {
            $$ = $1;
            VarDeclNode* m_decl = $2;
            $1->push_back(m_decl);
          }
          | varDecl
          {
            std::list<VarDeclNode *> * m_list;
            m_list = new std::list<VarDeclNode *>();
            m_list->push_back($1);
            $$ = m_list;
          }

fnDecl : type id formals fnBody
{
  size_t typeLine = $1->line();
	size_t typeCol = $1->col();
  TypeNode* m_type = $1;
  IDNode* m_id = $2;
  FormalDeclNodeList* m_list = new FormalDeclNodeList($3);
  $$ = new FnDeclNode(m_type, m_id, m_list, $4);
}

formals : LPAREN RPAREN
  {
    $$ = nullptr;
  }
  | LPAREN formalsList RPAREN
  {
    $$ = $2;
  }

formalsList : formalDecl
  {
    std::list<FormalDeclNode * >* m_list;
    m_list = new std::list<FormalDeclNode * >();
    m_list->push_front($1);
    $$ = m_list;
  }
  | formalDecl COMMA formalsList
  {
    $$ = $3;
    $3->push_front($1);
  }

 formalDecl : type id
 {
   $$ = new FormalDeclNode($1, $2);
 }

 fnBody : LCURLY stmtList RCURLY
 {
   StmtListNode* m_list = new StmtListNode($2);
   $$ = new FnBodyNode(m_list);
 }

 stmtList : stmtList stmt
  {
    $$ = $1;
    $1->push_back($2);
  }
  | /* epsilon */
  {
    std::list<negatron::StmtNode *> * s_list = new std::list<negatron::StmtNode *>();
    $$ = s_list;
  }

stmt : varDecl
      {
        $$ = $1;
      }
      | assignExp SEMICOLON
      {
        $$ = new AssignStmtNode($1);
      }
      | loc DASHDASH SEMICOLON
      {
        DotAccessNode* m_loc = new DotAccessNode($1);
        $$ = new PostDeclStmtNode(m_loc);
      }
      | INPUT loc SEMICOLON
      {
        DotAccessNode* m_loc = new DotAccessNode($2);
        $$ = new InputStmtNode(m_loc);
      }
      | OUTPUT exp SEMICOLON
      {
        $$ = new OutputStmtNode($2);
      }
      | IF LPAREN exp RPAREN LCURLY stmtList RCURLY
      {
        StmtListNode* m_list = new StmtListNode($6);
        $$ = new IfStmtNode($3, m_list);
      }
      | IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
      {
        StmtListNode* if_list = new StmtListNode($6);
        StmtListNode* else_list = new StmtListNode($10);
        $$ = new IfElseStmtNode($3, if_list, else_list);
      }
      | WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
      {
        StmtListNode* m_list = new StmtListNode($6);
        $$ = new WhileStmtNode($3, m_list);
      }
      | RETURN exp SEMICOLON
      {
        $$ = new ReturnStmtNode($2);
      }
      | RETURN SEMICOLON
      {
        $$ = new ReturnStmtNode();
      }
      | fncall SEMICOLON
      {
        $$ = new CallStmtNode($1);
      }

assignExp : loc ASSIGN exp
 {
   std::list<IDNode * >* loc_list = $1;
   DotAccessNode* m_loc = new DotAccessNode(loc_list);
   $$ = new AssignExpNode(m_loc, $3);
 }

 fncall : id LPAREN RPAREN   // fn call with no args
  {
    $$ = new CallExpNode($1);
  }
  | id LPAREN actualsList RPAREN  // with args
    {
      $$ = new CallExpNode($1, $3);
    }

 exp : assignExp
  {
      $$ = $1;
   }
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
     $$ = new NotEqualNode($2->line(), $2->col(), $1, $3);
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
   {
     $$ = $1;
   }


   actualsList  : exp
   {
     std::list<ExpNode * >* args = new std::list<ExpNode * >();
     args->push_back($1);
     $$ = args;
   }
   | actualsList COMMA exp
   {
     $$ = $1;
     $1->push_back($3);
   }

   term  : loc
   {
     $$ = new DotAccessNode($1);
   }
   | INTLITERAL
   {
     $$ = new IntLitNode($1);
   }
   | STRLITERAL
   {
     $$ = new StrLitNode($1);
   }
   | FALSE
   {
     $$ = new FalseNode($1->line(), $1->col());
   }
   | NULLREF
   {
     $$ = new NullRefNode($1->line(), $1->col());
   }
   | LPAREN exp RPAREN
   {
     $$ = $2;
   }
   | fncall
   {
     $$ = $1;
   }

loc  : id
   {
     std::list<IDNode * >* m_list;
     m_list = new std::list<IDNode * >();
     m_list->push_back($1);
     $$ = m_list;
   }
    | loc DOT id
    {
      $$ = $1;
      $1->push_back($3);
    }

id : ID { $$ = new IDNode($1); }
%%
void
negatron::Parser::error(const std::string &err_message )
{
   /* For project grading, only report "syntax error"
      if a program has bad syntax. However, you will
      probably want better output for debugging. Thus,
      this error function prints a verbose message to
      stdout, but only prints "syntax error" to stderr
   */
   std::cout << err_message << std::endl;
   std::cerr << "syntax error" << std::endl;
}
