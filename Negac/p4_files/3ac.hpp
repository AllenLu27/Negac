#ifndef NEGATRON_3AC_HPP
#define NEGATRON_3AC_HPP

#include "list"
#include "map"
#include "err.hpp"
#include "symbol_table.hpp"

namespace negatron{

class Procedure;
class IRProgram;

class Label{
public:
	Label(std::string nameIn){
		this->name = nameIn;
	}
	std::string toString(){
		return "lbl_" + this->name;
	}
private:
	std::string name;
};

enum OpdType{
	STRING, NUMERIC
};

class Opd{
public:
	virtual std::string toString() = 0;
	virtual OpdType getType() = 0;
};

class SymOpd : public Opd{
public:
	std::string toString() override{ 
		return mySym->getName();
	}
	const Symbol * getSym(){ return mySym; }
	virtual OpdType getType() override {
		return NUMERIC;
	}
private:
	SymOpd(Symbol * sym) : mySym(sym) {} 
	Symbol * mySym;
	friend class Procedure;
	friend class IRProgram;
};

class LitOpd : public Opd{
public:
	LitOpd(std::string valIn) : val(valIn){ }
	std::string toString() override{
		return val;
	}
	virtual OpdType getType() override {
		// I recommend represent string
		// literals as AuxOpd, making litOpds
		// always numeric (int or bool)
		return NUMERIC;
	}
private:
	std::string val;
};

class AuxOpd : public Opd{
public:
	AuxOpd(std::string valIn, OpdType typeIn) 
	: val(valIn), myType(typeIn) { }
	std::string toString() override{
		return val;
	}
	virtual OpdType getType() override {
		return myType;
	}
private:
	std::string val;
	OpdType myType;
};

enum BinOp {
	SUB, DIV, MULT, OR, AND, NEQ, LT, LTE
};
enum UnaryOp{
	NEG, NOT
};
enum Syscall {
	OUTPUT, INPUT, EXIT
};

class Quad{
public:
	Quad();
	void addLabel(Label * label);
	virtual std::string repr() = 0;
	std::string commentStr();
	virtual std::string toString(bool verbose=false);
	void setComment(std::string commentIn);
private:
	std::string myComment;
	std::list<Label *> labels;
};

class BinOpQuad : public Quad{
public:
	BinOpQuad(Opd * dstIn, BinOp opIn, Opd * src1In, Opd * src2In);
	std::string repr() override;
private:
	Opd * dst;
	BinOp op;
	Opd * src1;
	Opd * src2;
};

class UnaryOpQuad : public Quad {
public:
	UnaryOpQuad(Opd * dstIn, UnaryOp opIn, Opd * srcIn);
	std::string repr() override ;
private:
	Opd * dst;
	UnaryOp op;
	Opd * src;
};

class AssignQuad : public Quad{
	
public:
	AssignQuad(Opd * dstIn, Opd * srcIn)
	: dst(dstIn), src(srcIn)
	{ }
	std::string repr() override;

private:
	Opd * dst;
	Opd * src;
};

class LocQuad : public Quad {
public:
	LocQuad(Opd * srcIn, Opd * tgtIn);
	std::string repr() override;
private:
	Opd * src;
	Opd * tgt;
};

class JmpQuad : public Quad {
public:
	JmpQuad(Label * tgtIn);
	std::string repr() override;
private:
	Label * tgt;
};

class JmpIfQuad : public Quad {
public:
	JmpIfQuad(Opd * cndIn, bool invertIn, Label * tgtIn);
	std::string repr() override;
private:
	Opd * cnd;
	bool invert;
	Label * tgt;
};

class NopQuad : public Quad {
public:
	NopQuad();
	std::string repr() override;
};

class SyscallQuad : public Quad {
public:
	SyscallQuad(Syscall syscall, Opd * arg);
	std::string repr() override;
private:
	Opd * myArg;
	Syscall mySyscall;
};

class CallQuad : public Quad{
public:
	CallQuad(Symbol * calleeIn);
	std::string repr() override;
private:
	Symbol * callee;
};

class EnterQuad : public Quad{
public:
	EnterQuad(Procedure * proc);
	virtual std::string repr();
private:
	Procedure * myProc;
};

class LeaveQuad : public Quad{
public:
	LeaveQuad(Procedure * proc);
	virtual std::string repr() override;
private:
	Procedure * myProc;
};

class SetInQuad : public Quad{
public:
	SetInQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class GetInQuad : public Quad{
public:
	GetInQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class SetOutQuad : public Quad{
public:
	SetOutQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class GetOutQuad : public Quad{
public:
	GetOutQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
private:
	size_t index;
	Opd * opd;
};

class Procedure{
public:
	Procedure(IRProgram * prog, std::string name);
	void addQuad(Quad * quad);
	Quad * popQuad();
	IRProgram * getProg();
	negatron::Label * makeLabel();

	void gatherLocal(Symbol * sym);
	void gatherFormal(Symbol * sym);
	SymOpd * getSymOpd(Symbol * sym);
	AuxOpd * makeTmp();

	std::string toString(bool verbose=false); 
	std::string getName();

	negatron::Label * getLeaveLabel();
private:
	EnterQuad * enter;
	Quad * leave;
	Label * leaveLabel;

	IRProgram * myProg;
	std::map<Symbol *, SymOpd *> locals;
	std::list<Opd *> temps; 
	std::list<SymOpd *> formals; 
	std::list<Quad *> bodyQuads;
	std::string myName;
	size_t maxTmp;
};

class IRProgram{
public:
	Procedure * makeProc(std::string name);
	Label * makeLabel();
	Opd * makeString(std::string val);
	void gatherGlobal(Symbol * sym);
	SymOpd * getGlobal(Symbol * sym);

	std::string toString(bool verbose=false);
private:
	size_t max_label = 0;
	size_t str_idx = 0;
	std::list<Procedure *> procs; 
	HashMap<AuxOpd *, std::string> strings;
	std::map<Symbol *, SymOpd *> globals;
};

}

#endif 
