#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"

namespace negatron {

bool ProgramNode::semanticAnalysis(){
	SymbolTable symTab;
	ScopeTable* globalScope = new ScopeTable();
	symTab.addScope(globalScope);
	//symTab.enterScope(); //Enter the global scope
	for (auto decl : *myGlobals){
		decl->nameAnalysis(symTab);
	}
	symTab.dropScope();
	//symTab.leaveScope(); //Leave the global scope
	bool message_issued = 0;
	for (auto decl : *myGlobals){
		decl->typeAnalysis("int", message_issued);
	}

	//TODO: return true if the program passes semantic
	// analysis
	if(symTab.getFlag() || message_issued){
		return false;
	}
	else{
		return true;
	}
}

//Name analysis
void VarDeclNode::nameAnalysis(SymbolTable& symTab){
	// TODO([DELETE ME] I am a varDecl
	// 	you should add the info from my subtree
	// 	 to the symbolTable as a new entry in the
	// 	 current scope);
	if(symTab.IsInCurrent(myID->getName())){
		symTab.thereIsError();
		SemanticReport::multiDecl(myID->line(), myID->col());
	}
	if(myType->baseType() == "void"){
		symTab.thereIsError();
		SemanticReport::badDecl(myID->line(), myID->col());
	}
	else if(myType->baseType() != "bool" && myType->baseType() != "int"){
		if(symTab.IsStructDeclared(myType->baseType())){
			Symbol* occurence = symTab.getOccurence(myType->baseType());
			HashMap<std::string, Symbol *> * temp = occurence->getFieldTable();
			Symbol* newSymbol = new Symbol("StructVarDecl", myType->baseType());
			newSymbol->setFields(temp);
	 	  symTab.addSymboltoCurrent(myID->getName(), newSymbol);
		}
		else{
			    symTab.thereIsError();
					SemanticReport::unknownStruct(line(), col());
		}
	}
	else
	{
	 Symbol* newSymbol = new Symbol(nodeKind(), myType->baseType());
	 symTab.addSymboltoCurrent(myID->getName(), newSymbol);
  }
}

void FnDeclNode::nameAnalysis(SymbolTable& symTab){
	// TODO([DELETE ME] I am a fnDecl
	// 	 you should add my symbol to the global scope
	// 	 and make a new scope table for my body);
	bool flag = 0;
	std::string returnType = myRetType->baseType();
	if(symTab.IsInCurrent(myID->getName())){
		flag = 1;
		SemanticReport::multiDecl(myID->line(), myID->col());
		symTab.thereIsError();
	}
	if(returnType != "int" && returnType != "bool" && returnType != "void"){
		if(!(symTab.IsStructDeclared(myRetType->baseType()))){
			flag = 1;
			symTab.thereIsError();
			SemanticReport::unknownStruct(line(), col());
		}
	}
	else{
				Symbol* newSymbol = new Symbol(nodeKind(), returnType);
				symTab.addSymboltoCurrent(myID->getName(), newSymbol);
				//Symbol* my_formal = nullptr;
				ScopeTable* newScope = new ScopeTable();
				std::list<std::string>* args = new std::list<std::string>();
				symTab.addScope(newScope);
				for(auto decl : *myFormals){
					// if(decl->getType() == "void"){
					// 	symTab.thereIsError();
					// 	flag = 1;
					// 	SemanticReport::badDecl(decl->ID()->line(), decl->ID()->col());
					// 	if(newScope->IsInScope(decl->ID()->getName())){
					// 		symTab.thereIsError();
					// 		SemanticReport::multiDecl(decl->ID()->line(), decl->ID()->col());
					// 	}
					// }
					// else
					// {
					// 	if(newScope->IsInScope(decl->ID()->getName())){
					// 		symTab.thereIsError();
					// 		SemanticReport::multiDecl(decl->ID()->line(), decl->ID()->col());
					// 	}
					// 	else{
					// 				my_formal = new Symbol(decl->nodeKind(), decl->getType());
					// 			 	newScope->push(decl->ID()->getName(), my_formal);
									decl->nameAnalysis(symTab);
									args->push_back(decl->getType());
						//}
					// }
				}
				int actual_size = newScope->table_size();
				int size = args->size();
				if(!flag && actual_size == size){
				 newSymbol->setArgs(args);
			  }
				else{
							ScopeTable* currentScope = symTab.dropScope();
							symTab.deleteSymbolInCurrent(myID->getName());
							symTab.addScope(currentScope);
				}
				for (auto decl : *myBody){
					decl->nameAnalysis(symTab);
				}
				symTab.dropScope();
			}
}

void StructDeclNode::nameAnalysis(SymbolTable& symTab){
	// TODO([DELETE ME] I am a structDecl
	// 	 you should add my symbol to the current scope
	// 	 and make it so variables can that symbol as a type);
	if(symTab.IsStructDeclared("struct " + myID->getName()))
	{
		symTab.thereIsError();
		SemanticReport::multiDecl(myID->line(), myID->col());
		symTab.addScope(new ScopeTable());
		for (auto decl : *myFields){
			decl->nameAnalysis(symTab);
		}
		symTab.dropScope();
	}
	else{
				bool flag = 0;
				int size = myFields->size();
				Symbol* newSymbol = new Symbol(nodeKind(), "struct " + myID->getName());
				Symbol* field = nullptr;
				ScopeTable* newScope = new ScopeTable();
				symTab.addScope(newScope);
				newSymbol->setUpFields();
				std::string itself = "struct " + myID->getName();
				for (auto decl : *myFields){
					if(decl->getType() == itself)
					{
							flag = 1;
							Symbol* my_sym = new Symbol("StructVarDecl", decl->getType());
							my_sym->setUpFields();
					 	  symTab.addSymboltoCurrent(decl->ID()->getName(), my_sym);
					}
					else{
							 decl->nameAnalysis(symTab);
						  }
				}
				int actual_size = newScope->table_size();
				//symTab.dropScope();
				if(size == actual_size){
					if(flag == 1){
						for (auto decl : *myFields){
							if(decl->getType() == itself){
								Symbol* symbol = symTab.getOccurence(decl->ID()->getName());
								symbol->setFields(newScope->getTable());
							}
						}
					}
					symTab.dropScope();
					newSymbol->setFields(newScope->getTable());
					symTab.addSymboltoCurrent("struct " + myID->getName(), newSymbol);
					//std::cout<<"struct " + myID->getName()<<'\n';
				}
			}
}

void IDNode::nameAnalysis(SymbolTable& symTab){
	Symbol* occurence = symTab.getOccurence(name);
	if(occurence == nullptr){
		symTab.thereIsError();
		SemanticReport::noDecl(line(), col());
	}
	else{
			setSymbol(occurence);
	}
}

void DotAccessNode::nameAnalysis(SymbolTable& symTab){
				myBase->nameAnalysis(symTab);
				Symbol* my_sym = myBase->getSymbol();
				if(my_sym == nullptr){

				}
				else if(my_sym->getType() == "int" || my_sym->getType() == "bool")
				{
					symTab.thereIsError();
					SemanticReport::badDot(myBase->line(), myBase->col());
				}
				else{
							Symbol* entry = my_sym->getOccurence(myField->getName());
							myField->setSymbol(entry);
							if(entry == nullptr){
								symTab.thereIsError();
								SemanticReport::noDecl(myField->line(), myField->col());
							}
				}
}

void AssignStmtNode::nameAnalysis(SymbolTable& symTab){
	myExp->nameAnalysis(symTab);
}

void AssignExpNode::nameAnalysis(SymbolTable& symTab){
		myDst->nameAnalysis(symTab);
		mySrc->nameAnalysis(symTab);
}

void InputStmtNode::nameAnalysis(SymbolTable& symTab){
				myLoc->nameAnalysis(symTab);
}

void OutputStmtNode::nameAnalysis(SymbolTable& symTab){
		myExp->nameAnalysis(symTab);
}

void PostDecStmtNode::nameAnalysis(SymbolTable& symTab){
	if(myLoc->nodeKind() == "ID"){
		myLoc->nameAnalysis(symTab);
	}
}

void IfStmtNode::nameAnalysis(SymbolTable& symTab){
	myCond->nameAnalysis(symTab);
	symTab.addScope(new ScopeTable());
	for(auto stmt: *myBody){
		stmt->nameAnalysis(symTab);
	}
	symTab.dropScope();
}

void IfElseStmtNode::nameAnalysis(SymbolTable& symTab){
	myCond->nameAnalysis(symTab);
	symTab.addScope(new ScopeTable());
	for(auto stmt: *myBodyTrue){
		stmt->nameAnalysis(symTab);
	}
	symTab.dropScope();
	symTab.addScope(new ScopeTable());
	for(auto stmt: *myBodyFalse){
		stmt->nameAnalysis(symTab);
	}
	symTab.dropScope();
}

void WhileStmtNode::nameAnalysis(SymbolTable& symTab){
	myCond->nameAnalysis(symTab);
	symTab.addScope(new ScopeTable());
	for(auto stmt: *myBody){
		stmt->nameAnalysis(symTab);
	}
	symTab.dropScope();
}

void ReturnStmtNode::nameAnalysis(SymbolTable& symTab){
	if(myExp != nullptr){
		myExp->nameAnalysis(symTab);
	}
}

void CallStmtNode::nameAnalysis(SymbolTable& symTab){
	myCallExp->nameAnalysis(symTab);
}

void CallExpNode::nameAnalysis(SymbolTable& symTab){
	myID->nameAnalysis(symTab);
	for(auto expr: *myArgs){
		// if(expr->nodeKind() == "ID"){
			expr->nameAnalysis(symTab);
		// }
	}
}


//Type analysis
bool IDNode::typeAnalysis(std::string my_parent, bool& error_happened){
	return(true);
}

std::string IDNode::typeAnalysis2(bool& error_happened){
	if(mySymbol == nullptr){
			return("nullptr");
	}
	else if(getExpKind() == "FnDecl"){
		return("FnDecl");
	}
	else{
				return(getExpType());
			}
}

bool MinusNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "int" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "int" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool TimesNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "int" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "int" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool DivideNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "int" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "int" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool NegNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string my_type = myExp->typeAnalysis2(error_happened);
	if(my_type != "int" && my_type != "nullptr"){
		error_happened = 1;
		SemanticReport::badLogicOpd(myExp->line(), myExp->col());
	}
	return(1);
}

