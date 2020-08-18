#include "symbol_table.hpp"
#include "err.hpp"
#include "types.hpp"
namespace negatron{

SymbolTable::SymbolTable(){
	scopeTableChain = new std::list<ScopeTable *>();
}

ScopeTable * SymbolTable::enterScope(){
	ScopeTable * newScope = new ScopeTable();
	scopeTableChain->push_front(newScope);
	return newScope;
}

void SymbolTable::leaveScope(){
	if (scopeTableChain->empty()){
		throw new InternalError("Attempt to pop"
			"empty symbol table");
	}
	scopeTableChain->pop_front();
}

ScopeTable * SymbolTable::getCurrentScope(){
	return scopeTableChain->front();
}


bool SymbolTable::clash(std::string varName){
	bool hasClash = getCurrentScope()->clash(varName);
	return hasClash;
}

Symbol * SymbolTable::find(std::string varName){
	for (ScopeTable * scope : *scopeTableChain){
		Symbol * sym = scope->lookup(varName);
		if (sym != nullptr) { return sym; }
	}
	return nullptr;
}

bool SymbolTable::insert(Symbol * symbol){
	return scopeTableChain->front()->insert(symbol);
}

ScopeTable::ScopeTable(){
	symbols = new HashMap<std::string, Symbol *>();
}

std::string ScopeTable::toString(){
	std::string result = "";
	for (auto entry : *symbols){
		result += entry.second->toString();
		result += "\n";
	}
	return result;
}

bool ScopeTable::clash(std::string varName){
	Symbol * found = lookup(varName);
	if (found != nullptr){
		return true;
	}
	return false;
}

Symbol * ScopeTable::lookup(std::string name){
	auto found = symbols->find(name);
	if (found == symbols->end()){
		return NULL;
	}
	return found->second;
}

bool ScopeTable::insert(Symbol * symbol){
	std::string symName = symbol->getName();
	bool alreadyInScope = (this->lookup(symName) != NULL);
	if (alreadyInScope){
		return false;
	}
	this->symbols->insert(std::make_pair(symName, symbol));
	return true;
}

std::string Symbol::getTypeString(){
	return myType->getString();
}

std::string Symbol::toString(){
	std::string result = "";
	result += "name: " + this->getName();
	result += "\nkind: " + kindToString(this->getKind());
	result += "\ntype: " + this->getTypeString();
	return result + "\n";
}

}
