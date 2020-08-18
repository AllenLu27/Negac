#include <ostream>
#include "3ac.hpp"
#include "err.hpp"

namespace negatron{

void IRProgram::allocGlobals(){
	for (std::map<SemSymbol *, SymOpd *>::iterator it=globals.begin(); it!=globals.end(); ++it){
	  std::string loc = "global_";
		loc = loc + it->first->getName();
    it->second->setMemoryLoc(loc);
  }
	for (auto str : strings){
	  std::string loc = "str_";
		loc = loc + str.first->getName();
    str.first->setMemoryLoc(loc);
  }
}

void IRProgram::datagenX64(std::ostream& out){
	out<<".data"<<'\n';
	for (std::map<SemSymbol *, SymOpd *>::iterator it=globals.begin(); it!=globals.end(); ++it){
	  std::string loc = it->second->getMemoryLoc();
		out<< loc <<": .quad 0"<<'\n'<<'\n';
  }
  for (auto str : strings){
	  std::string loc = str.first->getMemoryLoc();
    out<< loc << ": .asciz "<< str.second <<'\n'<<'\n';
  }
	out<<".align 8"<<'\n';
	out<<'\n'<<'\n'<<".text"<<'\n';
	out<<".globl main"<<'\n';
}

void IRProgram::toX64(std::ostream& out){
	allocGlobals();
	datagenX64(out);
	for (auto proc : procs){
		proc->toX64(out);
	}
}

void Procedure::allocLocals(){
	signed int offset= -24;
	for (auto local : locals){
		std::string loc = std::to_string(offset);
		loc += "(%rbp)";
		local.second->setMemoryLoc(loc);
		offset = offset - 8;
	}
	for(auto formal: formals){
		std::string loc = std::to_string(offset);
	  loc += "(%rbp)";
		formal->setMemoryLoc(loc);
		offset = offset - 8;
	}
	for(auto temp: temps){
		std::string loc = std::to_string(offset);
	  loc += "(%rbp)";
		temp->setMemoryLoc(loc);
		offset = offset - 8;
	}
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();
	if(myName == "main"){
		out << myName << ":" << "\n";
	}
	else{
			out << "fun_" << myName << ":" << "\n";
		}

  size_t loc_size = localsSize() * 8;
	size_t temp_size = numTemps() * 8;
	size_t f_size = (this->formals.size()) * 8;
	size_t total_bytes = loc_size + temp_size + f_size;
	size_t formal_begin = loc_size + temp_size + 16;
	enter->codegenX64(out);
	out<<"subq $"<<total_bytes<<", %rsp"<<'\n';

	f_size = f_size - 8;
	//GetIn
	for(auto formal: formals){
		std::string loc = std::to_string(f_size);
		loc += "(%rbp)";
		out<<"movq "<< loc <<", %rax"<<'\n';
		formal->genStore(out, "%rax");
		f_size = f_size - 8;
	}

	for (auto quad : bodyQuads){
		quad->codegenLabels(out);
		quad->codegenX64(out);
	}
	leave->codegenLabels(out);
	out<<"addq $"<<total_bytes<<", %rsp"<<'\n';
	leave->codegenX64(out);
	out<<'\n'<<'\n'<<'\n';
}

void Quad::codegenLabels(std::ostream& out){
	if (labels.empty()){ return; }

	size_t numLabels = labels.size();
	size_t labelIdx = 0;
	for ( Label * label : labels){
		out << label->toString() << ": ";
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
	}
}

void BinOpQuad::codegenX64(std::ostream& out){
	if(op == SUB){
		src1->genLoad(out, "%r8");
		src2->genLoad(out, "%r9");
		out<<"subq %r9, %r8"<<'\n';
		dst->genStore(out, "%r8");
	}
	else if(op == DIV){
		out<<"movq $0, %rdx"<<'\n';
		src1->genLoad(out, "%rax");
		src2->genLoad(out, "%r8");
		out<<"idivq %r8"<<'\n';
		dst->genStore(out, "%rax");
	}
	else if(op == MULT){
		src1->genLoad(out, "%rax");
		src2->genLoad(out, "%r8");
		out<<"imulq %r8"<<'\n';
		dst->genStore(out, "%rax");
	}
	else if(op == OR){
		src1->genLoad(out, "%r8");
		src2->genLoad(out, "%r9");
		out<<"orq %r8, %r9"<<'\n';
		dst->genStore(out, "%r9");
	}
	else if(op == AND){
		src1->genLoad(out, "%r8");
		src2->genLoad(out, "%r9");
		out<<"andq %r8, %r9"<<'\n';
		dst->genStore(out, "%r9");
	}
	else if(op == NEQ){
		src1->genLoad(out, "%r8");
		src2->genLoad(out, "%r9");
		out<<"cmpq %r8, %r9"<<'\n';
		out<<"movq $0, %rcx"<<'\n';
		out<<"setne %cl"<<'\n';
		dst->genStore(out, "%rcx");
	}
	else if(op == LT){
		src1->genLoad(out, "%r8");
		src2->genLoad(out, "%r9");
		out<<"cmpq %r9, %r8"<<'\n';
		out<<"movq $0, %rcx"<<'\n';
		out<<"setl %cl"<<'\n';
		dst->genStore(out, "%rcx");
	}
	else{
		src1->genLoad(out, "%r8");
		src2->genLoad(out, "%r9");
		out<<"cmpq %r9, %r8"<<'\n';
		out<<"movq $0, %rcx"<<'\n';
		out<<"setle %cl"<<'\n';
		dst->genStore(out, "%rcx");
	}
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	if(op == NEG){
		out<<"movq $0, %rdx"<<'\n';
		src->genLoad(out, "%rax");
		out<<"subq %rax, %rdx"<<'\n';
		dst->genStore(out, "%rdx");
	}
	else{
		out<<"movq $1, %rdx"<<'\n';
		src->genLoad(out, "%rax");
		out<<"xorq %rdx, %rax"<<'\n';
		dst->genStore(out, "%rax");
	}
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoad(out, "%rax");
	dst->genStore(out, "%rax");
}

void LocQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void JmpQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->toString() << "\n";
}

