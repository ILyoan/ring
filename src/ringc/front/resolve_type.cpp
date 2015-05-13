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
	if (let->type_id().none()) {
		let->type_id(let->expr()->type_id());
		Symbol* symbol = session()->symbol_table()->value(let->name().symbol_id());
		symbol->type_id(let->type_id());
	} else {
		ASSERT_EQ(let->type_id(), let->expr()->type_id(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprEmpty(ExprEmpty* empty) {
	if (empty->type_id().none()) {
		empty->type_id(getPrimTypeId(PRIM_TYPE_NIL));
	} else {
		ASSERT_EQ(empty->type_id(), getPrimTypeId(PRIM_TYPE_INT), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprBlock(ExprBlock* block) {
	if (block->type_id().none()) {
		if (block->stmts().empty()) {
			block->type_id(getPrimTypeId(PRIM_TYPE_INT));
		} else {
			Stmt* last = block->stmts().back();
			if (last->isExpr()) {
				Expr* expr = static_cast<Expr*>(last);
				block->type_id(expr->type_id());
			} else {
				ERROR("Last statement of block MUST be expression - block: %s",
					session()->ast_str()->toString(block).c_str());
			}
		}
	}
}


void TypeResolver::onVisitPostExprFn(ExprFn* fn) {
	ASSERT(fn->type_id().some(), SRCPOS);
}


void TypeResolver::onVisitPostExprIf(ExprIf* if_) {
	if (if_->type_id().none()) {
		if (if_->alt()) {
			TypeId con_ty = if_->con()->type_id();
			TypeId alt_ty = if_->alt()->type_id();
			if (con_ty == alt_ty) {
				if_->type_id(con_ty);
			} else {
				ERROR("Type of 'then' and 'else' block of 'if' MUST be the same - if: %s",
						session()->ast_str()->toString(if_).c_str());
				if_->type_id(getPrimTypeId(PRIM_TYPE_INT));
			}
		} else {
			if_->type_id(getPrimTypeId(PRIM_TYPE_INT));
		}
	}
}


void TypeResolver::onVisitPostExprIdent(ExprIdent* ident) {
	Symbol* symbol = session()->symbol_table()->value(ident->id().symbol_id());
	if (ident->type_id().none()) {
		ident->type_id(symbol->type_id());
	} else {
		ASSERT_EQ(ident->type_id(), symbol->type_id(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprLiteral(ExprLiteral* literal) {
	if (literal->type_id().none()) {
		switch (literal->lit_type()) {
			case LIT_FALSE:
			case LIT_TRUE:
				literal->type_id(getPrimTypeId(PRIM_TYPE_BOOL));
				break;
			case LIT_NUM:
				literal->type_id(getPrimTypeId(PRIM_TYPE_INT));
				break;
			default:
				literal->type_id(getPrimTypeId(PRIM_TYPE_NIL));
		}
	}
}


void TypeResolver::onVisitPostExprMember(ExprMember* member) {
}


void TypeResolver::onVisitPostExprCall(ExprCall* call) {
	if (call->type_id().none()) {
		if (call->callee()->type_id().some()) {
			FunctionType* fn_ty = session()->type_table()->getFuncType(
					call->callee()->type_id());
			ASSERT_NE(fn_ty, NULL, SRCPOS);
			call->type_id(fn_ty->ret());
		}
	}
	ASSERT(call->type_id().some(), SRCPOS);
}


void TypeResolver::onVisitPostExprUnary(ExprUnary* unary) {
	if (unary->type_id().none()) {
		unary->type_id(unary->expr()->type_id());
	} else {
		ASSERT_EQ(unary->type_id(), unary->expr()->type_id(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprBinary(ExprBinary* binary) {
	if (binary->type_id().none()) {
		TypeId ty1 = binary->left()->type_id();
		TypeId ty2 = binary->right()->type_id();
		if (ty1 == ty2) {
			binary->type_id(ty1);
		} else {
			ERROR("Type of left and right side of binary expression MUST be the same - binary: %s",
					session()->ast_str()->toString(binary).c_str());
			binary->type_id(getPrimTypeId(PRIM_TYPE_NIL));
		}
	} else {
		ASSERT_EQ(binary->type_id(), binary->left()->type_id(), SRCPOS);
		ASSERT_EQ(binary->type_id(), binary->right()->type_id(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprLogical(ExprLogical* logical) {
	if (logical->type_id().none()) {
		logical->type_id(getPrimTypeId(PRIM_TYPE_BOOL));
	} else {
		ASSERT_EQ(logical->type_id(), getPrimTypeId(PRIM_TYPE_BOOL), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprConditional(ExprConditional* cond) {
	if (cond->type_id().none()) {
		TypeId con_ty = cond->con()->type_id();
		TypeId alt_ty = cond->alt()->type_id();
		if (con_ty == alt_ty) {
			cond->type_id(con_ty);
		} else {
			ERROR("Type of 'con' and 'alt' block of conditional expression MUST be the same - cond: %s",
					session()->ast_str()->toString(cond).c_str());
			cond->type_id(getPrimTypeId(PRIM_TYPE_INT));
		}
	} else {
		ASSERT_EQ(cond->type_id(), cond->con()->type_id(), SRCPOS);
		ASSERT_EQ(cond->type_id(), cond->alt()->type_id(), SRCPOS);
	}
}


void TypeResolver::onVisitPostExprAssignment(ExprAssignment* assign) {
	if (assign->type_id().none()) {
		assign->type_id(assign->left()->type_id());
	} else {
		ASSERT_EQ(assign->type_id(), assign->left()->type_id(), SRCPOS);
	}
}


TypeId TypeResolver::getPrimTypeId(const PrimTypeKey& prim_type_key) {
	return session()->type_table()->getPrimTypeId(prim_type_key);
}