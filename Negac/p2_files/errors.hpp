#ifndef NEGATRON_ERRRORS_H
#define NEGATRON_ERRRORS_H

#include <iostream>

namespace negatron{

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

	static void argCount(size_t l, size_t c){
		Report::fatal(l, c, 
		"Function call with wrong number of args");
	}
};

class InternalError{
public:
	InternalError(const char * msgIn) : myMsg(msgIn){}
	std::string msg(){ return myMsg; }
private:
	std::string myMsg;
};

}

#endif
