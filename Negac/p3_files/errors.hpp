#ifndef NEGATRON_ERRRORS_H
#define NEGATRON_ERRRORS_H

#define EXPAND2(x) #x
#define EXPAND1(x) EXPAND2(x)
#define CODELOC __FILE__ ":" EXPAND1(__LINE__) " - "
#define TODO(x) throw new ToDoError(CODELOC #x);

#include <iostream>

namespace negatron{

class InternalError{
public:
	InternalError(const char * msgIn) : myMsg(msgIn){}
	std::string msg(){ return myMsg; }
private:
	std::string myMsg;
};

class ToDoError{
public:
	ToDoError(const char * msgIn) : myMsg(msgIn){}
	const char * msg(){ return myMsg; }
private:
	const char * myMsg;
};

class Report{
public:
	static void fatal(
		size_t l,
		size_t c,
		const char * msg
	){
		std::cerr << "FATAL [" << l << "," << c << "]: "
		<< msg  << std::endl;
	}

	static void fatal(
		size_t l,
		size_t c,
		const std::string msg
	){
		fatal(l,c,msg.c_str());
	}

	static void warn(
		size_t l,
		size_t c,
		const char * msg
	){
		std::cerr << "*WARNING* [" << l << "," << c << "]: "
		<< msg  << std::endl;
	}

	static void warn(
		size_t l,
		size_t c,
		const std::string msg
	){
		warn(l,c,msg.c_str());
	}
};

class SemanticReport{
public:
	static void multiDecl(size_t l, size_t c){
		Report::fatal(l, c,
		"Multiply declared identifier");
	}

	static void noDecl(size_t l, size_t c){
		Report::fatal(l, c,
		"Undeclared identifier");
	}

	static void badDecl(size_t l, size_t c){
		Report::fatal(l, c,
		"Invalid type in declaration");
	}

	static void unknownStruct(size_t l, size_t c){
		Report::fatal(l, c,
		"Unknown struct used");
	}

	static void outputStruct(size_t l, size_t c){
		Report::fatal(l, c,
		"Attempt to output a struct variable");
	}

	static void outputFn(size_t l, size_t c){
		Report::fatal(l, c,
		"Attempt to output function");
	}

	static void outputFnVoid(size_t l, size_t c){
		Report::fatal(l, c,
		"Attempt to write void");
	}

	static void inputFn(size_t l, size_t c){
		Report::fatal(l, c,
		"Attempt to read a function");
	}

	static void inputStruct(size_t l, size_t c){
		Report::fatal(l, c,
		"Attempt to read a struct variable");
	}

	static void NotFnCall(size_t l, size_t c){
		Report::fatal(l, c,
		"Attempt to call a non-function");
	}

	static void argCount(size_t l, size_t c){
		Report::fatal(l, c,
		"Function call with wrong number of args");
	}

	static void argType(size_t l, size_t c){
		Report::fatal(l, c,
		"Type of actual does not match type of formal");
	}

	static void noRet(size_t l, size_t c){
		Report::fatal(l, c, "Missing return value");
	}

	static void retFromVoid(size_t l, size_t c){
		Report::fatal(l, c, "Return with a value in a void function");
	}

	static void badRet(size_t l, size_t c){
		Report::fatal(l, c, "Bad return value");
	}

	static void badArithOpd(size_t l, size_t c){
		Report::fatal(l, c, "Arithmetic operator applied to invalid operand");
	}

	static void badArith(size_t l, size_t c){
		Report::fatal(l, c, "Arithmetic operator applied to incompatible operands");
	}

	static void badRelOpd(size_t l, size_t c){
		Report::fatal(l, c, "Relational operator applied to non-numeric operand");
	}

	static void badLogicOpd(size_t l, size_t c){
		Report::fatal(l, c, "Logical operator applied to non-bool operand");
	}

	static void badIfPred(size_t l, size_t c){
		Report::fatal(l, c, "Non-bool expression used as an if condition");
	}

	static void badWhilePred(size_t l, size_t c){
		Report::fatal(l, c,"Non-bool expression used as a while condition");
	}

	static void badEqOpd(size_t l, size_t c){
		Report::fatal(l, c, "Invalid equality operand");
	}

	static void badEq(size_t l, size_t c){
		Report::fatal(l, c, "Invalid equality operation");
	}

	static void badAssignOpd(size_t l, size_t c){
		Report::fatal(l, c, "Invalid assignment operand");
	}

	static void badAssign(size_t l, size_t c){
		Report::fatal(l, c, "Invalid assignment operation");
	}

	static void badDot(size_t l, size_t c){
		Report::fatal(l, c, "Bad dot operation");
	}
};

}

#endif
