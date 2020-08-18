#ifndef NEGATRON_AST_HPP
#define NEGATRON_AST_HPP

#include <iostream>
#include <list>
#include <typeinfo>

#include "symbol_table.hpp"

namespace negatron {
	class IDNode;
	class DeclNode;
	class StmtNode;
	class TypeNode;
	class LocNode;
	class ExpNode;
	class AssignExpNode;
	class CallExpNode;

class ASTNode{
public:
	ASTNode(size_t lIn, size_t cIn) : l(lIn),c(cIn){}
	virtual void unparse(std::ostream&, int) = 0;
	size_t line();
	size_t col();
	std::string pos(){
		return "[" + std::to_string(line()) + ","
			+ std::to_string(col()) + "]" ;
	}
	virtual std::string nodeKind() = 0;
private:
	size_t l; // Node's line position in the input
	size_t c; // Node's column position in the input
};

class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn)
	: ASTNode(1,1), myGlobals(globalsIn){
	}
	bool semanticAnalysis();
	virtual std::string nodeKind() override { return "Program"; }
	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode *> * myGlobals;
};

class StmtNode : public ASTNode{
public:
	StmtNode(size_t l, size_t c) : ASTNode(l, c){}
	void unparse(std::ostream& out, int indent) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual void nameAnalysis(SymbolTable& symTab) = 0;
	virtual void typeAnalysis(std::string RetType, bool& error_happened) = 0;
};

class DeclNode : public StmtNode{
public:
	DeclNode(size_t lIn, size_t cIn);
	void unparse(std::ostream& out, int indent) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual void nameAnalysis(SymbolTable& symTab) = 0;
};

class VarDeclNode : public DeclNode{
public:
	VarDeclNode(size_t lIn,size_t cIn,TypeNode * typeIn,IDNode * IDIn);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "VarDecl"; }
	virtual void nameAnalysis(SymbolTable& symTab);
	virtual void typeAnalysis(std::string RetType, bool& error_happened){}
	IDNode * ID(){ return myID; }
	std::string getType();
protected:
	TypeNode * myType;
	IDNode * myID;
};


class FormalDeclNode : public VarDeclNode{
public:
	FormalDeclNode(size_t lIn, size_t cIn,
	  TypeNode * typeIn, IDNode * IDIn);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FormalDecl"; }
};

class StructDeclNode : public DeclNode{
public:
	StructDeclNode(size_t l, size_t c, IDNode * id,
	  std::list<VarDeclNode * > * fields);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "StructDecl"; }
	void nameAnalysis(SymbolTable& symTab) override;
	void typeAnalysis(std::string RetType, bool& error_happened){}
private:
	std::list<VarDeclNode *> * myFields;
	IDNode * myID;
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(size_t lIn, size_t cIn,
	  TypeNode * retTypeIn, IDNode * IDIn,
	  std::list<FormalDeclNode *> * formalsIn,
	  std::list<StmtNode *> * bodyIn)
	: DeclNode(lIn, cIn),
	  myID(IDIn), myRetType(retTypeIn),
	  myFormals(formalsIn), myBody(bodyIn){
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "FnDecl"; }
	virtual void nameAnalysis(SymbolTable& symTab);
	virtual void typeAnalysis(std::string RetType, bool& error_happened);
private:
	IDNode * myID;
	TypeNode * myRetType;
	std::list<FormalDeclNode *> * myFormals;
	std::list<StmtNode *> * myBody;
};

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(size_t l, size_t c, AssignExpNode * expIn);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "AssignStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	AssignExpNode * myExp;
};

class InputStmtNode : public StmtNode{
public:
	InputStmtNode(size_t l, size_t c, LocNode * expIn);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "InputStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	LocNode * myLoc;
};

class OutputStmtNode : public StmtNode{
public:
	OutputStmtNode(size_t l, size_t c, ExpNode * expIn);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "OutputStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	ExpNode * myExp;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(size_t l, size_t c, LocNode * locIn);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "PostDecStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	LocNode * myLoc;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(size_t l, size_t c, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IfStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class IfElseStmtNode : public StmtNode {
public:
	IfElseStmtNode(size_t l, size_t c,
	  ExpNode * condIn,
	  std::list<StmtNode *> * bodyTrueIn,
	  std::list<StmtNode *> * bodyFalseIn
	);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IfElseStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBodyTrue;
	std::list<StmtNode *> * myBodyFalse;
};

class WhileStmtNode : public StmtNode {
public:
	WhileStmtNode(size_t l, size_t c, ExpNode * condIn,
	  std::list<StmtNode *> * bodyIn);
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "WhileStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	ExpNode * myCond;
	std::list<StmtNode *> * myBody;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(size_t l, size_t c, ExpNode * expIn);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "ReturnStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	ExpNode * myExp;

};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(size_t l, size_t c, CallExpNode * expIn);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "CallStmt"; }
	void nameAnalysis(SymbolTable& symTab);
	void typeAnalysis(std::string RetType, bool& error_happened);
private:
	CallExpNode * myCallExp;
};

