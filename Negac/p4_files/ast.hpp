#ifndef NEGATRON_AST_HPP
#define NEGATRON_AST_HPP

#include <ostream>
#include <sstream>
#include <string.h>
#include <list>
#include "err.hpp"
#include "tokens.hpp"
#include "types.hpp"
#include "3ac.hpp"

namespace negatron {

class TypeAnalysis;

class Opd;

class SymbolTable;
class Symbol;

class DerefNode;
class RefNode;
class DeclListNode;
class StmtListNode;
class FormalsListNode;
class DeclNode;
class VarDeclNode;
class StmtNode;
class AssignExpNode;
class FormalDeclNode;
class TypeNode;
class ExpNode;
class IDNode;

class ASTNode{
public:
	ASTNode(size_t lineIn, size_t colIn)
	: l(lineIn), c(colIn){ }
	virtual void unparse(std::ostream&, int) = 0;
	size_t line() const { return this->l; }
	size_t col() const { return this->c; }
	std::string pos(){
		return "[" + std::to_string(line()) + ","
			+ std::to_string(col()) + "]";
	}
	virtual std::string nodeKind() = 0;
	virtual bool nameAnalysis(SymbolTable *) = 0;
	//Note that there is no ASTNode::typeAnalysis. To allow
	// for different type signatures, type analysis is
	// implemented as needed in various subclasses
private:
	size_t l;
	size_t c;
};

class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn)
	: ASTNode(1,1), myGlobals(globalsIn){}
	virtual std::string nodeKind() override { return "Program"; }
	void unparse(std::ostream&, int) override;
	bool semanticAnalysis(){
		SymbolTable * symTab = new SymbolTable();
		bool namesOK = this->nameAnalysis(symTab);
		TypeAnalysis * typing = new TypeAnalysis();
		this->typeAnalysis(typing);
		return namesOK && typing->passed();
	}
	virtual bool nameAnalysis(SymbolTable *) override;
	virtual void typeAnalysis(TypeAnalysis *);
	IRProgram * to3AC();
	virtual ~ProgramNode(){ }
private:
	std::list<DeclNode *> * myGlobals;
};

class TypeNode : public ASTNode{
public:
	TypeNode(size_t l, size_t c) : ASTNode(l, c){ }
	void unparse(std::ostream&, int) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual const DataType * getDataType() = 0;
	virtual std::string getTypeString();
	virtual bool nameAnalysis(SymbolTable *) override;
};

class StmtNode : public ASTNode{
public:
	StmtNode(size_t lIn, size_t cIn) : ASTNode(lIn, cIn){ }
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fnType) = 0;
	virtual void to3AC(Procedure * proc) = 0;
};

class DeclNode : public StmtNode{
public:
	DeclNode(size_t l, size_t c) : StmtNode(l, c){ }
	void unparse(std::ostream& out, int indent) override =0;
	virtual std::string nodeKind() override = 0;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fnType) override = 0;
	virtual void to3AC(IRProgram * prog) = 0;
	virtual void to3AC(Procedure * proc) = 0;
	virtual const DataType * getDeclaredType() const = 0;
};

class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t lIn, size_t cIn, TypeNode * typeIn, IDNode * IDIn)
	: DeclNode(lIn, cIn), myType(typeIn), myID(IDIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "VarDecl"; }
	IDNode * ID(){ return myID; }
	virtual const DataType * getDeclaredType() const {
		return myType->getDataType(); }
	bool nameAnalysis(SymbolTable * symTab) override;
	void typeAnalysis(TypeAnalysis * typing, FnType * fnType) override;
	virtual void to3AC(Procedure * proc) override;
	virtual void to3AC(IRProgram * prog) override;
	virtual TypeNode * getTypeNode() { return myType; }
protected:
	TypeNode * myType;
	IDNode * myID;
};

