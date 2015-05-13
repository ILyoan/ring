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


LiteralType ast::convertLiteralType(Token::TokenType token_type) {
	assert(TokenUtil::isLiteral(token_type));

	switch (token_type) {
		case Token::LIT_FALSE: return LIT_FALSE;
		case Token::LIT_TRUE: return LIT_TRUE;
		case Token::LIT_NUMERIC: return LIT_NUM;
		case Token::LIT_STRING: return LIT_STRING;
	}
	AST_FATAL("Expected a literal, but found %s",
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


bool Type::isPrimitiveType() const {
	return _type == TYPE_PRIMITIVE;
}


bool Type::isFunctionType() const {
	return _type == TYPE_FUNCTION;
}


bool Type::isNil() const {
	return (isPrimitiveType() && static_cast<const PrimitiveType*>(this)->isNil());
}


PrimitiveType::PrimitiveType(PrimitiveTypeType type)
		: Type(TYPE_PRIMITIVE)
		, _primitive_type(type) {
}


bool PrimitiveType::isNil() const {
	return _primitive_type == PRIM_TYPE_NIL;
}


FunctionType::FunctionType(const vector<TypeId>& args, TypeId ret)
		: Type(TYPE_FUNCTION)
		, _args(args)
		, _ret(ret) {
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


Extern::Extern(Ident name, TypeId type_id)
		: AstNode(AST_EXTERN)
		, _name(name)
		, _type_id(type_id) {
}


Stmt::Stmt(AstNodeType node_type)
		: AstNode(node_type) {
}


Let::Let(bool is_pub, bool is_mut, Ident name, TypeId type_id, Expr* expr)
		: Stmt(AST_LET)
		, _is_pub(is_pub)
		, _is_mut(is_mut)
		, _name(name)
		, _type_id(type_id)
		, _expr(expr) {
}


bool Let::inModuleScope() const {
	return _parent->isModule();
}


Expr::Expr(AstNodeType node_type)
		: Stmt(node_type)
		, _type_id() {
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


ExprFn::ExprFn(TypeId type_id, const vector<Ident>& args, ExprBlock* body)
		: Expr(AST_EXPR_FN)
		, _args(args)
		, _body(body) {
	Expr::type_id(type_id);
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


ExprLiteral::ExprLiteral(LiteralType lit_type, StrId str_id)
		: Expr(AST_EXPR_LITERAL)
		, _lit_type(lit_type)
		, _str_id(str_id) {
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
