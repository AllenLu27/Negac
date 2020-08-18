#include "ast.hpp"
#include "symbol_table.hpp"
#include "types.hpp"

namespace negatron {

void ProgramNode::typeAnalysis(TypeAnalysis * typing){
	for (auto decl : *myGlobals){
		decl->typeAnalysis(typing, nullptr);
	}
	typing->nodeType(this, VarType::VOID());
}

void IDNode::typeAnalysis(TypeAnalysis * typing){
	if (mySymbol == nullptr){
		typing->nodeType(this, ErrorType::produce());
	} else {
		typing->nodeType(this, mySymbol->getType());
	}
}

void VarDeclNode::typeAnalysis(TypeAnalysis * typing, FnType * fnType){
	typing->nodeType(this, myType->getDataType());
}

void FnDeclNode::typeAnalysis(TypeAnalysis * typing, FnType * fnType){
	IDNode * id = this->ID();
	if (ID() == NULL){ throw new InternalError("No id!"); }

	SemSymbol * sym = ID()->getSymbol();

	
	FnType * myDataType = nullptr;
	if (sym == nullptr){
		typing->nodeType(this, ErrorType::produce());
	} else { 
		std::list<const DataType *> * formalTypes = 
	  		new std::list<const DataType *>();
		for (auto formal : *myFormals){
			formal->typeAnalysis(typing, fnType);
			formalTypes->push_back(typing->nodeType(formal));
		}	
		const TupleType * formalsType = new TupleType(formalTypes);
		myDataType = new FnType(formalsType, myRetType->getDataType());
		typing->nodeType(this, myDataType);
	}
	
	for (auto stmt : *myBody){
		stmt->typeAnalysis(typing, myDataType);
	}
}

static const DataType * typeAssignOpd(TypeAnalysis * typing, ExpNode * opd){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	//Errors are invalid, but don't cause re-reports
	if (type->asError()){ return nullptr; }

	//Valid types are returned
	if (type->asVar()){ return type; }

	//Invalid types are reported and skip operator check
	typing->badAssignOpd(opd->line(),opd->col());
	return nullptr;
}

void AssignExpNode::typeAnalysis(TypeAnalysis * typing){
	const DataType * dstType = typeAssignOpd(typing, myDst);
	const DataType * srcType = typeAssignOpd(typing, mySrc);

	if (!dstType || !srcType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (dstType == srcType){
		typing->nodeType(this, dstType);
		return;
	}

	typing->badAssignOpr(line(), col());
	typing->nodeType(this, ErrorType::produce());
	return;
}

void CallExpNode::typeAnalysis(TypeAnalysis * typing){

	std::list<const DataType *> * aList = new std::list<const DataType *>();
	for (auto actual : *myArgs){
		actual->typeAnalysis(typing);
		aList->push_back(typing->nodeType(actual));
	}

	SemSymbol * calleeSym = myID->getSymbol();
	if (calleeSym == nullptr){
		//We're in a function with an invalid declaration
		typing->nodeType(this, ErrorType::produce());
		return;
	}
	const DataType * calleeType = calleeSym->getType();
	const FnType * fnType = calleeType->asFn();
	if (fnType == nullptr){
		typing->badCallee(myID->line(), myID->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	const TupleType * actualsTypes = new TupleType(aList);
	const TupleType * formalsTypes = fnType->getFormalTypes();
	auto fList = formalsTypes->getElts();
	if (aList->size() != fList->size()){
		typing->badArgCount(line(), col());
		typing->nodeType(this, ErrorType::produce());
		return;
	} else {
		auto actualTypesItr = aList->begin();
		auto formalTypesItr = fList->begin();
		auto actualsItr = myArgs->begin();
		while(actualTypesItr != aList->end()){
			const DataType * actualType = *actualTypesItr;
			const DataType * formalType = *formalTypesItr;
			const ExpNode * actual = *actualsItr;
			actualTypesItr++;
			formalTypesItr++;
			actualsItr++;

			//Matching to error is ignored
			if (actualType->asError()){ continue; }
			if (formalType->asError()){ continue; }

			//Ok match
			if (formalType == actualType){ continue; }

			//Bad match
			typing->badArgMatch(actual->line(), actual->col());
			typing->nodeType(this, ErrorType::produce());
		}
	}

	typing->nodeType(this, fnType->getReturnType());
	return;
}

void NegNode::typeAnalysis(TypeAnalysis * typing){
	myExp->typeAnalysis(typing);
	const DataType * subType = typing->nodeType(myExp);

	//Propagate error, don't re-report
	if (subType->asError()){
		typing->nodeType(this, subType);
		return;
	}
	if (subType == VarType::produce(INT)){
		typing->nodeType(this, VarType::produce(INT));
	} else {
		typing->badMathOpd(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
	}
}

void NotNode::typeAnalysis(TypeAnalysis * typing){
	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);
	if (childType == VarType::produce(BOOL)){
		typing->nodeType(this, childType);
	} else if (childType->asError() != nullptr){
		typing->nodeType(this, ErrorType::produce());
		return;
	} else {
		typing->badLogicOpd(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
	}
}

static bool typeMathOpd(TypeAnalysis * typing, ExpNode * opd){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);
	if (type->isInt()){ return true; }
	if (type->asError()){
		//Don't re-report an error, but don't check for
		// incompatibility
		return false;
	}

	typing->badMathOpd(opd->line(), opd->col());
	return false;
}


void BinaryExpNode::binaryMathTyping(
	TypeAnalysis * typing
){
	bool lhsValid = typeMathOpd(typing, myExp1);
	bool rhsValid = typeMathOpd(typing, myExp2);
	if (!lhsValid || !rhsValid){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	//Get the valid operand types, check operator
	const DataType * lhsType = typing->nodeType(myExp1);
	const DataType * rhsType = typing->nodeType(myExp2);

	if (lhsType->isInt() && rhsType->isInt()){
		typing->nodeType(this, VarType::produce(INT));
		return;
	}
	typing->badMathOpr(this->line(), this->col());
	typing->nodeType(this, ErrorType::produce());
	return;
}

static const DataType * typeLogicOpd(
	TypeAnalysis * typing, ExpNode * opd
){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	//Return type if it's valid
	if (type->isBool()){ return type; }

	//Don't re-report an error, but return null to
	// indicate incompatibility
	if (type->asError()){ return nullptr; }

	//If type isn't an error, but is incompatible,
	// report and indicate incompatibility
	typing->badLogicOpd(opd->line(), opd->col());
	return NULL;
}

void BinaryExpNode::binaryLogicTyping(TypeAnalysis * typing){
	const DataType * lhsType = typeLogicOpd(typing, myExp1);
	const DataType * rhsType = typeLogicOpd(typing, myExp2);
	if (!lhsType || !rhsType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	//Given valid operand types, check operator
	if (lhsType->isBool() && rhsType->isBool()){
		typing->nodeType(this, VarType::produce(BOOL));
		return;
	}

	//We never expect to get here, so we'll consider it
	// an error with the compiler itself
	throw new InternalError("Incomplete typing");
	typing->nodeType(this, ErrorType::produce());
	return;
}

void MinusNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void TimesNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void DivideNode::typeAnalysis(TypeAnalysis * typing){
	binaryMathTyping(typing);
}

void AndNode::typeAnalysis(TypeAnalysis * typing){
	binaryLogicTyping(typing);
}

void OrNode::typeAnalysis(TypeAnalysis * typing){
	binaryLogicTyping(typing);
}

static const DataType * typeEqOpd(
	TypeAnalysis * typing, ExpNode * opd
){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	if (type->isInt()){ return type; }
	if (type->isBool()){ return type; }

	//Errors are invalid, but don't cause re-reports
	if (type->asError()){ return nullptr; }

	typing->badEqOpd(opd->line(), opd->col());
	return nullptr;
}

void BinaryExpNode::binaryEqTyping(TypeAnalysis * typing){
	const DataType * lhsType = typeEqOpd(typing, myExp1);
	const DataType * rhsType = typeEqOpd(typing, myExp2);

	if (!lhsType || !rhsType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (lhsType == rhsType){
		typing->nodeType(this, VarType::produce(BOOL));
		return;
	}

	typing->badEqOpr(line(), col());
	typing->nodeType(this, ErrorType::produce());
	return;
}

void NotEqualsNode::typeAnalysis(TypeAnalysis * typing){
	binaryEqTyping(typing);
}

static const DataType * typeRelOpd(
	TypeAnalysis * typing, ExpNode * opd
){
	opd->typeAnalysis(typing);
	const DataType * type = typing->nodeType(opd);

	if (type->isInt()){ return type; }

	//Errors are invalid, but don't cause re-reports
	if (type->asError()){ return nullptr; }

	typing->badRelOpd(opd->line(),opd->col());
	typing->nodeType(opd, ErrorType::produce());
	return nullptr;
}

void BinaryExpNode::binaryRelTyping(TypeAnalysis * typing){
	const DataType * lhsType = typeRelOpd(typing, myExp1);
	const DataType * rhsType = typeRelOpd(typing, myExp2);

	if (!lhsType || !rhsType){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (lhsType->isInt() && rhsType->isInt()){
		typing->nodeType(this, VarType::produce(BOOL));
		return;
	}
	//There is no bad relational operator, so we never 
	// expect to get here
	return;
}

void LessNode::typeAnalysis(TypeAnalysis * typing){
	binaryRelTyping(typing);
}

void LessEqNode::typeAnalysis(TypeAnalysis * typing){
	binaryRelTyping(typing);
}

void AssignStmtNode::typeAnalysis(
	TypeAnalysis * typing, FnType * fn
){
	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);
	if (childType->asError()){
		typing->nodeType(this, ErrorType::produce());
	} else {
		typing->nodeType(this, VarType::VOID());
	}
}

static const DataType * typeUnaryMath(
	size_t line, size_t col,
	TypeAnalysis * typing, ExpNode * exp
){
	exp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(exp);

	//Propagate error but don't re-report
	if (childType->asError()){
		return ErrorType::produce();
	}

	//It's fine to do unary math on an int
	if (childType->isInt()){
		return childType;
	}

	//Any other unary math is an error
	typing->badMathOpr(line, col);
	return ErrorType::produce();
}

void PostDecStmtNode::typeAnalysis(
	TypeAnalysis * typing, FnType * fn
){
	typing->nodeType(this, typeUnaryMath(this->line(), 
		this->col(), typing, myID));
}

void InputStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	myID->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myID);
	const VarType * childAsVar = childType->asVar();

	if (childType->asVar()){
		//Always ok
	} else if (childType->asFn()){
		typing->readFn(myID->line(),myID->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	}
	typing->nodeType(this, VarType::VOID());
}

void OutputStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);

	//Mark error, but don't re-report
	if (childType->asError()){
		typing->nodeType(this, childType);
		return;
	}

	//Check for invalid type
	if (childType == VarType::VOID()){
		typing->badWriteVoid(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	} else if (childType->asFn()){
		typing->writeFn(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	} else if (const VarType * asVar = childType->asVar()){
		//Can write to a var of any other type
	}

	typing->nodeType(this, VarType::VOID());
}

void IfStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	//Start off the typing as void, but may update to error
	typing->nodeType(this, VarType::VOID());

	myCond->typeAnalysis(typing);
	const DataType * condType = typing->nodeType(myCond);
	if (condType->asError()){
		typing->nodeType(this, ErrorType::produce());
	} else if (condType != VarType::produce(BOOL)){
		typing->badIfCond(
			myCond->line(), 
			myCond->col());
		typing->nodeType(this, 
			ErrorType::produce());
	}

	for (auto stmt : *myBody){
		stmt->typeAnalysis(typing, fn);
	}
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	myCond->typeAnalysis(typing);
	const DataType * condType = typing->nodeType(myCond);

	if (condType->asError()){
		typing->nodeType(this, ErrorType::produce());
	} else if (condType != VarType::produce(BOOL)){
		typing->badIfCond(myCond->line(), myCond->col());
	}
	for (auto stmt : *myBodyTrue){
		stmt->typeAnalysis(typing, fn);
	}
	for (auto stmt : *myBodyFalse){
		stmt->typeAnalysis(typing, fn);
	}
	
	typing->nodeType(this, VarType::produce(VOID));
}

void WhileStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	myCond->typeAnalysis(typing);
	const DataType * condType = typing->nodeType(myCond);

	if (condType->asError()){
		typing->nodeType(this, ErrorType::produce());
	} else if (condType != VarType::produce(BOOL)){
		typing->badWhileCond(myCond->line(), myCond->col());
	}

	for (auto stmt : *myBody){
		stmt->typeAnalysis(typing, fn);
	}

	typing->nodeType(this, VarType::produce(VOID));
}

void CallStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	myCallExp->typeAnalysis(typing);
	typing->nodeType(this, VarType::VOID());
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis * typing, FnType * fn){
	const FnType * fnType = fn;
	const DataType * fnRet = fnType->getReturnType();

	//Check: shouldn't return anything
	if (fnRet == VarType::VOID()){
		if (myExp != nullptr) {
			myExp->typeAnalysis(typing);
			typing->extraRetValue(
				myExp->line(), 
				myExp->col()); 
			typing->nodeType(this, ErrorType::produce());
		} else {
			typing->nodeType(this, VarType::VOID());
		}
		return;
	}

	//Check: returns nothing, but should
	if (myExp == nullptr){
			typing->badNoRet(line(), col());
			typing->nodeType(this, ErrorType::produce());
			return;
	}

	myExp->typeAnalysis(typing);
	const DataType * childType = typing->nodeType(myExp);

	if (childType->asError()){
		typing->nodeType(this, ErrorType::produce());
		return;
	}

	if (childType != fnRet){
		typing->badRetValue(myExp->line(), myExp->col());
		typing->nodeType(this, ErrorType::produce());
		return;
	}
	typing->nodeType(this, ErrorType::produce());
	return;
}

void StrLitNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, VarType::produce(BaseType::STR));
}

void FalseNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, VarType::produce(BaseType::BOOL));
}

void IntLitNode::typeAnalysis(TypeAnalysis * typing){
	typing->nodeType(this, VarType::produce(BaseType::INT));
}

}