class FormalDeclNode : public VarDeclNode{
public:
	FormalDeclNode(size_t lIn, size_t cIn, TypeNode * type, IDNode * id)
	: VarDeclNode(lIn, cIn, type, id){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FormalDecl"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	//virtual void typeAnalysis(TypeAnalysis *, FnType *) <- use superclass
	virtual TypeNode * getTypeNode() { return myType; }
	virtual const DataType * getDeclaredType() const {
		return myType->getDataType(); }
	virtual void to3AC(Procedure * proc) override;
	virtual void to3AC(IRProgram * prog) override;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(size_t lIn, size_t cIn,
	  TypeNode * retTypeIn, IDNode * idIn,
	  std::list<FormalDeclNode *> * formalsIn,
	  std::list<StmtNode *> * bodyIn)
	: DeclNode(lIn, cIn),
	  myID(idIn), myRetType(retTypeIn),
	  myFormals(formalsIn), myBody(bodyIn){ }
	IDNode * ID() const { return myID; }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FnDecl"; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fnType);
	void to3AC(IRProgram * prog) override;
	void to3AC(Procedure * prog) override;
	virtual TypeNode * getTypeNode() {
		return myRetType;
	}
	virtual const DataType * getDeclaredType() const override;
private:
	IDNode * myID;
	TypeNode * myRetType;
	std::list<FormalDeclNode *> * myFormals;
	std::list<StmtNode *> * myBody;
};

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(size_t l, size_t c, AssignExpNode * expIn)
	: StmtNode(l, c), myExp(expIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "AssignStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	AssignExpNode * myExp;
};

class InputStmtNode : public StmtNode{
public:
	InputStmtNode(size_t l, size_t c, IDNode * idIn)
	: StmtNode(l, c), myID(idIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "InputStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	IDNode * myID;
};

class OutputStmtNode : public StmtNode{
public:
	OutputStmtNode(size_t l, size_t c, ExpNode * exp)
	: StmtNode(l, c), myExp(exp){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "OutputStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myExp;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(size_t l, size_t c, IDNode * idIn)
	: StmtNode(l, c), myID(idIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "PostDecStmt"; }
	virtual bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	IDNode * myID;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(size_t l, size_t c, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn)
	: StmtNode(l, c), myCond(condIn), myBody(bodyIn){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IfStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(size_t l, size_t c, ExpNode * condIn,
	  std::list<StmtNode *> * bodyTrueIn,
	  std::list<StmtNode *> * bodyFalseIn)
	: StmtNode(l, c), myCond(condIn),
	  myBodyTrue(bodyTrueIn), myBodyFalse(bodyFalseIn) { }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IfElseStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBodyTrue;
	std::list<StmtNode *> * myBodyFalse;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(size_t l, size_t c, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn)
	: StmtNode(l, c), myCond(condIn), myBody(bodyIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "WhileStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * prog) override;
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(size_t l, size_t c, ExpNode * exp)
	: StmtNode(l, c), myExp(exp){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "ReturnStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * proc) override;
private:
	ExpNode * myExp;
};


class ExpNode : public ASTNode{
public:
	ExpNode(size_t lIn, size_t cIn) : ASTNode(lIn, cIn){ }
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;
	virtual void typeAnalysis(TypeAnalysis *) = 0;
	virtual Opd * flatten(Procedure * proc) = 0;
};

class IDNode : public ExpNode{
public:
	IDNode(size_t lIn, size_t cIn, std::string nameIn)
	: ExpNode(lIn, cIn), name(nameIn){}
	std::string getName(){ return name; }
	virtual std::string nodeKind() override { return "ID"; }
	void unparse(std::ostream& out, int indent) override;
	void attachSymbol(Symbol * symbolIn);
	Symbol * getSymbol() const;
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *);
	virtual Opd * flatten(Procedure * proc) override;
private:
	std::string name;
	Symbol * mySymbol = nullptr;
};

class CallExpNode : public ExpNode{
public:
	CallExpNode(size_t l, size_t c, IDNode * id,
	  std::list<ExpNode *> * argsIn)
	: ExpNode(l, c), myID(id), myArgs(argsIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "CallExp"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	void typeAnalysis(TypeAnalysis *) override;
	DataType * getRetType();
	bool isVoid(){
		const DataType * my_type = myID->getSymbol()->getType();
		const FnType * fnType = my_type->asFn();
		const DataType* my_return = fnType->getReturnType();
		return(my_return->isVoid());
	}
	virtual Opd * flatten(Procedure * proc) override;
private:
	IDNode * myID;
	std::list<ExpNode *> * myArgs;
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(size_t lIn, size_t cIn, ExpNode * lhs, ExpNode * rhs)
	: ExpNode(lIn, cIn), myExp1(lhs), myExp2(rhs) { }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override = 0;
	virtual Opd * flatten(Procedure * prog) override = 0;
protected:
	ExpNode * myExp1;
	ExpNode * myExp2;
	void binaryLogicTyping(TypeAnalysis * typing);
	void binaryEqTyping(TypeAnalysis * typing);
	void binaryRelTyping(TypeAnalysis * typing);
	void binaryMathTyping(TypeAnalysis * typing);
};

class MinusNode : public BinaryExpNode{
public:
	MinusNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Minus"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class TimesNode : public BinaryExpNode{
public:
	TimesNode(size_t l, size_t c, ExpNode * e1In, ExpNode * e2In)
	: BinaryExpNode(l, c, e1In, e2In){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Times"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class DivideNode : public BinaryExpNode{
public:
	DivideNode(size_t lIn, size_t cIn, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(lIn, cIn, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Divide"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class AndNode : public BinaryExpNode{
public:
	AndNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "And"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class OrNode : public BinaryExpNode{
public:
	OrNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Or"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class NotEqualsNode : public BinaryExpNode{
public:
	NotEqualsNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "NotEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;

};

class LessNode : public BinaryExpNode{
public:
	LessNode(size_t lineIn, size_t colIn,
		ExpNode * exp1, ExpNode * exp2)
	: BinaryExpNode(lineIn, colIn, exp1, exp2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "NotEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
};

class LessEqNode : public BinaryExpNode{
public:
	LessEqNode(size_t l, size_t c, ExpNode * e1, ExpNode * e2)
	: BinaryExpNode(l, c, e1, e2){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "LessEq"; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class UnaryExpNode : public ExpNode {
public:
	UnaryExpNode(size_t lIn, size_t cIn, ExpNode * expIn)
	: ExpNode(lIn, cIn){
		this->myExp = expIn;
	}
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual bool nameAnalysis(SymbolTable * symTab) override = 0;
	virtual void typeAnalysis(TypeAnalysis *) override = 0;
	virtual Opd * flatten(Procedure * prog) override = 0;
protected:
	ExpNode * myExp;
};

class NotNode : public UnaryExpNode{
public:
	NotNode(size_t lIn, size_t cIn, ExpNode * exp)
	: UnaryExpNode(lIn, cIn, exp){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Not"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class NegNode : public UnaryExpNode{
public:
	NegNode(size_t l, size_t c, ExpNode * exp)
	: UnaryExpNode(l, c, exp){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Neg"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t l, size_t c) : TypeNode(l, c){}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "VoidType"; }
	virtual const DataType * getDataType() override;
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t l, size_t c) : TypeNode(l, c){}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "IntType"; }
	virtual const DataType * getDataType() override;
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t l, size_t c): TypeNode(l, c) { }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "BoolType"; }
	virtual const DataType * getDataType() override;
};

class AssignExpNode : public ExpNode{
public:
	AssignExpNode(size_t l, size_t c, IDNode * dstIn, ExpNode * srcIn)
	: ExpNode(l, c), myDst(dstIn), mySrc(srcIn){ }
	void unparse(std::ostream& out, int indent) override;
	void unparseTop(std::ostream& out, int indent);
	virtual std::string nodeKind() override { return "AssignExp"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
private:
	ExpNode * myDst;
	ExpNode * mySrc;
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(size_t l, size_t c, const int numIn)
	: ExpNode(l, c), myNum(numIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "IntLit"; }
	bool nameAnalysis(SymbolTable * symTab) override { return true; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
private:
	const int myNum;
};

class StrLitNode : public ExpNode{
public:
	StrLitNode(size_t l, size_t c, const std::string strIn)
	: ExpNode(l, c), myStr(strIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "StrLit"; }
	bool nameAnalysis(SymbolTable *) override { return true; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * proc) override;
private:
	 const std::string myStr;
};

class FalseNode : public ExpNode{
public:
	FalseNode(size_t l, size_t c): ExpNode(l, c){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "False"; }
	bool nameAnalysis(SymbolTable * symTab) override { return true; }
	virtual void typeAnalysis(TypeAnalysis *) override;
	virtual Opd * flatten(Procedure * prog) override;
};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(size_t l, size_t c, CallExpNode * expIn)
	: StmtNode(l, c), myCallExp(expIn){ }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "CallStmt"; }
	bool nameAnalysis(SymbolTable * symTab) override;
	virtual void typeAnalysis(TypeAnalysis *, FnType * fn) override;
	virtual void to3AC(Procedure * proc) override;
private:
	CallExpNode * myCallExp;
};


} //End namespace negatron

#endif