class ExpNode : public ASTNode{
public:
	ExpNode(size_t l, size_t c) : ASTNode(l,c){}
	virtual void unparse(std::ostream& out, int indent) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual void nameAnalysis(SymbolTable& symTab){}
	virtual bool typeAnalysis(std::string my_parent, bool& error_happened){return false;}
	virtual std::string typeAnalysis2(bool& error_happened){ return "none"; }
	virtual std::string getExpKind(){return "none";}
	virtual std::string getExpType(){return "none";}
};

class LocNode : public ExpNode {
public:
	LocNode(size_t l, size_t c);
	virtual std::string nodeKind() override = 0;
	virtual Symbol* getSymbol(){return nullptr;};
};


class IDNode : public LocNode{
public:
	IDNode(size_t lIn, size_t cIn, std::string nameIn)
	: LocNode(lIn, cIn), name(nameIn){ }
	std::string getName(){ return name; }
	virtual std::string nodeKind() override { return "ID"; }
	void unparse(std::ostream& out, int indent) override;
	void setSymbol(Symbol * symbolIn){ mySymbol = symbolIn; }
	Symbol* getSymbol(){ return mySymbol; }
	void nameAnalysis(SymbolTable& symTab);
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string getExpKind();
	std::string getExpType(){
		return (mySymbol->getType());
	}
	std::string typeAnalysis2(bool& error_happened);
private:
	std::string name;
	Symbol * mySymbol = nullptr;
};

class CallExpNode : public ExpNode {
public:
	CallExpNode(size_t l, size_t c, IDNode * IDIn,
	std::list<ExpNode *> * argsIn)
	: ExpNode(l,c), myID(IDIn), myArgs(argsIn){
	}
	std::string nodeKind() override { return "CallExp"; }
	void unparse(std::ostream& out, int indent) override;
	void nameAnalysis(SymbolTable& symTab);
	bool IsReturnVoid(){
		std::string RetType = (myID->getSymbol())->getType();
		if(RetType == "void"){
			return(1);
		}
		else{
			return(0);
		}
	}
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){
		typeAnalysis(this->nodeKind(), error_happened);
		if((myID->getSymbol()) == nullptr){
			return "nullptr";
		}
		return((myID->getSymbol())->getType());
	}
private:
	IDNode * myID;
	std::list<ExpNode *> * myArgs;
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(size_t l, size_t c, ExpNode * lhs, ExpNode *rhs)
	: ExpNode(l, c), myExp1(lhs), myExp2(rhs){
	}
	virtual void nameAnalysis(SymbolTable& symTab){
		myExp1->nameAnalysis(symTab);
		myExp2->nameAnalysis(symTab);
	}
	virtual bool typeAnalysis(std::string my_parent, bool& error_happened) = 0;
	std::string typeAnalysis2(bool& error_happened){ typeAnalysis(nodeKind(), error_happened);
															 return "bool"; }
protected:
	ExpNode * myExp1;
	ExpNode * myExp2;
};

class MinusNode : public BinaryExpNode {
public:
	MinusNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Minus"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){ typeAnalysis(nodeKind(), error_happened);
															 return "int"; }
};

class TimesNode : public BinaryExpNode {
public:
	TimesNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Times"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){ typeAnalysis(nodeKind(), error_happened);
															 return "int"; }
};

class DivideNode : public BinaryExpNode {
public:
	DivideNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Divide"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){ typeAnalysis(nodeKind(), error_happened);
															 return "int"; }
};

class AndNode : public BinaryExpNode {
public:
	AndNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "And"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
};

class OrNode : public BinaryExpNode {
public:
	OrNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Or"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
};

