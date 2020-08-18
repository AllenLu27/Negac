#include "types.hpp"
#include <list>
#include <sstream>

namespace negatron{

std::string VarType::getString() const{
	std::string res = "";
	switch(myBaseType){
	case INT:
		res += "int";
		break;
	case BOOL:
		res += "bool";
		break;
	case BaseType::VOID:
		res += "void";
		break;
	case STR:
		res += "string";
		break;
	}
	return res;
}

} //End namespace
