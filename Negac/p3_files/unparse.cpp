#include "ast.hpp"
#include "errors.hpp"

namespace negatron{

static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

void ProgramNode::unparse(std::ostream& out, int indent){
	for (DeclNode * decl : *myGlobals){
		decl->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	myType->unparse(out, 0);
	out << " ";
	myID->unparse(out, 0);
	out << ";\n";
}

void FormalDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	myType->unparse(out, 0);
	out << " ";
	myID->unparse(out, 0);
}

void StructDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	out << "struct ";
	myID->unparse(out, 0);
	out << " {\n";
	for (auto elt : *myFields){
		elt->unparse(out, indent + 1);
	}
	out << "};\n";
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent); 
	myRetType->unparse(out, 0); 
	out << " ";
	myID->unparse(out, 0);
	out << "(";
	bool firstFormal = true;
	for(auto formal : *myFormals){
		if (firstFormal) { firstFormal = false; }
		else { out << ", "; }
		formal->unparse(out, 0);
	}
	out << "){\n";
	for(auto stmt : *myBody){
		stmt->unparse(out, indent+1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void AssignStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myExp->unparseTop(out,0);
	out << ";\n";
}

void InputStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "input ";
	myLoc->unparse(out,0);
	out << ";\n";
}

void OutputStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "output ";
	myExp->unparse(out,0);
	out << ";\n";
}

void PostDecStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLoc->unparse(out,0);
	out << "--;\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "if (";
	myCond->unparse(out, 0);
	out << "){\n";
	for (auto stmt : *myBody){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "if (";
	myCond->unparse(out, 0);
	out << "){\n";
	for (auto stmt : *myBodyTrue){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "} else {\n";
	for (auto stmt : *myBodyFalse){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void WhileStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "while (";
	myCond->unparse(out, 0);
	out << "){\n";
	for (auto stmt : *myBody){
		stmt->unparse(out, indent + 1);
	}
	doIndent(out, indent);
	out << "}\n";
}

void ReturnStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "return ";
	if (myExp != nullptr){
		myExp->unparse(out, 0);
	}
	out << ";\n";
}

void CallStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myCallExp->unparse(out, 0);
	out << ";\n";
}

void CallExpNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myID->unparse(out, 0);
	out << "(";
	
	bool firstArg = true;
	for(auto arg : *myArgs){
		if (firstArg) { firstArg = false; }
		else { out << ", "; }
		arg->unparse(out, 0);
	}
	out << ")";
}

void MinusNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " - ";
	myExp2->unparse(out, 0);
	out << ")";
}

void TimesNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " * ";
	myExp2->unparse(out, 0);
	out << ")";
}

void DivideNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " / ";
	myExp2->unparse(out, 0);
	out << ")";
}

void AndNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " && ";
	myExp2->unparse(out, 0);
	out << ")";
}

void OrNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " || ";
	myExp2->unparse(out, 0);
	out << ")";
}

void NotEqualsNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " != ";
	myExp2->unparse(out, 0);
	out << ")";
}

void LessNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " < ";
	myExp2->unparse(out, 0);
	out << ")";
}

void LessEqNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myExp1->unparse(out, 0); 
	out << " <= ";
	myExp2->unparse(out, 0);
	out << ")";
}

void NotNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	out << "!";
	myExp->unparse(out, 0); 
	out << ")";
}

void NegNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	out << "-";
	myExp->unparse(out, 0); 
	out << ")";
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "void";
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "int";
	if (isRef()){ out << " &"; }
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "bool";
	if (isRef()){ out << " &"; }
}

void StructTypeNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "struct ";
	myStructID->unparse(out, 0);
	if (isRef()){ out << " &"; }
}

void AssignExpNode::unparseTop(std::ostream& out, int indent){
	doIndent(out, indent);
	myDst->unparse(out, 0);
	out << " = ";
	mySrc->unparse(out, 0);
}

void AssignExpNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(";
	myDst->unparse(out, 0);
	out << " = ";
	mySrc->unparse(out, 0);
	out << ")";
}

void IDNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << name;
}

void DotAccessNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myBase->unparse(out, 0); 
	out << ".";
	myField->unparse(out, 0); 
}

void IntLitNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << myNum;
}

void StrLitNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << myStr;
}

void FalseNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "false";
}

void NullRefNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "nullref";
}

} //End namespace negatron
