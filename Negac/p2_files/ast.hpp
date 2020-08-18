#ifndef NEGATRON_AST_HPP
#define NEGATRON_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"

// **********************************************************************
// ASTnode class (base class for all other kinds of nodes)
// **********************************************************************

namespace negatron{

/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/
class DeclListNode;
class DeclNode;
class TypeNode;
class IDNode;

class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
	: l(lineIn), c(colIn){
	}
	virtual void unparse(std::ostream& out, int indent) = 0;
	size_t line(){ return l; }
	size_t col() { return c; }

	/**
	* Return a string specifying the position this node begins.
	* For some node a position doesn't really make sense (i.e.
	* ProgramNode) but for the rest it's the position in the
	* input file that represents that node
	**/
	std::string pos(){
		return "[" + std::to_string(line()) + ","
		+ std::to_string(col()) + "]";
	}

private:
	size_t l; /// The line at which the node starts in the input file
	size_t c; /// The column at which the node starts in the input file
};

/**
* \class ProgramNode
* Class that contains the entire abstract syntax tree for a program.
* Note the list of declarations encompasses all global declarations
* which includes (obviously) all global variables and struct declarations
* and (perhaps less obviously), all function declarations
**/
class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn)
	: ASTNode(1, 1), myGlobals(globalsIn){
	}
	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode * > * myGlobals;
};



class FormalDeclNode : public ASTNode{
public:
	FormalDeclNode(TypeNode* f_type, IDNode* f_id)
	: ASTNode(0, 0) {
			my_type = f_type;
			my_id = f_id;
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode* my_type;
	IDNode* my_id;
};

class FormalDeclNodeList : public ASTNode{
public:
	FormalDeclNodeList(std::list<FormalDeclNode * >* temp_list)
	: ASTNode(0, 0) {
			my_list = temp_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	std::list<FormalDeclNode * >* my_list;
};
/**  \class ExpNode
* Superclass for expression nodes (i.e. nodes that can be used as
* part of an expression).  Nodes that are part of an expression
* should inherit from this abstract superclass.
**/
class ExpNode : public ASTNode{
protected:
	ExpNode(size_t line, size_t col)
	: ASTNode(line, col){
	}
};

class StmtNode : public ASTNode{
public:
	StmtNode(size_t line, size_t col)
	: ASTNode(line, col){
	}
	void unparse(std::ostream& out, int indent) override = 0;
};

class StmtListNode : public ASTNode{
public:
	StmtListNode(std::list<StmtNode * > * temp_list)
	: ASTNode(0, 0){
		my_list = temp_list;
	}

	void unparse(std::ostream& out, int indent);
private:
	std::list<StmtNode * > * my_list;
};


/** \class DeclNode
* Superclass for declarations (i.e. nodes that can be used to
* declare a struct, function, variable, etc).  This base class will
**/
class DeclNode : public StmtNode{
public:
	DeclNode(size_t line, size_t col)
	: StmtNode(line, col) {
	}
	// void unparse(std::ostream& out, int indent);
};
/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/
class TypeNode : public ASTNode{
protected:
	TypeNode(size_t lineIn, size_t colIn, bool refIn)
	: ASTNode(lineIn, colIn), myIsReference(refIn){
	}
	bool myIsReference;
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
	//TODO: consider adding an isRef to use in unparse to
	// indicate if this is a reference type
// private:
// 	bool myIsReference;
};

/** An identifier. Note that IDNodes subclass
 * ExpNode because they can be used as part of an expression.
**/

class LocNode : public ExpNode{
public:
	LocNode(size_t line, size_t col)
	: ExpNode(line, col){
	}
};

/** All ExpNode **/
class IDNode : public LocNode{
public:
	IDNode(IDToken * token)
	: LocNode(token->line(), token->col()), myStrVal(token->value()){
		myStrVal = token->value();
	}
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	std::string myStrVal;
};

class DotAccessNode : public LocNode{
public:
	DotAccessNode(std::list<IDNode * >* t_m_list)
	: LocNode(0, 0){
		m_list = t_m_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	std::list<IDNode * >* m_list;
};

class StrLitNode : public ExpNode{
public:
	StrLitNode(StrToken * token)
	: ExpNode(token->line(), token->col()), myStrVal(token->str()){
		myStrVal = token->str();
	}
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	std::string myStrVal;
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(IntLitToken * token)
	: ExpNode(token->line(), token->col()), myNumVal(token->num()){
		myNumVal = token->num();
	}
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	int myNumVal;
};

class NullRefNode : public ExpNode{
public:
	NullRefNode(size_t lineIn, size_t colIn)
	: ExpNode(lineIn, colIn){
	}
	void unparse(std::ostream& out, int indent);
};

class NotNode : public ExpNode{
public:
		NotNode(size_t lineIn, size_t colIn, ExpNode* temp_m_exp)
		: ExpNode(lineIn, colIn){
			m_exp = temp_m_exp;
		}
		void unparse(std::ostream& out, int indent);
private:
	ExpNode* m_exp;
};

class NegNode : public ExpNode{
public:
	NegNode(size_t lineIn, size_t colIn, ExpNode* temp_m_exp)
	: ExpNode(lineIn, colIn){
		m_exp = temp_m_exp;
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* m_exp;
};

class FalseNode : public ExpNode{
public:
	FalseNode(size_t lineIn, size_t colIn)
	: ExpNode(lineIn, colIn){
	}
	void unparse(std::ostream& out, int indent);
};

class CallExpNode : public ExpNode{
public:
	CallExpNode(IDNode* token)
	: ExpNode(0, 0){
		myID = token;
		args = nullptr;
	}

