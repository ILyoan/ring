#ifndef RING_RINGC_SYNTAX_AST_VISITOR_H
#define RING_RINGC_SYNTAX_AST_VISITOR_H


#include "ast.h"
using namespace ring::ringc::ast;


#define VISIT_ON_PRE(type, value) \
	if (_terminate) { return; }\
	_skip_children = false;\
	onVisitPreNode(value);\
	onVisitPre##type(value);\
	if (_skip_children) { return ; }\


#define VISIT_CHILD(type, child) \
	if (_terminate) { return; }\
	this->visit##type(child);\


#define VISIT_ON_POST(type, value) \
	if (_terminate) { return; }\
	onVisitPost##type(value);\
	onVisitPostNode(value);\


#define ADD_VISITOR_PRE(type) virtual void onVisitPre##type(type *)

#define ADD_VISITOR_POST(type) virtual void onVisitPost##type(type *)

#define ADD_VISITOR(type)\
	virtual void visit##type(type *);\
	ADD_VISITOR_PRE(type) {}\
	ADD_VISITOR_POST(type) {}


namespace ring {
namespace ringc {
namespace ast {


class AstVisitor {
public:
	AstVisitor();

	virtual void visit(AstNode* node);

protected:
	ADD_VISITOR(Module)
	ADD_VISITOR(Use)
	ADD_VISITOR(Extern)
	ADD_VISITOR(Stmt)
	ADD_VISITOR(Let)
	ADD_VISITOR(Expr)
	ADD_VISITOR(ExprEmpty)
	ADD_VISITOR(ExprBlock)
	ADD_VISITOR(ExprFn)
	ADD_VISITOR(ExprIf)
	ADD_VISITOR(ExprIdent)
	ADD_VISITOR(ExprLiteral)
	ADD_VISITOR(ExprMember)
	ADD_VISITOR(ExprCall)
	ADD_VISITOR(ExprUnary)
	ADD_VISITOR(ExprBinary)
	ADD_VISITOR(ExprLogical)
	ADD_VISITOR(ExprConditional)
	ADD_VISITOR(ExprAssignment)

	virtual void skipChildren();
	virtual void terminate();

	// Called when before visit children node.
	// This handler will be called before type specific handler.
	virtual void onVisitPreNode(AstNode* node) {}

	// Called when after visit children node.
	// This handler will be called after type specific handler.
	virtual void onVisitPostNode(AstNode* node) {}

protected:
	bool _skip_children;
	bool _terminate;
};


} // namespace ast
} // namespace ringc
} // namespace ring


#endif
