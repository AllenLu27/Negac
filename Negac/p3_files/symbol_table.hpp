#ifndef NEGA_SYMBOL_TABLE_HPP
#define NEGA_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace negatron{

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols
// exist for the lifetime of a scope in the
// symbol table.
class Symbol {
	//TODO add the fields that
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields
	std::string m_type;
	// std::string m_id;
	std::list<std::string>* m_args;
	std::string m_kind;
	HashMap<std::string, Symbol *> * struct_fields;
public:
				Symbol(std::string temp_kind, std::string temp_type);
				std::string getType();
				std::string getKind();
				std::list<std::string>* getArgs();
				void setArgs(std::list<std::string>* temp_args);
				void addFields(std::string entry_id, Symbol* m_symbol);
				void setUpFields();
				bool IsInfields(std::string entry_id);
				std::string typeInField(std::string entry_id);
				Symbol* getOccurence(std::string my_id);
				HashMap<std::string, Symbol *> * getFieldTable();
				void setFields(HashMap<std::string, Symbol *> * temp);
};

//A single scope. The symbol table is broken down into a
// chain of scope tables, and each scope table holds
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		//TODO: add functions for looking up symbols
		// and/or returning information to indicate
		// that the symbol does not exist within the
		// current scope.
		HashMap<std::string, Symbol *> * getTable();
		int table_size();
		Symbol* lookUp(std::string entry_id);
		bool IsInScope(std::string entry_id);
		bool push(std::string entry_id, Symbol* m_symbol);
		void remove(std::string entry_id);
	private:
		HashMap<std::string, Symbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc.
		bool addSymboltoCurrent(std::string entry_id, Symbol* m_symbol);
		void deleteSymbolInCurrent(std::string entry_id);
		bool IsStructDeclared(std::string temp);
		bool IsInCurrent(std::string entry_id);
		Symbol* getOccurence(std::string my_id);
		void addScope(ScopeTable* newScope);
	 	ScopeTable* dropScope();
		void thereIsError();
		bool getFlag();
	private:
		bool anyError;
		std::list<ScopeTable *> * scopeTableChain;
};


}

#endif
