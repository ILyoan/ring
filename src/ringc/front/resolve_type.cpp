#include "resolve_type.h"
#include <algorithm>
using namespace ring::ringc;
using namespace std;


TypeResolver::TypeResolver(Resolver* resolver)
		: _resolver(resolver)
		, _session(resolver->session()) {
}


void TypeResolver::resolve(AstNode* node) {
	visit(node);
}


void TypeResolver::onVisitPostLet(Let* let) {
	if (let->ty().none()) {
		let->ty(let->expr()->ty());
		Symbol* symbol = session()->symbol_table()->value(let->name().symbol_id());
		symbol->ty(let->ty());
	} else {
		ASSERT_EQ(let->ty(), let->expr()->ty(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprEmpty(ExprEmpty* empty) {
	if (empty->ty().none()) {
		empty->ty(getPrimTypeId(PRIM_TYPE_NIL));
	} else {
		ASSERT_EQ(empty->ty(), getPrimTypeId(PRIM_TYPE_INT), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprBlock(ExprBlock* block) {
	if (block->ty().none()) {
		if (block->stmts().empty()) {
			block->ty(getPrimTypeId(PRIM_TYPE_INT));
		} else {
			Stmt* last = block->stmts().back();
			if (last->isExpr()) {
				Expr* expr = static_cast<Expr*>(last);
				block->ty(expr->ty());
			} else {
				ERROR("Last statement of block MUST be expression - block: %s",
					session()->ast_str()->toString(block).c_str());
			}
		}
	}
}


void TypeResolver::onVisitPostExprFn(ExprFn* fn) {
	ASSERT(fn->ty().some(), SRCPOS);
}


void TypeResolver::onVisitPostExprIf(ExprIf* if_) {
	if (if_->ty().none()) {
		if (if_->alt()) {
			TypeId con_ty = if_->con()->ty();
			TypeId alt_ty = if_->alt()->ty();
			if (con_ty == alt_ty) {
				if_->ty(con_ty);
			} else {
				ERROR("Type of 'then' and 'else' block of 'if' MUST be the same - if: %s",
						session()->ast_str()->toString(if_).c_str());
				if_->ty(getPrimTypeId(PRIM_TYPE_INT));
			}
		} else {
			if_->ty(getPrimTypeId(PRIM_TYPE_INT));
		}
	}
}


void TypeResolver::onVisitPostExprIdent(ExprIdent* ident) {
	Symbol* symbol = session()->symbol_table()->value(ident->id().symbol_id());
	if (ident->ty().none()) {
		ident->ty(symbol->ty());
	} else {
		ASSERT_EQ(ident->ty(), symbol->ty(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprLiteral(ExprLiteral* lit) {
	if (lit->ty().none()) {
		if (lit->lit_type() == LIT_BASIC) {
			FATAL("The type of literal SHOULD be set befer type resovle - literal: %s",
					session()->ast_str()->toString(lit).c_str());
		} else {
		}
	}
}


void TypeResolver::onVisitPostExprMember(ExprMember* member) {
}


void TypeResolver::onVisitPostExprCall(ExprCall* call) {
	if (call->ty().none()) {
		if (call->callee()->ty().some()) {
			TypeFunc* fn_type = session()->type_table()->getFuncType(
					call->callee()->ty());
			ASSERT_NE(fn_type, NULL, SRCPOS);
			call->ty(fn_type->ty_ret());
		}
	}
	ASSERT(call->ty().some(), SRCPOS);
}


void TypeResolver::onVisitPostExprUnary(ExprUnary* unary) {
	if (unary->ty().none()) {
		unary->ty(unary->expr()->ty());
	} else {
		ASSERT_EQ(unary->ty(), unary->expr()->ty(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprBinary(ExprBinary* binary) {
	if (binary->ty().none()) {
		TypeId ty1 = binary->left()->ty();
		TypeId ty2 = binary->right()->ty();
		if (ty1 == ty2) {
			binary->ty(ty1);
		} else {
			ERROR("Type of left and right side of binary expression MUST be the same - binary: %s",
					session()->ast_str()->toString(binary).c_str());
			binary->ty(getPrimTypeId(PRIM_TYPE_NIL));
		}
	} else {
		ASSERT_EQ(binary->ty(), binary->left()->ty(), SRCPOS);
		ASSERT_EQ(binary->ty(), binary->right()->ty(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprLogical(ExprLogical* logical) {
	if (logical->ty().none()) {
		logical->ty(getPrimTypeId(PRIM_TYPE_BOOL));
	} else {
		ASSERT_EQ(logical->ty(), getPrimTypeId(PRIM_TYPE_BOOL), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprConditional(ExprConditional* cond) {
	if (cond->ty().none()) {
		TypeId con_ty = cond->con()->ty();
		TypeId alt_ty = cond->alt()->ty();
		if (con_ty == alt_ty) {
			cond->ty(con_ty);
		} else {
			ERROR("Type of 'con' and 'alt' block of conditional expression MUST be the same - cond: %s",
					session()->ast_str()->toString(cond).c_str());
			cond->ty(getPrimTypeId(PRIM_TYPE_INT));
		}
	} else {
		ASSERT_EQ(cond->ty(), cond->con()->ty(), SRCPOS);
		ASSERT_EQ(cond->ty(), cond->alt()->ty(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprAssignment(ExprAssignment* assign) {
	if (assign->ty().none()) {
		assign->ty(assign->left()->ty());
	} else {
		ASSERT_EQ(assign->ty(), assign->left()->ty(), SRCPOS);
	}
}


TypeId TypeResolver::getPrimTypeId(const PrimTypeKey& prim_type_key) {
	return session()->type_table()->getPrimTypeId(prim_type_key);
}