#include "symbol_table.hpp"
namespace negatron{

ScopeTable::ScopeTable(){
	symbols = new HashMap<std::string, Symbol *>();
}

bool ScopeTable::push(std::string entry_id, Symbol* m_symbol){
	if(IsInScope(entry_id)){
		return(0);
	}
	else{
		this->symbols->insert({entry_id, m_symbol});
		return(1);
	}
}

bool ScopeTable::IsInScope(std::string entry_id){
	if(this->symbols->count(entry_id) == 1){
		return(1);
	}
	else{
		return(0);
	}
}

Symbol* ScopeTable::lookUp(std::string entry_id){
	if(IsInScope(entry_id)){
		return(this->symbols->at(entry_id));
	}
	else{
		return(nullptr);
	}
 }

 int ScopeTable::table_size(){
	 return(this->symbols->size());
 }

HashMap<std::string, Symbol *> * ScopeTable::getTable(){
	return(symbols);
}

void ScopeTable::remove(std::string entry_id){
	this->symbols->erase(entry_id);
}

//SymbolTable methods
SymbolTable::SymbolTable(){
	//TODO: implement the list of hashtables approach
	// to building a symbol table:
	// Upon entry to a scope a new scope table will be
	// entered into the front of the chain and upon exit the
	// latest scope table will be removed from the front of
	// the chain.
	anyError = false;
	scopeTableChain = new std::list<ScopeTable *>();
}

void SymbolTable::addScope(ScopeTable* newScope){
	this->scopeTableChain->push_front(newScope);
}

ScopeTable* SymbolTable::dropScope(){
		ScopeTable* currentScope = scopeTableChain->front();
		this->scopeTableChain->pop_front();
		return(currentScope);
}

bool SymbolTable::addSymboltoCurrent(std::string entry_id, Symbol* m_symbol){
	ScopeTable* currentScope = scopeTableChain->front();
	if(currentScope->push(entry_id, m_symbol)){
		return(1);
	}
	else{
		return(0);
	}
}

void SymbolTable::deleteSymbolInCurrent(std::string entry_id){
	ScopeTable* currentScope = scopeTableChain->front();
	currentScope->remove(entry_id);
}

bool SymbolTable::IsInCurrent(std::string entry_id){
	ScopeTable* currentScope = scopeTableChain->front();
	if(currentScope->IsInScope(entry_id)){
		return(1);
	}
	else{
	  return(0);
   }
}

bool SymbolTable::IsStructDeclared(std::string temp){
		ScopeTable* globalScope = scopeTableChain->back();
		if(globalScope->IsInScope(temp)){
			return(1);
		}
		else{
			return(0);
		}
}

Symbol* SymbolTable::getOccurence(std::string my_id){
	ScopeTable* my_scope = nullptr;
	for (auto scope : *scopeTableChain){
		if(scope->IsInScope(my_id)){
			my_scope = scope;
			break;
		}
	}
	if(my_scope != nullptr){
		return(my_scope->lookUp(my_id));
	}
	else{
		return(nullptr);
	}
}

void SymbolTable::thereIsError(){
	anyError = true;
}

bool SymbolTable::getFlag(){
	return(anyError);
}

}

//Symbol definition
negatron::Symbol::Symbol(std::string temp_kind, std::string temp_type){
    	m_type = temp_type;
			m_kind = temp_kind;
			// m_id = "";
			m_args = nullptr;
			struct_fields = nullptr;
}

std::string negatron::Symbol::getType(){
	return(m_type);
}

// std::string negatron::Symbol::getId(){
// 	return(m_id);
// }

std::string negatron::Symbol::getKind(){
	return(m_kind);
}

std::list<std::string>* negatron::Symbol::getArgs(){
	return(m_args);
}

// void negatron::Symbol::setID(std::string temp_ID){
// 	m_id = temp_ID;
// }

void negatron::Symbol::setArgs(std::list<std::string>* temp_args){
	m_args = temp_args;
}

void negatron::Symbol::addFields(std::string entry_id, Symbol* m_symbol){
	this->struct_fields->insert({entry_id, m_symbol});
}

void negatron::Symbol::setUpFields(){
	struct_fields = new HashMap<std::string, Symbol *>();
}

bool negatron::Symbol::IsInfields(std::string entry_id){
	if(this->struct_fields->count(entry_id) == 1){
		return(1);
	}
	else{
		return(0);
	}
}

std::string negatron::Symbol::typeInField(std::string entry_id){
	if(IsInfields(entry_id)){
			Symbol* my_sym = this->struct_fields->at(entry_id);
			return(my_sym->getType());
	}
	else{
			return "none";
	}
}

HashMap<std::string, negatron::Symbol *> * negatron::Symbol::getFieldTable(){
	return(struct_fields);
}

void negatron::Symbol::setFields(HashMap<std::string, Symbol *> * temp){
	struct_fields = temp;
}

negatron::Symbol* negatron::Symbol::getOccurence(std::string my_id){
		if(IsInfields(my_id)){
			Symbol* my_sym = this->struct_fields->at(my_id);
			return(my_sym);
		}
	else{
		return(nullptr);
	}
}
