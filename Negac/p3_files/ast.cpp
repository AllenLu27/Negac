#include "ast.hpp"

using namespace negatron;

size_t ASTNode::line() { return this->l; }
size_t ASTNode::col() { return this->c; }

StructDeclNode::StructDeclNode(size_t l, size_t c,
  IDNode * id, std::list<VarDeclNode *> * fieldsIn)
: DeclNode(l,c), myFields(fieldsIn), myID(id){
}

FormalDeclNode::FormalDeclNode(size_t l, size_t c,
  TypeNode * typeIn, IDNode * IDin)
: VarDeclNode(l,c,typeIn,IDin){
}

StructTypeNode::StructTypeNode(size_t l, size_t c,
  IDNode * structID, bool isRefIn)
: TypeNode(l,c, isRefIn), myStructID(structID){
}

DeclNode::DeclNode(size_t l, size_t c)
: StmtNode(l, c){
}

VarDeclNode::VarDeclNode(size_t l, size_t c,
  TypeNode * typeIn, IDNode * IDIn)
: DeclNode(l, c), myType(typeIn), myID(IDIn){
}

AssignExpNode::AssignExpNode(size_t l, size_t c,
  LocNode * dstIn, ExpNode * srcIn)
: ExpNode(l,c), myDst(dstIn), mySrc(srcIn){
}

AssignStmtNode::AssignStmtNode(size_t l, size_t c, AssignExpNode * expIn)
: StmtNode(l, c), myExp(expIn){
}

PostDecStmtNode::PostDecStmtNode(size_t l, size_t c, LocNode * locIn)
: StmtNode(l, c), myLoc(locIn){
}

InputStmtNode::InputStmtNode(size_t l, size_t c, LocNode * locIn)
: StmtNode(l, c), myLoc(locIn){
}

OutputStmtNode::OutputStmtNode(size_t l, size_t c, ExpNode * expIn)
: StmtNode(l, c), myExp(expIn){
}

IfStmtNode::IfStmtNode(size_t l, size_t c, ExpNode * condIn,
  std::list<StmtNode *> * bodyIn)
: StmtNode(l, c), myCond(condIn), myBody(bodyIn){
}

IfElseStmtNode::IfElseStmtNode(size_t l, size_t c,
  ExpNode * condIn,
  std::list<StmtNode *> * bodyTrueIn,
  std::list<StmtNode *> * bodyFalseIn)
: StmtNode(l, c), myCond(condIn),
  myBodyTrue(bodyTrueIn), myBodyFalse(bodyFalseIn){
}

WhileStmtNode::WhileStmtNode(size_t l, size_t c,
  ExpNode * condIn, std::list<StmtNode *> * bodyIn)
: StmtNode(l, c), myCond(condIn), myBody(bodyIn){
}

ReturnStmtNode::ReturnStmtNode(size_t l, size_t c, ExpNode * expIn)
: StmtNode(l, c), myExp(expIn)
{
}

CallStmtNode::CallStmtNode(size_t l, size_t c,
  CallExpNode * expIn)
: StmtNode(l, c), myCallExp(expIn){
}

DotAccessNode::DotAccessNode(size_t l, size_t c, LocNode * baseIn, IDNode * fieldIn)
: LocNode(l,c), myBase(baseIn), myField(fieldIn){
}

LocNode::LocNode(size_t l, size_t c)
: ExpNode(l, c){
}

std::string VarDeclNode::getType(){
  return(myType->baseType());
}