class NotEqualsNode : public BinaryExpNode {
public:
	NotEqualsNode(size_t l, size_t c, ExpNode * e1In, ExpNode * e2In)
	: BinaryExpNode(l, c, e1In, e2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "NotEq"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
};

class LessNode : public BinaryExpNode {
public:
	LessNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Less"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
};



class LessEqNode : public BinaryExpNode {
public:
	LessEqNode(size_t l, size_t c, ExpNode * exp1In, ExpNode * exp2In)
	: BinaryExpNode(l, c, exp1In, exp2In){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "LessEq"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened);
};

class NotNode : public ExpNode {
public:
	NotNode(size_t l, size_t c, ExpNode * expIn)
	: ExpNode(l, c), myExp(expIn){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Not"; }
	void nameAnalysis(SymbolTable& symTab){
		myExp->nameAnalysis(symTab);
	}
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){ typeAnalysis(nodeKind(), error_happened);
															 return "bool"; }
private:
	ExpNode * myExp;
};

class NegNode : public ExpNode {
public:
	NegNode(size_t l, size_t c, ExpNode * expIn)
	: ExpNode(l, c), myExp(expIn){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "Neg"; }
	void nameAnalysis(SymbolTable& symTab){
		myExp->nameAnalysis(symTab);
	}
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){ typeAnalysis(nodeKind(), error_happened);
															 return "int"; }
private:
	ExpNode * myExp;
};

class TypeNode : public ASTNode{
public:
	TypeNode(size_t lIn, size_t cIn, bool isRefIn)
	: ASTNode(lIn, cIn), myIsRef(isRefIn){
	}
	bool isRef(){ return myIsRef; }
	void unparse(std::ostream& out, int indent) override = 0;
	virtual std::string nodeKind() override = 0;
	virtual std::string baseType(){ return ""; }
private:
	bool myIsRef;
};

class VoidTypeNode : public TypeNode{
public:
	VoidTypeNode(size_t lIn, size_t cIn): TypeNode(lIn, cIn, false){
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override {
		return "VoidType";
	}
	std::string baseType() override { return "void"; }
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(size_t lIn, size_t cIn, bool isRefIn)
	: TypeNode(lIn, cIn, isRefIn){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "IntType"; }
	std::string baseType() override { return "int"; }
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(size_t lIn, size_t cIn, bool isRefIn)
	: TypeNode(lIn, cIn, isRefIn){
	}
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "BoolType"; }
	std::string baseType() override { return "bool"; }
};

class StructTypeNode : public TypeNode{
public:
	StructTypeNode(size_t lIn, size_t cIn, IDNode * structID,
	  bool isRefIn);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return baseType(); }
	std::string baseType() override {
		return "struct " + myStructID->getName();
	}
private:
	IDNode * myStructID;
};

class AssignExpNode : public ExpNode{
public:
	AssignExpNode(size_t l, size_t c, LocNode * dstIn, ExpNode * srcIn);
	void unparse(std::ostream& out, int indent) override;
	void unparseTop(std::ostream& out, int indent);
	std::string nodeKind() override { return "AssignExp"; }
	void nameAnalysis(SymbolTable& symTab);
	std::string getExpType(){
		return(myDst->getExpType());
	}
	bool typeAnalysis(std::string my_parent, bool& error_happened);
	std::string typeAnalysis2(bool& error_happened){
		typeAnalysis(this->nodeKind(), error_happened);
		return(myDst->getExpType());
	}
private:
	LocNode * myDst;
	ExpNode * mySrc;
};

class DotAccessNode : public LocNode{
public:
	DotAccessNode(size_t l, size_t c, LocNode * baseIn,
	  IDNode *fieldIn);
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "DotAccess"; }
	void nameAnalysis(SymbolTable& symTab);
	Symbol* getSymbol(){
		return(myField->getSymbol());
	}
	std::string typeAnalysis2(bool& error_happened){
		if((myField->getSymbol()) == nullptr){
			return "nullptr";
		}
		return ((myField->getSymbol())->getType());
	}
private:
	LocNode * myBase;
	IDNode * myField;
};

class IntLitNode : public ExpNode{
public:
	IntLitNode(size_t l, size_t c, const int numIn)
	: ExpNode(l, c), myNum(numIn){
	}
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "IntLit"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened){ return(1); }
	std::string typeAnalysis2(bool& error_happened){
		return "int";
	}
private:
	const int myNum;
};

class StrLitNode : public ExpNode {
public:
	StrLitNode(size_t l, size_t c, const std::string strIn)
	: ExpNode(l, c), myStr(strIn){ }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "StrLit"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened){ return(1); }
	std::string typeAnalysis2(bool& error_happened){
		return "string";
	}
private:
	const std::string myStr;
};

class FalseNode : public ExpNode {
public:
	FalseNode(size_t l, size_t c) : ExpNode(l, c) { }
	void unparse(std::ostream& out, int indent) override;
	virtual std::string nodeKind() override { return "False"; }
	bool typeAnalysis(std::string my_parent, bool& error_happened){ return(1); }
	std::string typeAnalysis2(bool& error_happened){
		return "bool";
	}
};

class NullRefNode : public ExpNode {
public:
	NullRefNode(size_t l, size_t c) : ExpNode(l, c) { }
	void unparse(std::ostream& out, int indent) override;
	std::string nodeKind() override { return "NullRef"; }
};

}

#endif