	CallExpNode(IDNode* token, std::list<ExpNode * > * temp_args)
	: ExpNode(token->line(), token->col()){
		myID = token;
		args = temp_args;
	}

	void unparse(std::ostream& out, int indent);
private:
	IDNode* myID;
	std::list<ExpNode * > * args;
};

class AssignExpNode: public ExpNode{
public:
	AssignExpNode(LocNode * token, ExpNode* expr)
	: ExpNode(0,0){
		m_loc = token;
		myExpr = expr;
	}
	void unparse(std::ostream& out, int indent);
private:
	LocNode* m_loc;
	ExpNode* myExpr;
};
/** End ExpNode **/

/** All BinaryExpNode **/
class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: ExpNode(line, col){
		lhs = expr1;
		rhs = expr2;
	}
	void unparse(std::ostream& out, int indent);
	virtual std::string op() = 0;

protected:
	ExpNode* lhs;
	ExpNode* rhs;
};

class AndNode : public BinaryExpNode{
public:
	AndNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" && ");
 }
};

class DivideNode : public BinaryExpNode{
public:
	DivideNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" / ");
 }
};

class LessEqNode : public BinaryExpNode{
public:
	LessEqNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" <= ");
 }
};

class LessNode : public BinaryExpNode{
public:
	LessNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" < ");
 }
};

class MinusNode : public BinaryExpNode{
public:
	MinusNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" - ");
 }
};

class NotEqualNode : public BinaryExpNode{
public:
	NotEqualNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" != ");
 }
};

class OrNode : public BinaryExpNode{
public:
	OrNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" || ");
 }
};

class TimesNode : public BinaryExpNode{
public:
	TimesNode(size_t line, size_t col, ExpNode * expr1, ExpNode * expr2)
	: BinaryExpNode(line, col, expr1, expr2){
	}
 std::string op(){
	 return(" * ");
 }
};

/** End BinaryNode **/

/** A variable declaration. Note that this class is intended to
 * represent a global or local variable of any type (including a struct
 * type. Note that this is not intended to represent a declaration of
 * a struct. In other words:
 * struct MyStruct {
 *   int fieldA;
 * };
 * is NOT a VarDeclNode because it introduces a new datatype, not a new
 * variable (in this case, the example is a StructDeclNode).  * However,
 * struct MyStruct instance; *is* a VarDeclNode, since it introduces a
 * new variable to the program.
**/
class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t l, size_t c, TypeNode * type, IDNode * id)
	: DeclNode(type->line(), type->col()), myType(type), myId(id){
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode * myType;
	IDNode * myId;
};

