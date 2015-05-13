#include "eval.h"
#include "../ringc/session/diagnostic.h"
#include "../ringc/syntax/ast_stringify.h"
#include <algorithm>
using namespace ring::ringi;
using namespace ring::ringc;
using namespace std;


Evaluator::Evaluator(Interpret* interpret)
		: _interpret(interpret) {
}


Session* Evaluator::session() {
	return _interpret->session();
}


Environment* Evaluator::currEnv() {
	return _interpret->env_curr();
}


void Evaluator::enterBlock(AstNode* node) {
	// Create new environment.
	_interpret->enterBlock(node->node_id());
}


void Evaluator::exitBlock(AstNode* node) {
	// Exit from environment.
	_interpret->exitBlock(node->node_id());
}


// Evaluate an AST.
// The given AST must be pre-resolved and related information should be stacked on the session.
Value*  Evaluator::evaluate(AstNode* node) {
	if (node->isModule()) {
		return evalModule(static_cast<Module*>(node));
	} else if (node->isUse()) {
		return evalUse(static_cast<Use*>(node));
	} else if (node->isStmt()) {
		return evalStmt(static_cast<Stmt*>(node));
	} else {
		FATAL("AST node should be one of module, use, or statmemt. but found %d",
			node->node_type());
	}
}


Value* Evaluator::evalModule(Module* module) {
	Value* res = Value::Nil();
	enterBlock(module);

	for_each(module->uses().begin(), module->uses().end(),
		[this](Use* use) {
			this->evalUse(use);
		});
	for_each(module->decls().begin(), module->decls().end(),
		[this](Let* decl) {
			this->evalLet(decl);
		});

	if (session()->main().some()) {
		LOG(LOG_DEBUG,"invoke main function %d", session()->main());
		vector<Value*> empty_args;
		AstNode* ast_main = session()->ast_table()->value(session()->main());
		if (!ast_main->isLet()) {
			FATAL("`main` is not a let binding: %d", ast_main->node_id());
		}
		Let* let_main = static_cast<Let*>(ast_main);
		if (!let_main->expr()->isExprFn()) {
			FATAL("`main` does not hold a function: %d", ast_main->node_id());
		}
		ExprFn* expr_main = static_cast<ExprFn*>(let_main->expr());
		res = evalFnCall(expr_main, empty_args);
	}

	exitBlock(module);
	return res;
}


// TODO:
Value* Evaluator::evalUse(Use* use) {
	return Value::Nil();
}


Value* Evaluator::evalStmt(Stmt* stmt) {
	if (stmt->isLet()) {
		return evalLet(static_cast<Let*>(stmt));
	} else if (stmt->isExpr()) {
		return evalExpr(static_cast<Expr*>(stmt));
	} else {
		FATAL("statement %d shoud be either let or expression", stmt->node_id());
	}
}


// Evaluate let expression.
Value* Evaluator::evalLet(Let* let) {
	Value* value = evalExpr(let->expr());
	_interpret->initSymbol(let->name().symbol_id(), value);
	return Value::Nil();
}


Value* Evaluator::evalExpr(Expr* expr) {
	switch (expr->node_type()) {
		case AST_EXPR_EMPTY:
			return evalExprEmpty(static_cast<ExprEmpty*>(expr));
		case AST_EXPR_BLOCK:
			return evalExprBlock(static_cast<ExprBlock*>(expr));
		case AST_EXPR_FN:
			return evalExprFn(static_cast<ExprFn*>(expr));
		case AST_EXPR_IF:
			return evalExprIf(static_cast<ExprIf*>(expr));
		case AST_EXPR_IDENT:
			return evalExprIdent(static_cast<ExprIdent*>(expr));
		case AST_EXPR_LITERAL:
			return evalExprLiteral(static_cast<ExprLiteral*>(expr));
		case AST_EXPR_MEMBER:
			return evalExprMember(static_cast<ExprMember*>(expr));
		case AST_EXPR_CALL:
			return evalExprCall(static_cast<ExprCall*>(expr));
		case AST_EXPR_UNARY:
			return evalExprUnary(static_cast<ExprUnary*>(expr));
		case AST_EXPR_BINARY:
			return evalExprBinary(static_cast<ExprBinary*>(expr));
		case AST_EXPR_LOGICAL:
			return evalExprLogical(static_cast<ExprLogical*>(expr));
		case AST_EXPR_CONDITIONAL:
			return evalExprConditional(static_cast<ExprConditional*>(expr));
		case AST_EXPR_ASSIGNMENT:
			return evalExprAssignment(static_cast<ExprAssignment*>(expr));
	}
	FATAL("Unknown expression type %s for: expression %d", expr->node_type(), expr->node_id());
}


// Evaluate empty expression.
Value* Evaluator::evalExprEmpty(ExprEmpty* empty) {
	return Value::Nil();
}


// Evaluate block expression.
Value* Evaluator::evalExprBlock(ExprBlock* block) {
	Value* res = Value::Nil();
	enterBlock(block);

	for_each (block->stmts().begin(), block->stmts().end(), [this, &res] (Stmt* stmt) {
		res = evalStmt(stmt);
	});

	exitBlock(block);
	return res;
}


Value* Evaluator::evalExprFn(ExprFn* fn) {
	return new ValFn(fn->node_id());
}



Value* Evaluator::evalFnCall(NodeId fn_node_id, vector<Value*> args) {
	AstNode* ast_callee = session()->ast_table()->value(fn_node_id);
	if (ast_callee->isExprFn()) {
		ExprFn* fn = static_cast<ExprFn*>(ast_callee);
		return evalFnCall(fn, args);
	}
	FATAL("Callee is not a function %d", ast_callee->node_id());
}