void JmpIfQuad::codegenX64(std::ostream& out){
	cnd->genLoad(out, "%rsi");
	out<<"cmpq $0, %rsi"<<'\n';
	out << "je " << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

void IntrinsicQuad::codegenX64(std::ostream& out){
 if(myIntrinsic == EXIT){
	 out<<"movq $60, %rax"<<'\n';
	 out<<"movq $0, %rdi"<<'\n';
	 out<<"syscall"<<'\n';
 }
 else if(myIntrinsic == OUTPUT){
	 if(myArg->getType() == ADDR){
		  myArg->genLoad(out, "%rdi");
			out<<"callq printString"<<'\n';
	 }
	 else{
		 myArg->genLoad(out, "%rdi");
		 out<<"callq printInt"<<'\n';
	 }
 }
 else{
	 	out<<"callq getInt"<<'\n';
		myArg->genStore(out, "%rax");
 }
}

void CallQuad::codegenX64(std::ostream& out){
	out<<"callq fun_"<<callee->getName()<<'\n';
	out<<"addq $"<< clean_stack * 8 <<", %rsp"<<'\n';
}

void EnterQuad::codegenX64(std::ostream& out){
	out<<"pushq %rbp"<<'\n';
	out<<"movq %rsp, %rbp"<<'\n';
	out<<"addq $16, %rbp"<<'\n';
}

void LeaveQuad::codegenX64(std::ostream& out){
	out<<"popq %rbp"<<'\n';
	out<<"retq"<<'\n';
}

void SetInQuad::codegenX64(std::ostream& out){
	opd->genLoad(out, "%rdx");
	out<<"pushq %rdx"<<'\n';
}

void GetInQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here
}

void SetOutQuad::codegenX64(std::ostream& out){
	opd->genLoad(out, "%rax");
}

void GetOutQuad::codegenX64(std::ostream& out){
	opd->genStore(out, "%rax");
}

void SymOpd::genLoad(std::ostream & out, std::string regStr){
	if(getMemoryLoc().at(0) == 'g'){
			out<< "movq (" << getMemoryLoc() << "), " << regStr <<'\n';
	}
	else{
	    	out<< "movq " << getMemoryLoc() << ", " << regStr <<'\n';
			}
}

void SymOpd::genStore(std::ostream& out, std::string regStr){
	if(getMemoryLoc().at(0) == 'g'){
			out<< "movq " << regStr << ", (" << getMemoryLoc() <<')'<<'\n';
	}
	else{
			out<< "movq " << regStr << ", " << getMemoryLoc() <<'\n';
		}
}

void AuxOpd::genLoad(std::ostream & out, std::string regStr){
	if(getType() == QUADWORD){
		out<< "movq " << getMemoryLoc() << ", " << regStr <<'\n';
	}
	else{
		out<< "movq $" << getMemoryLoc() << ", "<< regStr <<'\n';
	}
}

void AuxOpd::genStore(std::ostream& out, std::string regStr){
  //if(getType() == QUADWORD){
		out<< "movq " << regStr << ", " << getMemoryLoc() <<'\n';
	//}
	// else{
	// 	out<< "movq " << regStr << ", (" << getMemoryLoc() << ")" <<'\n';
	// }
}

void LitOpd::genLoad(std::ostream & out, std::string regStr){
	out<< "movq $" << val << ", " << regStr <<'\n';
}

void LitOpd::genStore(std::ostream& out, std::string regStr){
	throw new InternalError("Cannot use literal as l-val");
}

}
