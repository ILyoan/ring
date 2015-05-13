#ifndef RING_RINGI_EVAL_H
#define RING_RINGI_EVAL_H


namespace ring {
namespace ringi {
	class Evaluator;
}}


#include "../common.h"
#include "../ringc/syntax/ast.h"
#include "../ringc/syntax/ast_visitor.h"
#include "../ringc/session/session.h"
#include "values.h"
#include "interpret.h"
#include "environment.h"
using namespace ring::ringc::ast;
using namespace ring::ringc;
using namespace ring::ringi;


namespace ring {
namespace ringi {


// Evaluator is for evaluate a AST according to its sementics.
class Evaluator {
public:
	Evaluator(Interpret* interpret);

	// Evaluate an AST.
	// The given AST must be pre-resolved and related information should be stacked on the session.
	Value* evaluate(AstNode* node);

protected:
	Session* session();
	Environment* currEnv();

	void enterBlock(AstNode* node);
	void exitBlock(AstNode* node);

	Value* evalFnCall(NodeId fn_node_id, vector<Value*> args);
	Value* evalFnCall(ExprFn* fn, vector<Value*> args);

	Value* evalModule(Module* module);
	Value* evalUse(Use* use);
	Value* evalStmt(Stmt* stmt);
	Value* evalLet(Let* let);
	Value* evalExpr(Expr* expr);
	Value* evalExprEmpty(ExprEmpty* empty);
	Value* evalExprBlock(ExprBlock* block);
	Value* evalExprFn(ExprFn* fn);
	Value* evalExprIf(ExprIf* if_);
	Value* evalExprIdent(ExprIdent* ident);
	Value* evalExprLiteral(ExprLiteral* lit);
	Value* evalExprMember(ExprMember* member);
	Value* evalExprCall(ExprCall* call);
	Value* evalExprUnary(ExprUnary* unary);
	Value* evalExprBinary(ExprBinary* binary);
	Value* evalExprLogical(ExprLogical* logical);
	Value* evalExprConditional(ExprConditional* conditional);
	Value* evalExprAssignment(ExprAssignment* assignment);

protected:
	Interpret* _interpret;
};


} // namespace ringi
} // namespace ring


#endif
