/*
#ifndef TEENC_ERROR_REPORTING_HH
#define TEENC_ERROR_REPORTING_HH

#define EXPAND2(x) #x
#define EXPAND1(x) EXPAND2(x)
#define CODELOC __FILE__ ":" EXPAND1(__LINE__) " - "
#define TODO(x) throw new ToDoError(CODELOC #x);

#include <iostream>

namespace negatron{

class Err{
	public:
	static void report(const std::string msg){ 
		std::cerr << msg << std::endl;
	}
	static void semanticReport(
		size_t line, 
		size_t col, 
		const std::string msg
	){
		std::cerr << line << "," << col 
			<< ": " << msg << std::endl;
	}
	static void syntaxReport(const std::string msg){
		negatron::Err::report(" ***ERROR*** " + msg);
	}
};

} //End namespace negatron

#endif
*/