bool LessNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "int" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badRelOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "int" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badRelOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool LessEqNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "int" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badRelOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "int" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badRelOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool AndNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "bool" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badLogicOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "bool" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badLogicOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool OrNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string type_lhs = myExp1->typeAnalysis2(error_happened);
	std::string type_rhs = myExp2->typeAnalysis2(error_happened);
	if(type_lhs != "bool" && type_lhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badLogicOpd(myExp1->line(), myExp1->col());
	}
	if(type_rhs != "bool" && type_rhs != "nullptr"){
		error_happened = 1;
		SemanticReport::badLogicOpd(myExp2->line(), myExp2->col());
	}
	return(1);
}

bool NotNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string my_type = myExp->typeAnalysis2(error_happened);
	if(my_type != "bool" && my_type != "nullptr"){
		error_happened = 1;
		SemanticReport::badLogicOpd(myExp->line(), myExp->col());
	}
	return(1);
}

std::string IDNode::getExpKind(){
	if(mySymbol == nullptr){
		return("none");
	}
	else{
		return(mySymbol->getKind());
	}
}

void FnDeclNode::typeAnalysis(std::string RetType, bool& error_happened){
	for(auto stmt: *myBody){
		stmt->typeAnalysis(myRetType->baseType(), error_happened);
	}
}

void AssignStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	myExp->typeAnalysis(RetType, error_happened);
}

bool AssignExpNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string dst_type = myDst->typeAnalysis2(error_happened);
	std::string src_type = mySrc->typeAnalysis2(error_happened);
	bool flag = 0;
	if(dst_type == "nullptr" || src_type == "nullptr"){
		flag = 1;
	}
	if(dst_type == "FnDecl"){
		error_happened = 1;
		SemanticReport::badAssignOpd(myDst->line(), myDst->col());
		flag = 1;
	}
	if(src_type == "FnDecl")
	{
		error_happened = 1;
		SemanticReport::badAssignOpd(mySrc->line(), mySrc->col());
		flag = 1;
	}
	if(dst_type != src_type && !flag){
		error_happened = 1;
		SemanticReport::badAssign(line(), col());
	}
	return(false);
}

void OutputStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	if(myExp->nodeKind() == "ID"){
		std::string flag = myExp->typeAnalysis2(error_happened);
		if(flag != "nullptr"){
				std::string my_kind = myExp->getExpKind();
				if(my_kind == "FnDecl"){
					error_happened = 1;
					SemanticReport::outputFn(myExp->line(), myExp->col());
				}
				if(my_kind == "StructVarDecl"){
					error_happened = 1;
					SemanticReport::outputStruct(myExp->line(), myExp->col());
			  }
	  }
	}
	else if(myExp->nodeKind() == "DotAccess"){
		std::string my_str = myExp->typeAnalysis2(error_happened);
		if(my_str != "nullptr" && my_str != "int" && my_str != "bool"){
			SemanticReport::outputStruct(myExp->line(), myExp->col());
		}
	}
	else{
		myExp->typeAnalysis(this->nodeKind(), error_happened);
	}
}

void InputStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	if(myLoc->nodeKind() == "ID"){
		std::string flag = myLoc->typeAnalysis2(error_happened);
	if(flag != "nullptr"){
			std::string my_kind = myLoc->getExpKind();
			if(my_kind == "FnDecl"){
				error_happened = 1;
				SemanticReport::inputFn(myLoc->line(), myLoc->col());
			}
			if(my_kind == "StructVarDecl"){
				error_happened = 1;
				SemanticReport::inputStruct(myLoc->line(), myLoc->col());
			}
	  }
  }
	else{
				std::string my_str = myLoc->typeAnalysis2(error_happened);
				if(my_str != "nullptr" && my_str != "int" && my_str != "bool"){
					SemanticReport::inputStruct(myLoc->line(), myLoc->col());
				}
	}
}

void PostDecStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	std::string my_type = myLoc->typeAnalysis2(error_happened);
	if(my_type != "int" && my_type != "nullptr"){
		error_happened = 1;
		SemanticReport::badArithOpd(myLoc->line(), myLoc->col());
	}
}

void IfStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	std::string my_type = myCond->typeAnalysis2(error_happened);
	if(my_type != "bool" && my_type != "nullptr"){
		error_happened = 1;
		SemanticReport::badIfPred(myCond->line(), myCond->col());
	}
	for(auto stmt: *myBody){
		stmt->typeAnalysis(RetType, error_happened);
	}
}

void IfElseStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	std::string my_type = myCond->typeAnalysis2(error_happened);
	if(my_type != "bool" && my_type != "nullptr"){
		error_happened = 1;
		SemanticReport::badIfPred(myCond->line(), myCond->col());
	}
	for(auto stmt: *myBodyTrue){
		stmt->typeAnalysis(RetType, error_happened);
	}
	for(auto stmt: *myBodyFalse){
		stmt->typeAnalysis(RetType, error_happened);
	}
}

void WhileStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	std::string my_type = myCond->typeAnalysis2(error_happened);
	if(my_type != "bool" && my_type != "nullptr"){
		error_happened = 1;
		SemanticReport::badWhilePred(myCond->line(), myCond->col());
	}
	for(auto stmt: *myBody){
		stmt->typeAnalysis(RetType, error_happened);
	}
}

void ReturnStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	if(myExp == nullptr && RetType != "void"){
		error_happened = 1;
		SemanticReport::noRet(line(), col());
	}
	else if(myExp != nullptr && RetType == "void"){
		error_happened = 1;
		SemanticReport::retFromVoid(line(), col());
	}
	else if(myExp != nullptr){
		std::string my_type = myExp->typeAnalysis2(error_happened);
		if(my_type != "nullptr" && RetType != my_type){
			error_happened = 1;
			SemanticReport::badRet(myExp->line(), myExp->col());
		}
	}
}

void CallStmtNode::typeAnalysis(std::string RetType, bool& error_happened){
	myCallExp->typeAnalysis(this->nodeKind(), error_happened);
}

bool NotEqualsNode::typeAnalysis(std::string my_parent, bool& error_happened){
	std::string my_kind1 = myExp1->nodeKind();
	std::string my_kind2 = myExp2->nodeKind();
	if(my_kind1 == "StrLit" || my_kind2 == "StrLit"){
		error_happened = 1;
		SemanticReport::badEq(line(), col());
		return(false);
	}
	else{
		std::string type_1 = myExp1->typeAnalysis2(error_happened);
		std::string type_2 = myExp2->typeAnalysis2(error_happened);
		if(type_1 != "int" && type_1 != "bool" && type_1 != "void" && type_1 != "FnDecl" && type_1 != "nullptr"){
			error_happened = 1;
			SemanticReport::badEq(line(), col());
			return(false);
		}
		else if(type_2 != "int" && type_2 != "bool" && type_2 != "void" && type_2 != "FnDecl" && type_2 != "nullptr"){
			error_happened = 1;
			SemanticReport::badEq(line(), col());
			return(false);
		}
		else{
			bool flag = 0;
			if(type_1 == "nullptr" || type_2 == "nullptr"){
				flag = 1;
			}
			if(type_1 == "void"){
				  error_happened = 1;
				  flag = 1;
					SemanticReport::badEqOpd(myExp1->line(), myExp1->col());
			}
			if(type_2 == "void"){
				error_happened = 1;
				flag = 1;
				SemanticReport::badEqOpd(myExp2->line(), myExp2->col());
			}
			if(type_1 == "FnDecl"){
				error_happened = 1;
				flag = 1;
				SemanticReport::badEqOpd(myExp1->line(), myExp1->col());
			}
			if(type_2 == "FnDecl"){
				error_happened = 1;
				flag = 1;
				SemanticReport::badEqOpd(myExp2->line(), myExp2->col());
			}
			if(type_1 != type_2 && !flag){
				error_happened = 1;
				SemanticReport::badEq(line(), col());
				return(false);
			}
			else{
						return(true);
					}
	 }
	 return(false);
  }
}


bool CallExpNode::typeAnalysis(std::string my_parent, bool& error_happened){
	Symbol* my_symbol = myID->getSymbol();
	if(my_parent == "OutputStmt"){
		if(my_symbol != nullptr){
		if(IsReturnVoid()){
			error_happened = 1;
			SemanticReport::outputFnVoid(myID->line(), myID->col());
		}
	 }
	}
	if(my_symbol != nullptr){
		std::list<std::string>* fn_args = my_symbol->getArgs();
		if(my_symbol->getKind() != "FnDecl"){
			error_happened = 1;
			SemanticReport::NotFnCall(myID->line(), myID->col());
		}
		else{
					int num_args = fn_args->size();
					int num_args_call = myArgs->size();
					if(num_args != num_args_call){
						error_happened = 1;
						SemanticReport::argCount(myID->line(), myID->col());
					}
					else{
								std::list<std::string>* new_list = new std::list<std::string>(*fn_args);
								for(auto expr : *myArgs){
									std::string real_type = new_list->front();
									std::string my_type = expr->typeAnalysis2(error_happened);
									if(my_type != "nullptr" && real_type != my_type){
										error_happened = 1;
										SemanticReport::argType(expr->line(), expr->col());
									}
									new_list->pop_front();
						}//endif
		    }
	    }
   }
	 					return(true);
}

} // end namespace