class VarDeclListNode : public DeclNode{
public:
	VarDeclListNode(std::list<VarDeclNode *> * temp_var_list) : DeclNode(0, 0){
	  var_list = temp_var_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	std::list<VarDeclNode *> * var_list;
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
	: TypeNode(lineIn, colIn, isRefIn){
	}
	void unparse(std::ostream& out, int indent);
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t lineIn, size_t colIn, bool isRefIn)
	: TypeNode(lineIn, colIn, isRefIn){
	}
	void unparse(std::ostream& out, int indent);
};

class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t lineIn, size_t colIn)
	: TypeNode(lineIn, colIn, 0){
	}
	void unparse(std::ostream& out, int indent);
};

class StructTypeNode : public TypeNode{
public:
	StructTypeNode(size_t lineIn, size_t colIn, IDNode* temp_s_id, bool isRefIn)
	: TypeNode(lineIn, colIn, isRefIn){
		s_id = temp_s_id;
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode* s_id;
};

/** All StmtNode **/
class AssignStmtNode : public StmtNode {
public:
	AssignStmtNode(AssignExpNode * temp_assignment)
	: StmtNode(0,0){
	assignment = temp_assignment;
	}
	void unparse(std::ostream& out, int indent);

private:
	AssignExpNode * assignment;

};

class CallStmtNode : public StmtNode {
public:
	CallStmtNode(CallExpNode* my_stat)
	: StmtNode(0,0){
	statement = my_stat;
	}
	void unparse(std::ostream& out, int indent);
private:
	CallExpNode* statement;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode()
	: StmtNode(0,0){
	expression = nullptr;
	}
	ReturnStmtNode(ExpNode* temp_expression)
	: StmtNode(0,0){
	expression = temp_expression;
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* expression;
};

class FnBodyNode : public ASTNode{
public:
	FnBodyNode(StmtListNode* body_stmts)
	: ASTNode(0,0){
		fn_body_stmts = body_stmts;
	}
	void unparse(std::ostream& out, int indent);
private:
	StmtListNode* fn_body_stmts;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(TypeNode* temp_fn_type, IDNode* temp_fn_id, FormalDeclNodeList* temp_fn_list, FnBodyNode* temp_fn_body)
	: DeclNode(0, 0){
		fn_type = temp_fn_type;
		fn_id = temp_fn_id;
		fn_list = temp_fn_list;
		fn_body = temp_fn_body;
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode* fn_type;
	IDNode* fn_id;
	FormalDeclNodeList* fn_list;
	FnBodyNode* fn_body;
};

class StructDeclNode : public DeclNode{
public:
	StructDeclNode(IDNode* temp_id, VarDeclListNode* temp_list)
	: DeclNode(0,0){
		s_id = temp_id;
		s_list = temp_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode* s_id;
	VarDeclListNode* s_list;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(ExpNode* temp_expression, StmtListNode* temp_if_list, StmtListNode* temp_else_list)
	: StmtNode(0,0){
		expression = temp_expression;
		if_list = temp_if_list;
		else_list = temp_else_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* expression;
	StmtListNode* if_list;
	StmtListNode* else_list;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(ExpNode* temp_expression, StmtListNode* temp_if_list)
	: StmtNode(0,0){
		expression = temp_expression;
		if_list = temp_if_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* expression;
	StmtListNode* if_list;
};

class InputStmtNode : public StmtNode{
public:
	InputStmtNode(LocNode* temp_str)
	: StmtNode(0,0){
		my_str = temp_str;
	}
	void unparse(std::ostream& out, int indent);
private:
	LocNode* my_str;
};


class OutputStmtNode : public StmtNode{
public:
	OutputStmtNode(ExpNode* temp_expression)
	: StmtNode(0,0){
		expression = temp_expression;
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* expression;
};

class PostDeclStmtNode : public StmtNode{
public:
	PostDeclStmtNode(LocNode * temp_m_loc)
	: StmtNode(0,0){
		m_loc = temp_m_loc;
}
void unparse(std::ostream& out, int indent);
private:
	LocNode* m_loc;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(ExpNode* temp_expression, StmtListNode* temp_w_list)
	: StmtNode(0,0){
		expression = temp_expression;
		w_list = temp_w_list;
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode* expression;
	StmtListNode* w_list;
};


} //End namespace negatron

#endif
