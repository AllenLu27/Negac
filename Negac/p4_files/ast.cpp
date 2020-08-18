#include "ast.hpp"

namespace negatron{

const DataType * FnDeclNode::getDeclaredType() const {
	const Symbol * mySymbol = myID->getSymbol();
	const DataType * type = mySymbol->getType();
	return type;
}

}
