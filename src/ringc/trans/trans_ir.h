#ifndef RING_RINGC_TRANS_TRANS_IR_H
#define RING_RINGC_TRANS_TRANS_IR_H


#include "../session/session.h"
#include "../syntax/ast.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

namespace ring {
namespace ringc {


enum ExprValType {
	EXPR_VAL_TYPE_L,
	EXPR_VAL_TYPE_R,
};


class TransIR {
	ADD_PROPERTY_P(session, Session)

public:
	TransIR(Session* session);

	llvm::Module* trans(ast::Module* module);

protected:
	llvm::Module* transModule(ast::Module* module);
	llvm::Value* transUse(ast::Use* use);
	llvm::Value* transExtern(ast::Extern* ext);
	llvm::Value* transStmt(ast::Stmt* stmt);
	llvm::Value* transLet(ast::Let* let);
	llvm::Value* transExpr(ast::Expr* expr);
	llvm::Value* transExprAs(ast::Expr* expr, ExprValType val_type);
	llvm::Value* transExprAsLval(ast::Expr* expr);
	llvm::Value* transExprAsRval(ast::Expr* expr);
	llvm::Value* transExprEmpty(ast::ExprEmpty* empty);
	llvm::Value* transExprBlock(ast::ExprBlock* block);
	llvm::Function* transExprFn(ast::ExprFn* fn);
	llvm::Value* transExprIf(ast::ExprIf* if_);
	llvm::Value* transExprIdent(ast::ExprIdent* ident);
	llvm::Value* transExprLiteral(ast::ExprLiteral* lit);
	llvm::Value* transExprMember(ast::ExprMember* member);
	llvm::Value* transExprCall(ast::ExprCall* call);
	llvm::Value* transExprUnary(ast::ExprUnary* unary);
	llvm::Value* transExprBinary(ast::ExprBinary* binary);
	llvm::Value* transExprLogical(ast::ExprLogical* logical);
	llvm::Value* transExprConditional(ast::ExprConditional* conditional);
	llvm::Value* transExprAssignment(ast::ExprAssignment* assignment);

	llvm::Type* transType(TypeId ty);
	llvm::Type* transType(ast::Type* type);
	llvm::Type* transPrimType(ast::TypePrim* prim_type);
	llvm::Type* transArrayType(ast::TypeArray* arr_type);
	llvm::FunctionType* transFuncType(ast::TypeFunc* fn_type);

	llvm::Function* transFuncProto(
			ast::TypeFunc* fn_type,
			vector<Ident>& args,
			NameId name_id = NameId(),
			bool is_external = true);
	llvm::Value* transStmts(vector<Stmt*>& stmts);

	llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* ll_fn, SymbolId symbol_id);
	void CreateArgumentAllocas(ast::ExprFn* fn, llvm::Function* ll_fn);

	llvm::Constant* getConstInt(int v, int bits = 32, bool is_signed = false);
	llvm::Constant* getConstInt(StrId str_id, int bits = 32, uint8_t radix = 10);

	llvm::LLVMContext& llvmContext();
	llvm::Function* llvmCurrFn();

	ExprValType exprValType(Expr* expr);

protected:
	llvm::Module* _module;
	llvm::IRBuilder<> _builder;
};


} // namespace ringc
} // namespace ring


#endif
