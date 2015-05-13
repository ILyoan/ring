#include "ast_visitor.h"
#include <algorithm>
#include <cassert>
using namespace ring::ringc::ast;
using namespace std;


AstVisitor::AstVisitor()
		: _skip_children(false)
		, _terminate(false) {
}


void AstVisitor::skipChildren() {
	_skip_children = true;
}


void AstVisitor::terminate() {
	_terminate = true;
}


void AstVisitor::visit(AstNode* node) {
	if (node->isModule()) {
		visitModule(static_cast<Module*>(node));
	} else if (node->isUse()) {
		visitUse(static_cast<Use*>(node));
	} else if (node->isStmt()) {
		visitStmt(static_cast<Stmt*>(node));
	} else {
		assert(!"AST node should be one of module, use, or statmemt.");
	}
}


void AstVisitor::visitModule(Module* module) {
	VISIT_ON_PRE(Module, module)

	for_each(module->uses().begin(), module->uses().end(),
		[this](Use* use) {
			VISIT_CHILD(Use, use)
		});
	for_each(module->exts().begin(), module->exts().end(),
		[this](Extern* ext) {
			VISIT_CHILD(Extern, ext)
		});
	for_each(module->decls().begin(), module->decls().end(),
		[this](Let* decl) {
			VISIT_CHILD(Let, decl)
		});

	VISIT_ON_POST(Module, module)
}


void AstVisitor::visitUse(Use* use) {
	VISIT_ON_PRE(Use, use)
	VISIT_ON_POST(Use, use)
}


void AstVisitor::visitExtern(Extern* ext) {
	VISIT_ON_PRE(Extern, ext);
	VISIT_ON_POST(Extern, ext);
}


void AstVisitor::visitStmt(Stmt* stmt) {
	if (stmt->isLet()) {
		visitLet(static_cast<Let*>(stmt));
	} else if (stmt->isExpr()) {
		visitExpr(static_cast<Expr*>(stmt));
	} else {
		assert(!"Statement node should be one of let binding or expression.");
	}
}


void AstVisitor::visitLet(Let* let) {
	VISIT_ON_PRE(Let, let)
	VISIT_CHILD(Expr, let->expr())
	VISIT_ON_POST(Let, let)
}


void AstVisitor::visitExpr(Expr* expr) {
	switch (expr->node_type()) {
		case AST_EXPR_EMPTY:
			visitExprEmpty(static_cast<ExprEmpty*>(expr));
			return;
		case AST_EXPR_BLOCK:
			visitExprBlock(static_cast<ExprBlock*>(expr));
			return;
		case AST_EXPR_FN:
			visitExprFn(static_cast<ExprFn*>(expr));
			return;
		case AST_EXPR_IF:
			visitExprIf(static_cast<ExprIf*>(expr));
			return;
		case AST_EXPR_IDENT:
			visitExprIdent(static_cast<ExprIdent*>(expr));
			return;
		case AST_EXPR_LITERAL:
			visitExprLiteral(static_cast<ExprLiteral*>(expr));
			return;
		case AST_EXPR_MEMBER:
			visitExprMember(static_cast<ExprMember*>(expr));
			return;
		case AST_EXPR_CALL:
			visitExprCall(static_cast<ExprCall*>(expr));
			return;
		case AST_EXPR_UNARY:
			visitExprUnary(static_cast<ExprUnary*>(expr));
			return;
		case AST_EXPR_BINARY:
			visitExprBinary(static_cast<ExprBinary*>(expr));
			return;
		case AST_EXPR_LOGICAL:
			visitExprLogical(static_cast<ExprLogical*>(expr));
			return;
		case AST_EXPR_CONDITIONAL:
			visitExprConditional(static_cast<ExprConditional*>(expr));
			return;
		case AST_EXPR_ASSIGNMENT:
			visitExprAssignment(static_cast<ExprAssignment*>(expr));
			return;
	}
	assert(!"Unknown expression");
}



void AstVisitor::visitExprEmpty(ExprEmpty* empty) {
	VISIT_ON_PRE(ExprEmpty, empty)
	VISIT_ON_POST(ExprEmpty, empty)
}


void AstVisitor::visitExprBlock(ExprBlock* block) {
	VISIT_ON_PRE(ExprBlock, block)
	for_each (block->stmts().begin(), block->stmts().end(), [this] (Stmt* stmt) {
		VISIT_CHILD(Stmt, stmt)
	});
	VISIT_ON_POST(ExprBlock, block)
}


void AstVisitor::visitExprFn(ExprFn* fn) {
	VISIT_ON_PRE(ExprFn, fn)
	VISIT_CHILD(ExprBlock, fn->body())
	VISIT_ON_POST(ExprFn, fn)
}


void AstVisitor::visitExprIf(ExprIf* if_) {
	VISIT_ON_PRE(ExprIf, if_)
	VISIT_CHILD(Expr, if_->test())
	VISIT_CHILD(Expr, if_->con())
	if (if_->alt()) {
		VISIT_CHILD(Expr, if_->alt())
	}
	VISIT_ON_POST(ExprIf, if_)
}


void AstVisitor::visitExprIdent(ExprIdent* ident) {
	VISIT_ON_PRE(ExprIdent, ident)
	VISIT_ON_POST(ExprIdent, ident)
}


void AstVisitor::visitExprLiteral(ExprLiteral* lit) {
	VISIT_ON_PRE(ExprLiteral, lit)
	VISIT_ON_POST(ExprLiteral, lit)
}


void AstVisitor::visitExprMember(ExprMember* member) {
	VISIT_ON_PRE(ExprMember, member)
	VISIT_CHILD(Expr, member->object())
	if (member->property().isIndex()) {
		VISIT_CHILD(Expr, member->property().index())
	}
	VISIT_ON_POST(ExprMember, member)
}


void AstVisitor::visitExprCall(ExprCall* call) {
	VISIT_ON_PRE(ExprCall, call)
	VISIT_CHILD(Expr, call->callee())
	for_each (call->args().begin(), call->args().end(), [this] (Expr* arg) {
		VISIT_CHILD(Expr, arg)
	});
	VISIT_ON_POST(ExprCall, call)
}


void AstVisitor::visitExprUnary(ExprUnary* unary) {
	VISIT_ON_PRE(ExprUnary, unary)
	VISIT_CHILD(Expr, unary->expr())
	VISIT_ON_POST(ExprUnary, unary)
}


void AstVisitor::visitExprBinary(ExprBinary* binary) {
	VISIT_ON_PRE(ExprBinary, binary)
	VISIT_CHILD(Expr, binary->left())
	VISIT_CHILD(Expr, binary->right())
	VISIT_ON_POST(ExprBinary, binary)
}


void AstVisitor::visitExprLogical(ExprLogical* logical) {
	VISIT_ON_PRE(ExprLogical, logical)
	VISIT_CHILD(Expr, logical->left())
	VISIT_CHILD(Expr, logical->right())
	VISIT_ON_POST(ExprLogical, logical)
}


void AstVisitor::visitExprConditional(ExprConditional* condition) {
	VISIT_ON_PRE(ExprConditional, condition)
	VISIT_CHILD(Expr, condition->test())
	VISIT_CHILD(Expr, condition->con())
	VISIT_CHILD(Expr, condition->alt())
	VISIT_ON_POST(ExprConditional, condition)
}


void AstVisitor::visitExprAssignment(ExprAssignment* assign) {
	VISIT_ON_PRE(ExprAssignment, assign)
	VISIT_CHILD(Expr, assign->left())
	VISIT_CHILD(Expr, assign->right())
	VISIT_ON_POST(ExprAssignment, assign)
}
