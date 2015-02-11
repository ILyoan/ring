#include "ast_factory.h"
using namespace ring::ringc::ast;



AstFactory::AstFactory(Session* session)
		: _session(session) {
}


AstNode* AstFactory::assignNode(AstNode* node) {
	_session->ast_table()->add(node);
	return node;
}


Module* AstFactory::createModule() {
	return static_cast<Module*>(assignNode(new Module()));
}


Use* AstFactory::createUse(Ident module_name, Ident module_as, Module* module) {
	return static_cast<Use*>(assignNode(new Use(module_name, module_as, module)));
}


Extern* AstFactory::createExtern(Ident name, TypeId ty) {
	return static_cast<Extern*>(assignNode(new Extern(name, ty)));
}


Let* AstFactory::createLet(bool is_pub, bool is_mut, Ident name, TypeId ty, Expr* expr) {
	return static_cast<Let*>(assignNode(new Let(is_pub, is_mut, name, ty, expr)));
}


ExprEmpty* AstFactory::createExprEmpty() {
	return static_cast<ExprEmpty*>(assignNode(new ExprEmpty()));
}


ExprBlock* AstFactory::createExprBlock() {
	return static_cast<ExprBlock*>(assignNode(new ExprBlock()));
}


ExprFn* AstFactory::createExprFn(
		TypeId ty, const vector<Ident>& args, ExprBlock* body) {
	return static_cast<ExprFn*>(assignNode(new ExprFn(ty, args, body)));
}


ExprIf* AstFactory::createExprIf(Expr* test, Expr* con, Expr* alt) {
	return static_cast<ExprIf*>(assignNode(new ExprIf(test, con, alt)));
}


ExprIdent* AstFactory::createExprIdent(Ident id) {
	return static_cast<ExprIdent*>(assignNode(new ExprIdent(id)));
}


ExprLiteralBasic* AstFactory::createExprLiteralBasic(StrId str_id) {
	return static_cast<ExprLiteralBasic*>(assignNode(new ExprLiteralBasic(str_id)));
}


ExprLiteralArray* AstFactory::createExprLiteralArray(const vector<Expr*>& arr) {
	return static_cast<ExprLiteralArray*>(assignNode(new ExprLiteralArray(arr)));
}


ExprMember* AstFactory::createExprMember(Expr* object, Property property) {
	return static_cast<ExprMember*>(assignNode(new ExprMember(object, property)));
}


ExprCall* AstFactory::createExprCall(Expr* callee, const vector<Expr*>& args) {
	return static_cast<ExprCall*>(assignNode(new ExprCall(callee, args)));
}


ExprUnary* AstFactory::createExprUnary(UnaryOp op, Expr* expr) {
	return static_cast<ExprUnary*>(assignNode(new ExprUnary(op, expr)));
}


ExprBinary* AstFactory::createExprBinary(BinaryOp op, Expr* left, Expr* right) {
	return static_cast<ExprBinary*>(assignNode(new ExprBinary(op, left, right)));
}


ExprLogical* AstFactory::createExprLogical(LogicalOp op, Expr* left, Expr* right) {
	return static_cast<ExprLogical*>(assignNode(new ExprLogical(op, left, right)));
}


ExprConditional* AstFactory::createExprConditional(Expr* test, Expr* con, Expr* alt) {
	return static_cast<ExprConditional*>(assignNode(new ExprConditional(test, con, alt)));
}


ExprAssignment* AstFactory::createExprAssignment(AssignmentOp op, Expr* left, Expr* right) {
	return static_cast<ExprAssignment*>(assignNode(new ExprAssignment(op, left, right)));
}


TypeId AstFactory::getPrimTypeId(const PrimTypeKey& prim_type_key) {
	return _session->type_table()->getPrimTypeId(prim_type_key);
}


TypeId AstFactory::getFuncTypeId(const FuncTypeKey& func_type_key) {
	return _session->type_table()->getFuncTypeId(func_type_key);
}


TypeId AstFactory::getArrayTypeId(const ArrayTypeKey& array_type_key) {
	return _session->type_table()->getArrayTypeId(array_type_key);
}