#include "ast.hpp"

namespace negatron{

/*
doIndent is declared static, which means that it can
only be called in this file (its symbol is not exported).
*/
static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

/*
In this code, the intention is that functions are grouped
into files by purpose, rather than by class.
If you're used to having all of the functions of a class
defined in the same file, this style may be a bit disorienting,
though it is legal. Thus, we can have
ProgramNode::unparse, which is the unparse method of ProgramNodes
defined in the same file as DeclNode::unparse, the unparse method
of DeclNodes.
*/


void ProgramNode::unparse(std::ostream& out, int indent){
	/* Oh, hey it's a for-each loop in C++!
	   The loop iterates over each element in a collection
	   without that gross i++ nonsense.
	 */
	for (auto global : *myGlobals){
		/* The auto keyword tells the compiler
		   to (try to) figure out what the
		   type of a variable should be from
		   context. here, since we're iterating
		   over a list of DeclNode *s, it's
		   pretty clear that global is of
		   type DeclNode *.
		*/
		global->unparse(out, indent);
	}
}

void FormalDeclNode::unparse(std::ostream& out, int indent){
	this->my_type->unparse(out, 0);
	out<<" ";
	this->my_id->unparse(out, 0);
}

void FormalDeclNodeList::unparse(std::ostream& out, int indent){
if(this->my_list != nullptr)
{
	int list_size = this->my_list->size();
	int counter = 0;
	for (auto formal_decl : *my_list){
		counter = counter + 1;
		formal_decl->unparse(out, indent);
		if(counter != list_size)
		{
			out<<", ";
		}
	}
  }
}

void StmtListNode::unparse(std::ostream& out, int indent){
	for (auto statement : *my_list){
		statement->unparse(out, indent);
		out<<'\n';
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	out << ";";
}

void IDNode::unparse(std::ostream& out, int indent){
	out << this->myStrVal;
}

void DotAccessNode::unparse(std::ostream& out, int indent){
	int list_size = this->m_list->size();
	int counter = 0;
	for (auto id : *m_list){
		counter = counter + 1;
		id->unparse(out, indent);
		if(counter != list_size)
		{
			out<<".";
		}
	}
}

void IntLitNode::unparse(std::ostream& out, int indent){
	out << this->myNumVal;
}

void StrLitNode::unparse(std::ostream& out, int indent){
	// out << "\"";
	out << this->myStrVal;
	// out << "\"";
}

void NullRefNode::unparse(std::ostream& out, int indent){
	out << "nullref";
}

void NotNode::unparse(std::ostream& out, int indent){
	out << "(";
	out << "!";
	this->m_exp->unparse(out, 0);
	out << ")";
}

void NegNode::unparse(std::ostream& out, int indent){
	out << "(";
	out << "-";
	this->m_exp->unparse(out, 0);
	out << ")";
}

void FalseNode::unparse(std::ostream& out, int indent){
	out << "false";
}

void CallExpNode::unparse(std::ostream& out, int indent){
	if(args == nullptr)
	{
		this->myID->unparse(out, 0);
		out<<"();";
	}
	else
	{
		this->myID->unparse(out, 0);
		int list_size = this->args->size();
		int counter = 0;
		out<<"(";
		for (auto arg : *args){
			 counter = counter + 1;
			/* The auto keyword tells the compiler
			   to (try to) figure out what the
			   type of a variable should be from
			   context. here, since we're iterating
			   over a list of DeclNode *s, it's
			   pretty clear that global is of
			   type DeclNode *.
			*/
			arg->unparse(out, indent);
			if(counter != list_size)
			{
				out<<", ";
			}
		}
		out<<");";
	}
}

void BinaryExpNode::unparse(std::ostream& out, int indent){
	out <<"(";
	lhs->unparse(out, indent);
	out<<op();
	rhs->unparse(out, indent);
	out <<")";
}

void AssignExpNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->m_loc->unparse(out, 0);
	out<<" = ";
	myExpr->unparse(out, indent);
	out<<";";
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	if(this->myIsReference == 1)
	{
		out<<"int &";
	}
	else{
		out << "int";
	}
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	if(this->myIsReference == 1)
	{
		out<<"bool &";
	}
	else{
		out << "bool";
	}
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	out<<"void";
}

void StructTypeNode::unparse(std::ostream& out, int indent){
	if(this->myIsReference == 1)
	{
		out<<"struct ";
		this->s_id->unparse(out,0);
		out<<" &";
	}
	else{
		out << "struct ";
		this->s_id->unparse(out,0);
		out <<" ";
	}
}

void AssignStmtNode::unparse(std::ostream& out, int indent){
	(this->assignment)->unparse(out, indent);
}

void CallStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	(this->statement)->unparse(out, indent);
}

void ReturnStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	if(this->expression == nullptr)
	{
		out<<"return;";
	}
	else
	{
		out<<"return ";
		this->expression->unparse(out, indent);
		out<<";";
	}
}

void FnBodyNode::unparse(std::ostream& out, int indent){
	this->fn_body_stmts->unparse(out,indent);
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	this->fn_type->unparse(out, 0);
	out<<" ";
	this->fn_id->unparse(out, 0);
	out<<"(";
	this->fn_list->unparse(out, 0);
	out<<")";
	out<<"{";
	out<<"\n";
	this->fn_body->unparse(out, 1);
	out<<"}";
	out<<"\n\n";
}

void VarDeclListNode::unparse(std::ostream& out, int indent){
	for (auto var_decl : *var_list){
		var_decl->unparse(out, indent);
		out<<"\n";
	}
}

void StructDeclNode::unparse(std::ostream& out, int indent){
	out<<"struct ";
	this->s_id->unparse(out, 0);
	out<<" {";
	out<<"\n";
	this->s_list->unparse(out, 1);
	out<<"};";
	out<<"\n\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, 1);
	out<<"if (";
	this->expression->unparse(out, 0);
	out<<"){\n";
	this->if_list->unparse(out, 2);
	doIndent(out, 1);
	out<<"} ";
	out<<"else {\n";
	this->else_list->unparse(out, 2);
	out<<"\n";
	doIndent(out, 1);
	out<<"}";
	out<<"\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, 1);
	out<<"if(";
	this->expression->unparse(out, 0);
	out<<"){\n";
	this->if_list->unparse(out, 2);
	out<<"\n";
	doIndent(out, 1);
	out<<"}";
	out<<"\n";
}

void InputStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out<<"input ";
	this->my_str->unparse(out, 0);
	out<<";";
}

void OutputStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out<<"output ";
	this->expression->unparse(out, 0);
	out<<";";
}

void PostDeclStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->m_loc->unparse(out,0);
	out<<"--;";
}

void WhileStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out<<"while (";
	this->expression->unparse(out, 0);
	out<<"){\n";
	this->w_list->unparse(out, 2);
	doIndent(out, 1);
	out<<"}";
}

} // End namespace negatron
