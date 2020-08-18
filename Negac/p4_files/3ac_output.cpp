#include "ast.hpp"

namespace negatron{

IRProgram * ProgramNode::to3AC(){
	IRProgram * prog = new IRProgram();
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	Procedure* my_proc = prog->makeProc(myID->getName());
	size_t counter = 1;
	Opd* src = nullptr;
	for(auto var : *myFormals){
		var->to3AC(my_proc);
		src = var->ID()->flatten(my_proc);
		my_proc->addQuad(new GetInQuad(counter, src));
		counter++;
	}
	for(auto stmt : *myBody){
		stmt->to3AC(my_proc);
	}
}

void FnDeclNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void FormalDeclNode::to3AC(IRProgram * prog){
	TODO(Implement me)
}

void FormalDeclNode::to3AC(Procedure * proc){
	Symbol * sym = ID()->getSymbol();
	proc->gatherFormal(sym);
}

Opd * IntLitNode::flatten(Procedure * proc){
	return new LitOpd(std::to_string(myNum));
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * FalseNode::flatten(Procedure * prog){
	LitOpd* my_opd = new LitOpd(std::to_string(0));
	return my_opd;
}

Opd * AssignExpNode::flatten(Procedure * proc){
	Opd* dst = myDst->flatten(proc);
	Opd* src = mySrc->flatten(proc);
	if(mySrc->nodeKind() == "CallExp"){
		proc->addQuad(new GetOutQuad(1, src));
	}
	AssignQuad* my_quad = new AssignQuad(dst, src);
	proc->addQuad(my_quad);
	return(dst);
}

Opd * CallExpNode::flatten(Procedure * proc){
	size_t counter = 1;
	for(auto arg: *myArgs){
		Opd* src = arg->flatten(proc);
		proc->addQuad(new SetInQuad(counter, src));
		counter++;
	}
	proc->addQuad(new CallQuad(myID->getSymbol()));
	if(isVoid()){
		return(nullptr);
	}
	else{
				AuxOpd* my_temp = proc->makeTmp();
				return my_temp;
			}
}

Opd * NegNode::flatten(Procedure * proc){
	Opd* my_opd = myExp->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new UnaryOpQuad(my_temp, NEG, my_opd));
	return my_temp;
}

Opd * NotNode::flatten(Procedure * proc){
	Opd* my_opd = myExp->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new UnaryOpQuad(my_temp, NOT, my_opd));
	return my_temp;
}

Opd * MinusNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, SUB, src1, src2));
	return my_temp;
}

Opd * TimesNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, MULT, src1, src2));
	return my_temp;
}

Opd * DivideNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, DIV, src1, src2));
	return my_temp;
}

Opd * AndNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, AND, src1, src2));
	return my_temp;
}

Opd * OrNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, OR, src1, src2));
	return my_temp;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, NEQ, src1, src2));
	return my_temp;
}

Opd * LessNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, LT, src1, src2));
	return my_temp;
}

Opd * LessEqNode::flatten(Procedure * proc){
	Opd* src1 = myExp1->flatten(proc);
	Opd* src2 = myExp2->flatten(proc);
	AuxOpd* my_temp = proc->makeTmp();
	proc->addQuad(new BinOpQuad(my_temp, LTE, src1, src2));
	return my_temp;
}

void AssignStmtNode::to3AC(Procedure * proc){
	myExp->flatten(proc);
}

void PostDecStmtNode::to3AC(Procedure * proc){
	Opd* src = myID->flatten(proc);
	Opd* src2 = new LitOpd(std::to_string(1));
	proc->addQuad(new BinOpQuad(src, SUB, src, src2));
}

void InputStmtNode::to3AC(Procedure * proc){
	Opd* src = myID->flatten(proc);
	proc->addQuad(new SyscallQuad(INPUT, src));
}

void OutputStmtNode::to3AC(Procedure * proc){
	Opd* src = myExp->flatten(proc);
	proc->addQuad(new SyscallQuad(OUTPUT, src));
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd* my_opd = myCond->flatten(proc);
	Label* my_label = proc->makeLabel();
	proc->addQuad(new JmpIfQuad(my_opd, 0, my_label));
	for(auto stmt: *myBody){
		stmt->to3AC(proc);
	}
	Quad* my_quad = new NopQuad();
	my_quad->addLabel(my_label);
	proc->addQuad(my_quad);
}

void IfElseStmtNode::to3AC(Procedure * proc){
	Opd* my_opd = myCond->flatten(proc);
	Label* my_label1 = proc->makeLabel();
	Label* my_label2 = proc->makeLabel();
	Quad* my_quad = new JmpIfQuad(my_opd, 0, my_label1);
	proc->addQuad(my_quad);
	for(auto stmt: *myBodyTrue){
		stmt->to3AC(proc);
	}
	proc->addQuad(new JmpQuad(my_label2));
	Quad* my_quad1 = new NopQuad();
	my_quad1->addLabel(my_label1);
	proc->addQuad(my_quad1);
	for(auto stmt: *myBodyFalse){
		stmt->to3AC(proc);
	}
	Quad* my_quad2 = new NopQuad();
	my_quad2->addLabel(my_label2);
	proc->addQuad(my_quad2);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Label* my_label1 = proc->makeLabel();
	Quad* nop_quad = new NopQuad();
	nop_quad->addLabel(my_label1);
	proc->addQuad(nop_quad);
	Opd* my_opd = myCond->flatten(proc);
	Label* my_label2 = proc->makeLabel();
	proc->addQuad(new JmpIfQuad(my_opd, 0, my_label2));
	for(auto stmt: *myBody){
		stmt->to3AC(proc);
	}
	proc->addQuad(new JmpQuad(my_label1));
	Quad* my_quad1 = new NopQuad();
	my_quad1->addLabel(my_label2);
	proc->addQuad(my_quad1);
}

void CallStmtNode::to3AC(Procedure * proc){
	myCallExp->flatten(proc);
}

void ReturnStmtNode::to3AC(Procedure * proc){
if(myExp == nullptr){
	proc->addQuad(new JmpQuad(proc->getLeaveLabel()));
}
else{
			Opd* my_opd = myExp->flatten(proc);
			proc->addQuad(new SetOutQuad(1, my_opd));
			proc->addQuad(new JmpQuad(proc->getLeaveLabel()));
		 }
}

void VarDeclNode::to3AC(Procedure * proc){
	//This version of the function should be called when variables
	// are declared in a function body (i.e. there is a
	// "parent" Prodecure).
	Symbol * sym = ID()->getSymbol();
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	//This version of the function is called when variables
	// are declared in the global scope (i.e. there is no
	// "parent" Prodecure.
	Symbol * sym = ID()->getSymbol();
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd * IDNode::flatten(Procedure * proc){
	return proc->getSymOpd(mySymbol);
}

}
