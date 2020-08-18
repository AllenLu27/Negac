#ifndef TEENC_NAME_ERROR_REPORTING_HH
#define TEENC_NAME_ERROR_REPORTING_HH

#include "errors.hpp"

namespace negatron{

class NameErr{
public:
static bool undecl(size_t line, size_t col){
	Report::fatal(line, col, "Undeclared identifier");
	return false;
}
static bool multiDecl(size_t line, size_t col){
	Report::fatal(line, col, "Multiply declared identifier");
	return false;
}
static bool badVoid(size_t line, size_t col){
	Report::fatal(line, col, "Invalid type in declaration");
	return false;
}
};

} //End namespace negatron

#endif
