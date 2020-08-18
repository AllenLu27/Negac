#ifndef NEGA_DATA_TYPES
#define NEGA_DATA_TYPES

#include <list>
#include <sstream>
#include "err.hpp"
#include "errors.hpp"

#include <unordered_map>

#ifndef NEGA_HASH_MAP_ALIAS
// Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;
#endif


namespace negatron{

class ASTNode;

class VarType;
class FnType;
class ErrorType;
class TupleType;

enum BaseType{
	INT, VOID, BOOL, STR
};

//This class is the superclass for all negatron types. You
// can get information about which type is implemented
// concretely using the as<X> functions, or query information
// using the is<X> functions.
class DataType{
public:
	virtual std::string getString() const = 0;
	virtual const VarType * asVar() const { return nullptr; }
	virtual const FnType * asFn() const { return nullptr; }
	virtual const TupleType * asTuple() const { return nullptr; }
	virtual const ErrorType * asError() const { return nullptr; }
	virtual bool isVoid() const { return false; }
	virtual bool isInt() const { return false; }
	virtual bool isBool() const { return false; }
protected:
};

//This DataType subclass is the superclass for all negatron types. 
// Note that there is exactly one instance of this 
class ErrorType : public DataType{
public:
	static ErrorType * produce(){
		//Note: this static member will only ever be initialized 
		// ONCE, no matter how many times the function is called.
		// That means there will only ever be 1 instance of errorType
		// in the entire codebase.
		static ErrorType * error = new ErrorType();
		
		return error;
	}
	virtual const ErrorType * asError() const override { return this; }
	virtual std::string getString() const override { 
		return "ERROR";
	}
private:
	ErrorType(){ 
		/* private constructor, can only 
		be called from produce */
	}
	size_t line;
	size_t col;
};

//DataType subclass for all scalar types 
class VarType : public DataType{
public:
	static VarType * VOID(){
		return produce(BaseType::VOID);
	}

	//Create a scalar type. If that type already exists,
	// return the known instance of that type. Making sure
	// there is only 1 instance of a class for a given set
	// of fields is known as the "flyweight" design pattern
	// and ensures that the memory needs of a program are kept
	// down: rather than having a distinct type for every base
	// INT (for example), only one is constructed and kept in
	// the flyweights list. That type is then re-used anywhere
	// it's needed. 

	//Note the use of the static function declaration, which 
	// means that no instance of VarType is needed to call
	// the function.
	static VarType * produce(BaseType base){
		//Note the use of the static local variable, which
		//means that the flyweights variable persists between
		// multiple calls to this function (it is essentially
		// a global variable that can only be accessed
		// in this function).
		static std::list<VarType *> flyweights;
		for(VarType * fly : flyweights){
			if (fly->getBaseType() == base){
				return fly;
			}
		}
		VarType * newType = new VarType(base);
		flyweights.push_back(newType);
		return newType;
	}
	const VarType * asVar() const {
		return this;
	}
	VarType * asVar(){
		return this;
	}
	bool isInt() const override {
		return myBaseType == INT;
	}
	bool isBool() const override {
		return myBaseType == BOOL;
	}
	virtual bool isVoid() const override { 
		return myBaseType == BaseType::VOID; 
	}
	virtual BaseType getBaseType() const { return myBaseType; }
	virtual std::string getString() const override;
private:
	VarType(BaseType base) 
	: myBaseType(base){ }
	BaseType myBaseType;
};

// DataType subclass for tuples of types (i.e. lists of more
// than 1 type. This is useful for expressing the types of 
// formals lists and argument lists (and for matching them up)
class TupleType : public DataType{
public:
	TupleType(std::list<const DataType *> * eltTypesIn)
	: eltTypes(eltTypesIn){
	}
	DataType * data;
	std::string getString() const override{
		std::string res = "";
		bool first = true;
		for (auto elt : *eltTypes){
			if (first){ first = false; }
			else { res += ","; }
			res += elt->getString();
		}
		return res;
	}
	virtual const TupleType * asTuple() const { return this; }

	std::list<const DataType *> * getElts () const {
		return eltTypes;
	}
private:
	std::list<const DataType *> * eltTypes;
};


//DataType subclass to represent the type of a function. It will
// have a list of argument types and a return type. 
class FnType : public DataType{
public:
	FnType(const TupleType * formalsIn, const DataType * retTypeIn) 
	: DataType(),
	  myFormalTypes(formalsIn),
	  myRetType(retTypeIn)
	{
	}
	std::string getString() const override{
		std::string result = "";
		bool first = true;
		result += myFormalTypes->getString();
		result += "->";
		result += myRetType->getString();
		return result;
	}
	virtual const FnType * asFn() const override { return this; }

	const DataType * getReturnType() const {
		return myRetType;
	}
	const TupleType * getFormalTypes() const {
		return myFormalTypes;
	}
private:
	const TupleType * myFormalTypes;
	const DataType * myRetType;
};

// An instance of this class will be passed over the entire
// AST. Rather than attaching types to each node, the 
// TypeAnalysis class contains a map from each ASTNode to it's
// DataType. Thus, instead of attaching a type field to most nodes,
// one can instead map the node to it's type, or lookup the node
// in the map.
class TypeAnalysis {
public:
	TypeAnalysis(){
		hasError = false;
	}
	//The type analysis has an instance variable to say whether
	// the analysis failed or not. Setting this variable is much
	// less of a pain than passing a boolean all the way up to the
	// root during the TypeAnalysis pass. 
	bool passed(){
		return !hasError;
	}

	//Set the type of a node. Note that the function name is 
	// overloaded: this 2-argument nodeType puts a value into the
	// map with a given type. 
	void nodeType(const ASTNode * node, const DataType * type){
		nodeToType[node] = type;
	}

	//Gets the type of a node already placed in the map. Note
	// that this function name is overloaded: the 1-argument nodeType
	// gets the type of the given node out of the map.
	const DataType * nodeType(const ASTNode * node){
		const DataType * res = nodeToType[node];
		if (res == nullptr){
			const char * msg = "No type for node ";
			throw new InternalError(msg);
		}
		return nodeToType[node];
	}

	//The following functions all report and error and 
	// tell the object that the analysis has failed. 

	void badArgMatch(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Type of actual does not match"
			" type of formal");
	}
	void badMathOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Arithmetic operator applied"
			" to invalid operand");
	}
	void badMathOpr(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Arithmetic operator applied"
			" to incompatible operands");
	}
	void badArgCount(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Function call with wrong"
			" number of args");
	}
	void badCallee(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to call a "
			"non-function");
	}
	void badAssignOpr(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid assignment operation");
	}
	void badAssignOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid assignment operand");
	}
	void badEqOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid equality operand");
	}
	void badEqOpr(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid equality operation");
	}
	void badLogicOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Logical operator applied to"
			" non-bool operand");
	}
	void badNoRet(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Missing return value");
	}
	void badRelOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Relational operator applied to"
			" non-numeric operand");
	}
	void badWriteVoid(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Attempt to write void");
	}

	void badWhileCond(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Non-bool expression used as"
			" a while condition");
	}
	void badIfCond(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Non-bool expression used as"
			" an if condition");
	}
	void badRetValue(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Bad return value");
	}
	void extraRetValue(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Return with a value in void"
			" function");
	}
	void writeFn(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to write a function");
	}
	
	void readFn(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to read a function");
	}
private:
	HashMap<const ASTNode *, const DataType *> nodeToType;
	bool hasError;
};

}

#endif
