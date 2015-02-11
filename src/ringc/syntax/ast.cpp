#include "ast.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <algorithm>
using namespace std;
using namespace ring::ringc::ast;


#define AST_FATAL(...) do { fprintf(stderr, __VA_ARGS__); exit(1); } while (0)


AssignmentOp ast::convertAssignmentOp(Token::TokenType token_type) {
	assert(TokenUtil::isAssigmentOp(token_type));

	switch (token_type) {
		case Token::ASSIGN: return AO_ASSIGN;
		case Token::ASSIGN_ADD: return AO_ADD;
		case Token::ASSIGN_SUB: return AO_SUB;
		case Token::ASSIGN_MUL: return AO_MUL;
		case Token::ASSIGN_DIV: return AO_DIV;
		case Token::ASSIGN_MOD: return AO_MOD;
		case Token::ASSIGN_LSH: return AO_LSH;
		case Token::ASSIGN_RSH: return AO_RSH;
		case Token::ASSIGN_BITWISE_OR: return AO_BITWISE_OR;
		case Token::ASSIGN_BITWISE_XOR: return AO_BITWISE_XOR;
		case Token::ASSIGN_BITWISE_AND: return AO_BITWISE_AND;
	}
	AST_FATAL("Expected an assignment operator, but found %s",
		Token(token_type).toString().c_str());
}


LogicalOp ast::convertLogicalOp(Token::TokenType token_type) {
	assert(TokenUtil::isLogicalOp(token_type));

	switch (token_type) {
		case Token::OROR: return LO_OR;
		case Token::ANDAND: return LO_AND;
	}
	AST_FATAL("Expected a logical operator, but found %s",
		Token(token_type).toString().c_str());
}


BinaryOp ast::convertBinaryOp(Token::TokenType token_type) {
	assert(TokenUtil::isBinaryOp(token_type));

	switch (token_type) {
		case Token::ADD: return BO_ADD;
		case Token::SUB: return BO_SUB;
		case Token::MUL: return BO_MUL;
		case Token::DIV: return BO_DIV;
		case Token::MOD: return BO_MOD;
		case Token::EXP: return BO_EXP;
		case Token::OR: return BO_BITWISE_OR;
		case Token::HAT: return BO_BITWISE_XOR;
		case Token::AND: return BO_BITWISE_AND;
		case Token::LSH: return BO_LSH;
		case Token::RSH: return BO_RSH;
		case Token::EQ: return BO_EQ;
		case Token::NE: return BO_NE;
		case Token::LT: return BO_LT;
		case Token::LE: return BO_LE;
		case Token::GE: return BO_GE;
		case Token::GT: return BO_GT;
	}
	AST_FATAL("Expected a binary operator, but found %s",
		Token(token_type).toString().c_str());
}


UnaryOp ast::convertUnaryOp(Token::TokenType token_type) {
	assert(TokenUtil::isUnaryOp(token_type));

	switch (token_type) {
		case Token::ADD: return UO_PLUS;
		case Token::SUB: return UO_MINUS;
		case Token::NOT: return UO_NOT;
		case Token::TILDE: return UO_BITWISE_NOT;
	}
	AST_FATAL("Expected an unary operator, but found %s",
		Token(token_type).toString().c_str());
}


Ident::Ident(NameId name_id)
		: _name_id(name_id)
		, _symbol_id() {
}


Ident::Ident(NameId name_id, SymbolId symbol_id)
		: _name_id(name_id)
		, _symbol_id(symbol_id) {
}



Type::Type(TypeType type)
		: _type(type) {
}


bool Type::isPrimType() const {
	return _type == TYPE_PRIM;
}


bool Type::isFuncType() const {
	return _type == TYPE_FUNC;
}


bool Type::isArrayType() const {
	return _type == TYPE_ARRAY;
}


bool Type::isNil() const {
	return (isPrimType() && static_cast<const TypePrim*>(this)->isNil());
}


bool Type::isInt() const {
	return (isPrimType() && static_cast<const TypePrim*>(this)->isInt());
}


bool Type::isBool() const {
	return (isPrimType() && static_cast<const TypePrim*>(this)->isBool());
}


TypePrim::TypePrim(TypePrimType type)
		: Type(TYPE_PRIM)
		, _prim_type(type) {
}


bool TypePrim::isNil() const {
	return _prim_type == PRIM_TYPE_NIL;
}


bool TypePrim::isInt() const {
	return _prim_type == PRIM_TYPE_INT;
}


bool TypePrim::isBool() const {
	return _prim_type == PRIM_TYPE_BOOL;
}


TypeFunc::TypeFunc(const vector<TypeId>& ty_args, TypeId ty_ret)
		: Type(TYPE_FUNC)
		, _ty_args(ty_args)
		, _ty_ret(ty_ret) {
}


TypeArray::TypeArray(TypeId ty_elem)
		: Type(TYPE_ARRAY)
		, _ty_elem(ty_elem) {
}


Property::Property(Expr* index)
		: _property_type(PROP_INDEX)
		, _index(index)
		, _ident() {
}