Value* Evaluator::evalFnCall(ExprFn* fn, vector<Value*> args) {
	Value* res = Value::Nil();
	enterBlock(fn->body());

	auto arg_list = fn->args();
	for (int i = 0; i < arg_list.size(); ++i) {
		_interpret->initSymbol(arg_list[i].symbol_id(), args[i]);
	}

	for_each (fn->body()->stmts().begin(), fn->body()->stmts().end(), [this, &res] (Stmt* stmt) {
		res = this->evalStmt(stmt);
	});

	exitBlock(fn->body());
	return res;
}


Value* Evaluator::evalExprIf(ExprIf* if_) {
	Value* test = evalExpr(if_->test());
	ValBool* btest = dynamic_cast<ValBool*>(test);
	if (btest->value()) {
		return evalExpr(if_->con());
	} else if (if_->alt()) {
		return evalExpr(if_->alt());
	}
	return Value::Nil();
}


// Evaluate ident expression.
// Find the definition of this ident in lexical scoping structure.(It was done in resolve phase)
// And get the value of that variable from environment.
Value* Evaluator::evalExprIdent(ExprIdent* ident) {
	Value* value = _interpret->getValueFromEnv(ident->id().symbol_id());
	LOG(LOG_DEBUG, "symbol %s has value %s",
			session()->str_table()->value(ident->id().name_id()).c_str(),
			value->toString().c_str());
	return value;
}


// Evaluate literal expression.
// Create a Value instance depends on the AST,
Value* Evaluator::evalExprLiteral(ExprLiteral* lit) {
	string val = session()->str_table()->value(lit->str_id());

	// Create new value according to literal.
	Value* new_value = Value::NoneVal();
	switch (lit->lit_type()) {
		case LIT_FALSE: new_value = ValBool::False(); break;
		case LIT_TRUE: new_value = ValBool::True(); break;
		case LIT_CHAR: break;
		case LIT_STRING: break;
		case LIT_NUM: new_value = new ValInt(val); break;
	}
	if (new_value == Value::NoneVal()) {
		FATAL("Unknown literal: lit %d, type %d, value %s",
				lit->node_id(), lit->lit_type(), val.c_str());
	}

	LOG(LOG_DEBUG, "literal value %s", new_value->toString().c_str());
	return new_value;
}


// TODO: Evaluate member expression.
Value* Evaluator::evalExprMember(ExprMember* member) {
	FATAL("%s", "unimplemented: evalEmprMember");
}

// Evaluates call expression.
Value* Evaluator::evalExprCall(ExprCall* call) {
	// Evaluate callee.
	Value* callee = evalExpr(call->callee());
	if (callee->value_type() != VALUE_FN) {
		FATAL("Callee expected to be function type(%d), but (%d)", VALUE_FN, callee->value_type());
	}

	// Evaluate arguemnts
	vector<Value*> args;
	for_each (call->args().begin(), call->args().end(), [this, &args] (Expr* expr) {
		args.push_back(this->evalExpr(expr));
	});

	ValFn* val_callee = dynamic_cast<ValFn*>(callee);
	return evalFnCall(val_callee->fn(), args);
}


// TODO: Evaluates unary expression.
Value* Evaluator::evalExprUnary(ExprUnary* unary) {
	FATAL("%s", "unimplemented: evalEmprMember");
}


// Evaluates binaray expression.
Value* Evaluator::evalExprBinary(ExprBinary* binary) {
	// Operands
	Value* operand1 = evalExpr(binary->left());
	Value* operand2 = evalExpr(binary->right());

	// Operates and get the result.
	Value* result =  Value::NoneVal();
	switch (binary->op()) {
		case BO_ADD: result = operand1->add(operand2); break;
		case BO_SUB: result = operand1->sub(operand2); break;
		case BO_MUL: result = operand1->mul(operand2); break;
		case BO_DIV: result = operand1->div(operand2); break;
		case BO_EXP: result = operand1->exp(operand2); break;
		case BO_EQ: result = operand1->eq(operand2); break;
		case BO_NE: result = operand1->ne(operand2); break;
		case BO_LT: result = operand1->lt(operand2); break;
		case BO_LE: result = operand1->le(operand2); break;
		case BO_GE: result = operand1->ge(operand2); break;
		case BO_GT: result = operand1->gt(operand2); break;
	}
	if (result == Value::NoneVal()) {
		FATAL("Unknown operation: %s %s %s",
				operand1->toString().c_str(),
				AstStringify::toString(binary->op()).c_str(),
				operand2->toString().c_str());
	}

	LOG(LOG_DEBUG, "binary operation %s = %s %s %s",
			result->toString().c_str(),
			operand1->toString().c_str(),
			AstStringify::toString(binary->op()).c_str(),
			operand2->toString().c_str());

	// Push the result the stack
	return result;
}


// TODO: Evaluates logical expression.
Value* Evaluator::evalExprLogical(ExprLogical* logical) {
	FATAL("%s", "unimplemented: evalExprLogical");
}


// TODO: Evaluates conditional expression.
Value* Evaluator::evalExprConditional(ExprConditional* conditional) {
	FATAL("%s", "unimplemented: evalExprConditional");
}


// TODO: Evaluates assignment expression.
Value* Evaluator::evalExprAssignment(ExprAssignment* assignment) {
	FATAL("%s", "unimplemented: evalExprAssignment");
}
