#include "ast.hpp"
#include "symbol_table.hpp"
#include "errName.hpp"
#include "types.hpp"

namespace negatron{

void FnDeclNode::isMain(bool& flag){
		if( myID->getName() == "main" ){
			flag = 1; }
	}

bool ProgramNode::nameAnalysis(SymbolTable * symTab){
	//Enter the global scope
	symTab->enterScope();
	bool res = true;
	bool flag_main = false;
	for (auto decl : *myGlobals){
		decl->isMain(flag_main);
	}
	if(flag_main){
		for (auto decl : *myGlobals){
			res = decl->nameAnalysis(symTab) && res;
		}
		//Leave the global scope
		symTab->leaveScope();
		return res;
	}
	else{
				throw new InternalError("Program requires a main function!");
		    return(false);
	}
}

const DataType * IntTypeNode::getDataType(){
	return VarType::produce(BaseType::INT);
}

const DataType * VoidTypeNode::getDataType(){
	return VarType::produce(BaseType::VOID);
}

const DataType * BoolTypeNode::getDataType(){
	return VarType::produce(BaseType::BOOL);
}

std::string TypeNode::getTypeString(){
	return this->getDataType()->getString();
}

bool TypeNode::nameAnalysis(SymbolTable * symTab){
	throw new InternalError("Name analysis should"
		" never reach type nodes");
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab){
	return myExp->nameAnalysis(symTab);
}

bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab){
	return myID->nameAnalysis(symTab);
}

bool InputStmtNode::nameAnalysis(SymbolTable * symTab){
	return myID->nameAnalysis(symTab);
}

bool OutputStmtNode::nameAnalysis(SymbolTable * symTab){
	return myExp->nameAnalysis(symTab);
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myCond->nameAnalysis(symTab) && result;
	symTab->enterScope();
	for (auto stmt : *myBody){
		result = stmt->nameAnalysis(symTab) && result;
	}
	symTab->leaveScope();
	return result;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myCond->nameAnalysis(symTab) && result;
	symTab->enterScope();
	for (auto stmt : *myBodyTrue){
		result = stmt->nameAnalysis(symTab) && result;
	}
	symTab->leaveScope();
	symTab->enterScope();
	for (auto stmt : *myBodyFalse){
		result = stmt->nameAnalysis(symTab) && result;
	}
	symTab->leaveScope();
	return result;
}

bool WhileStmtNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myCond->nameAnalysis(symTab) && result;
	symTab->enterScope();
	for (auto stmt : *myBody){
		result = stmt->nameAnalysis(symTab) && result;
	}
	symTab->leaveScope();
	return result;
}

static bool dataDecl(SymbolTable * symTab, VarDeclNode * decl, TypeNode * typeNode){
	const DataType * dataType = typeNode->getDataType();
	bool validType = true;

	const VarType * varObj = dataType->asVar();
	if (!varObj){
		throw InternalError("Variable without variable type");
	}

	IDNode * id = decl->ID();
	std::string varName = id->getName();
	if (varObj->getBaseType() == BaseType::VOID){
		NameErr::badVoid(id->line(), id->col());
		SemSymbol * sym = new SemSymbol(VAR, ErrorType::produce(), varName);
		decl->ID()->attachSymbol(sym);
		validType = false;
	}

	bool validName = !symTab->clash(varName);
	if (!validName){
		NameErr::multiDecl(id->line(), id->col());
	}

	if (!validType || !validName){ return false; }

	SemSymbol * sym = new SemSymbol(VAR, dataType, varName);
	decl->ID()->attachSymbol(sym);
	symTab->insert(sym);
	return true;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	return dataDecl(symTab, this, this->getTypeNode());
}

bool FormalDeclNode::nameAnalysis(SymbolTable * symTab){
	return dataDecl(symTab, this, this->getTypeNode());
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	std::string fnName = this->ID()->getName();
	const DataType * retType = myRetType->getDataType();
	const VarType * retVarType = retType->asVar();
	if (retVarType->getBaseType() == BaseType::VOID){
		//It's ok for a function to have a void return type
	}

	// hold onto the scope where the function itself is
	ScopeTable * atFnScope = symTab->getCurrentScope();
	//Enter a new scope for this function.
	ScopeTable * inFnScope = symTab->enterScope();

	bool validFormals = true;
	std::list<const DataType *> * formalTypes =
		new std::list<const DataType *>();
	for (auto formal : *(this->myFormals)){
		validFormals = formal->nameAnalysis(symTab) && validFormals;
		formalTypes->push_back(formal->getDeclaredType());
	}

	const TupleType * formalsType = new TupleType(formalTypes);

	//Note that we check for a clash of the function name in
	// the scope at which it exists (i.e. the function scope)
	bool validName = !atFnScope->clash(fnName);
	if (validName == false){
		NameErr::multiDecl(ID()->line(), ID()->col());
	}

	//Make sure the fnSymbol is in the symbol table before
	// analyzing the body, to allow for recursive calls
	if (validName && validFormals){
		FnType * fnType = new FnType(formalsType, retType);
		SemSymbol * fnSym = new SemSymbol(FN, fnType, fnName);
		atFnScope->insert(fnSym);
		ID()->attachSymbol(fnSym);
	}

	bool validBody = true;
	for (auto stmt : *myBody){
		validBody = stmt->nameAnalysis(symTab) && validBody;
	}

	symTab->leaveScope();
	return (validName && validFormals && validBody);
}

bool BinaryExpNode::nameAnalysis(SymbolTable * symTab){
	bool result = true;
	result = myExp1->nameAnalysis(symTab) && result;
	result = myExp2->nameAnalysis(symTab) && result;
	return result;
}

bool CallExpNode::nameAnalysis(SymbolTable* symTab){
	bool result = true;
	result = myID->nameAnalysis(symTab) && result;
	for (auto arg : *myArgs){
		result = arg->nameAnalysis(symTab) && result;
	}
	return result;
}

bool NegNode::nameAnalysis(SymbolTable* symTab){
	return myExp->nameAnalysis(symTab);
}

bool NotNode::nameAnalysis(SymbolTable* symTab){
	return myExp->nameAnalysis(symTab);
}

bool AssignExpNode::nameAnalysis(SymbolTable* symTab){
	bool result = true;
	result = myDst->nameAnalysis(symTab) && result;
	result = mySrc->nameAnalysis(symTab) && result;
	return result;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab){
	if (myExp == nullptr){
		return true;
	}
	return myExp->nameAnalysis(symTab);
}

bool CallStmtNode::nameAnalysis(SymbolTable* symTab){
	return myCallExp->nameAnalysis(symTab);
}

bool IDNode::nameAnalysis(SymbolTable* symTab){
	std::string myName = this->getName();
	SemSymbol * sym = symTab->find(myName);
	if (sym == nullptr){
		return NameErr::undecl(line(), col());
	}
	this->attachSymbol(sym);
	return true;
}

void IDNode::attachSymbol(SemSymbol * symbolIn){
	this->mySymbol = symbolIn;
}

SemSymbol * IDNode::getSymbol() const{
	return this->mySymbol;
}

}