Property::Property(StrId ident)
		: _property_type(PROP_INDEX)
		, _index(NULL)
		, _ident(ident) {
}


bool Property::isIndex() const {
	return _property_type == PROP_INDEX;
}


bool Property::isIdent() const {
	return _property_type == PROP_IDENT;
}


AstNode::AstNode(AstNodeType node_type)
		: _parent(NULL)
		, _node_id()
		, _node_type(node_type) {
}


bool AstNode::isModule() const {
	return _node_type == AST_MODULE;
}


bool AstNode::isUse() const {
	return _node_type == AST_USE;
}


bool AstNode::isStmt() const {
	return isLet() || isExpr();
}


bool AstNode::isLet() const {
	return _node_type == AST_LET;
}


bool AstNode::isExpr() const {
	return _node_type >= AST_EXPR_EMPTY && _node_type <= AST_EXPR_ASSIGNMENT;
}


bool AstNode::isExprFn() const {
	return _node_type == AST_EXPR_FN;
}


Module::Module()
	: AstNode(AST_MODULE) {
}


void Module::addUse(Use* use) {
	_uses.push_back(use);
}


void Module::addExtern(Extern* ext) {
	_exts.push_back(ext);
}


void Module::addDecl(Let* decl) {
	_decls.push_back(decl);
}


Use::Use(Ident module_name, Ident module_as, Module* module)
		: AstNode(AST_USE)
		, _module_name(module_name)
		, _module_as(module_as)
		, _module(module) {
}


Extern::Extern(Ident name, TypeId ty)
		: AstNode(AST_EXTERN)
		, _name(name)
		, _ty(ty) {
}


Stmt::Stmt(AstNodeType node_type)
		: AstNode(node_type) {
}


Let::Let(bool is_pub, bool is_mut, Ident name, TypeId ty, Expr* expr)
		: Stmt(AST_LET)
		, _is_pub(is_pub)
		, _is_mut(is_mut)
		, _name(name)
		, _ty(ty)
		, _expr(expr) {
}


bool Let::inModuleScope() const {
	return _parent->isModule();
}


Expr::Expr(AstNodeType node_type)
		: Stmt(node_type)
		, _ty() {
}


ExprEmpty::ExprEmpty()
		: Expr(AST_EXPR_EMPTY) {
}


ExprBlock::ExprBlock()
		: Expr(AST_EXPR_BLOCK) {
}


void ExprBlock::addStatement(Stmt* stmt) {
	_stmts.push_back(stmt);
}


ExprFn::ExprFn(TypeId ty, const vector<Ident>& args, ExprBlock* body)
		: Expr(AST_EXPR_FN)
		, _args(args)
		, _body(body) {
	Expr::ty(ty);
}


ExprIf::ExprIf(Expr* test, Expr* con, Expr* alt)
		: Expr(AST_EXPR_IF)
		, _test(test)
		, _con(con)
		, _alt(alt) {
}


ExprIdent::ExprIdent(Ident id)
		: Expr(AST_EXPR_IDENT)
		, _id(id) {
}


ExprLiteral::ExprLiteral(LiteralType lit_type)
		: Expr(AST_EXPR_LITERAL)
		, _lit_type(lit_type) {
}


bool ExprLiteral::isBasic() const {
	return _lit_type == LIT_BASIC;
}


bool ExprLiteral::isArray() const {
	return _lit_type == LIT_ARRAY;
}


ExprLiteralBasic::ExprLiteralBasic(StrId str_id)
		: ExprLiteral(LIT_BASIC)
		, _str_id(str_id) {
}


ExprLiteralArray::ExprLiteralArray(const vector<Expr*>& arr)
		: ExprLiteral(LIT_ARRAY)
		, _arr(arr) {
}


ExprMember::ExprMember(Expr* object, Property property)
		: Expr(AST_EXPR_MEMBER)
		, _object(object)
		, _property(property) {
}


ExprCall::ExprCall(Expr* callee, const vector<Expr*>& args)
		: Expr(AST_EXPR_CALL)
		, _callee(callee)
		, _args(args) {
}


ExprUnary::ExprUnary(UnaryOp op, Expr* expr)
		: Expr(AST_EXPR_UNARY)
		, _op(op)
		, _expr(expr) {
}


ExprBinary::ExprBinary(BinaryOp op, Expr* left, Expr* right)
		: Expr(AST_EXPR_BINARY)
		, _op(op)
		, _left(left)
		, _right(right) {
}


ExprLogical::ExprLogical(LogicalOp op, Expr* left, Expr* right)
		: Expr(AST_EXPR_LOGICAL)
		, _op(op)
		, _left(left)
		, _right(right) {
}


ExprConditional::ExprConditional(Expr* test, Expr* con, Expr* alt)
		: Expr(AST_EXPR_CONDITIONAL)
		, _test(test)
		, _con(con)
		, _alt(alt) {
}


ExprAssignment::ExprAssignment(AssignmentOp op, Expr* left, Expr* right)
		: Expr(AST_EXPR_ASSIGNMENT)
		, _op(op)
		, _left(left)
		, _right(right) {
}
