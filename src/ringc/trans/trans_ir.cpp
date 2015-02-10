#include "trans_ir.h"
using namespace ring::ringc;

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"


TransIR::TransIR(Session* session)
		: _session(session)
		, _module(NULL)
		, _builder(llvmContext()) {
}


// Translate program.
llvm::Module* TransIR::trans(ast::Module* module) {
	return transModule(module);
}


// Translate module.
llvm::Module* TransIR::transModule(ast::Module* module) {
	// Create llvm module.
	_module = new llvm::Module("top", llvmContext());

	for_each(module->uses().begin(), module->uses().end(),
		[this](ast::Use* use) {
			this->transUse(use);
		});
	for_each(module->exts().begin(), module->exts().end(),
		[this](ast::Extern* ext) {
			this->transExtern(ext);
		});
	for_each(module->decls().begin(), module->decls().end(),
		[this](ast::Let* decl) {
			this->transLet(decl);
		});

	return _module;
}


// Translate use.
llvm::Value* TransIR::transUse(ast::Use* use) {
	FATAL("Not implemented: %s", "transUse");
}


// Translate extern.
llvm::Value* TransIR::transExtern(ast::Extern* ext) {
	LOG(LOG_DEBUG, "Trans extern: %s", session()->str(ext->name().name_id()).c_str());
	// Symbol for this let bidning.
	Symbol* symbol = session()->symbol_table()->value(ext->name().symbol_id());
	ASSERT_EQ(symbol->llvm_val(), NULL, SRCPOS);
	// According to its type...
	if (session()->type_table()->isFuncType(ext->type_id())) {
		llvm::FunctionType* ll_fn_type = transFunctionType(
				session()->type_table()->getFuncType(ext->type_id()));
		llvm::Function* ll_fn = llvm::Function::Create(
				ll_fn_type,
				llvm::Function::ExternalLinkage,
				session()->str(symbol->name_id()),
				_module);
		symbol->llvm_val(ll_fn);
		return ll_fn;
	} else {
		FATAL("Not implemented: transExtern");
	}
}


// Translate statmement.
llvm::Value* TransIR::transStmt(ast::Stmt* stmt) {
	if (stmt->isLet()) {
		return transLet(static_cast<ast::Let*>(stmt));
	} else {
		return transExprAsRval(static_cast<ast::Expr*>(stmt));
	}
}


// Translate let binding.
llvm::Value* TransIR::transLet(ast::Let* let) {
	// Symbol for this let bidning.
	Symbol* symbol = session()->symbol_table()->value(let->name().symbol_id());
	ASSERT_EQ(symbol->llvm_val(), NULL, SRCPOS);
	// According to its type...
	if (session()->type_table()->isFuncType(let->type_id())) {
		llvm::Function* ll_fn = transExprFn(static_cast<ast::ExprFn*>(let->expr()));
		ASSERT_EQ(symbol->llvm_val(), ll_fn, SRCPOS);
	} else {
		if (let->inModuleScope()) {
			FATAL("Not implemented: %s", "top level expression");
		} else {
			// Create allocation instruction.
			llvm::AllocaInst* alloca = createEntryBlockAlloca(
					llvmCurrFn(), let->name().symbol_id());
			// Set instruction to symbol.
			ASSERT_EQ(symbol->llvm_val(), alloca, SRCPOS);
			// Initializer.
			llvm::Value* val = transExprAsRval(let->expr());
			// Create store instruction.
			_builder.CreateStore(val, alloca);
		}
	}
	return NULL;
}


// Translate expression.
llvm::Value* TransIR::transExpr(ast::Expr* expr) {
	switch (expr->node_type()) {
		case AST_EXPR_EMPTY:
			return transExprEmpty(static_cast<ExprEmpty*>(expr));
		case AST_EXPR_BLOCK:
			return transExprBlock(static_cast<ExprBlock*>(expr));
		case AST_EXPR_FN:
			return transExprFn(static_cast<ExprFn*>(expr));
		case AST_EXPR_IF:
			return transExprIf(static_cast<ExprIf*>(expr));
		case AST_EXPR_IDENT:
			return transExprIdent(static_cast<ExprIdent*>(expr));
		case AST_EXPR_LITERAL:
			return transExprLiteral(static_cast<ExprLiteral*>(expr));
		case AST_EXPR_MEMBER:
			return transExprMember(static_cast<ExprMember*>(expr));
		case AST_EXPR_CALL:
			return transExprCall(static_cast<ExprCall*>(expr));
		case AST_EXPR_UNARY:
			return transExprUnary(static_cast<ExprUnary*>(expr));
		case AST_EXPR_BINARY:
			return transExprBinary(static_cast<ExprBinary*>(expr));
		case AST_EXPR_LOGICAL:
			return transExprLogical(static_cast<ExprLogical*>(expr));
		case AST_EXPR_CONDITIONAL:
			return transExprConditional(static_cast<ExprConditional*>(expr));
		case AST_EXPR_ASSIGNMENT:
			return transExprAssignment(static_cast<ExprAssignment*>(expr));
	}
	FATAL("Unknown expression type %s for: expr %d", expr->node_type(), expr->node_id());
}


llvm::Value* TransIR::transExprAs(ast::Expr* expr, ExprValType val_type) {
	switch (val_type) {
		case EXPR_VAL_TYPE_L: return transExprAsLval(expr);
		case EXPR_VAL_TYPE_R: return transExprAsRval(expr);
	}
	FATAL("Unknown expression value type: %d", val_type);
}


llvm::Value* TransIR::transExprAsLval(ast::Expr* expr) {
	llvm::Value* val = transExpr(expr);
	if (exprValType(expr) == EXPR_VAL_TYPE_L) {
		return val;
	} else {
		FATAL("Expected l-value but found R-value: node %d", expr->node_id());
	}
}


llvm::Value* TransIR::transExprAsRval(ast::Expr* expr) {
	llvm::Value* val = transExpr(expr);
	if (exprValType(expr) == EXPR_VAL_TYPE_R) {
		return val;
	} else {
		if (llvm::isa<llvm::AllocaInst>(val)) {
			// If the value is an allocation, load the real value.
			return _builder.CreateLoad(val);
		} else {
			FATAL("Expected alloca but found not: node %d", expr->node_id());
		}
	}
}


llvm::Value* TransIR::transExprEmpty(ast::ExprEmpty* empty) {
	return NULL;
}


llvm::Value* TransIR::transExprBlock(ast::ExprBlock* block) {
	return transStmts(block->stmts());
}


// Translate function expression.
llvm::Function* TransIR::transExprFn(ast::ExprFn* fn) {
	NameId name_id;
	if (fn->parent()->isLet()) {
		name_id = static_cast<ast::Let*>(fn->parent())->name().name_id();
	}
	// Translate function prototype
	ast::FunctionType* ast_fn_type = session()->type_table()->getFuncType(fn->type_id());
	llvm::FunctionType* ll_fn_type = transFunctionType(ast_fn_type);
	llvm::Function* ll_fn = transFunctionProto(ast_fn_type, fn->args(), name_id, true);
	// If this function is in a let binding, set symbol for it llvm value.
	if (fn->parent()->isLet()) {
		ast::Let* let = static_cast<ast::Let*>(fn->parent());
		Symbol* symbol = session()->symbol_table()->value(let->name().symbol_id());
		symbol->llvm_val(ll_fn);
	}
	// Create Entry block of the function.
	llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvmContext(), "entry", ll_fn);
	_builder.SetInsertPoint(bb);
	// Argument bind - create alloca insr for arguments.
	CreateArgumentAllocas(fn, ll_fn);
	// Translate body of the function.
	llvm::Value* ll_ret = transStmts(fn->body()->stmts());
	// Return instruction.
	if (session()->type_table()->getType(ast_fn_type->ret())->isNil()) {
		_builder.CreateRetVoid();
	} else {
		_builder.CreateRet(ll_ret);
	}
	return ll_fn;
}


// Translate if expression.
llvm::Value* TransIR::transExprIf(ast::ExprIf* if_) {
	llvm::Value* res = NULL;

	// Create instruction for test.
	llvm::Value* vtest = transExprAsRval(if_->test());

	// Create basic blocks.
	llvm::Function* ll_fn = llvmCurrFn();
	llvm::BasicBlock* bb_con = llvm::BasicBlock::Create(llvmContext(), "con", ll_fn);
	llvm::BasicBlock* bb_alt = llvm::BasicBlock::Create(llvmContext(), "alt");
	llvm::BasicBlock* bb_merge = llvm::BasicBlock::Create(llvmContext(), "merge");

	// Craete condition branch.
	if (if_->alt()) {
		_builder.CreateCondBr(vtest, bb_con, bb_alt);
	} else {
		_builder.CreateCondBr(vtest, bb_con, bb_merge);
	}

	// Translate 'then' block.
	_builder.SetInsertPoint(bb_con);
	llvm::Value* vcon = transExprAsRval(if_->con());
	_builder.CreateBr(bb_merge);
	bb_con = _builder.GetInsertBlock();
	res = vcon;

	// Translate 'else' block.
	if (if_->alt()) {
		ll_fn->getBasicBlockList().push_back(bb_alt);
		_builder.SetInsertPoint(bb_alt);
		llvm::Value* valt = transExprAsRval(if_->alt());
		_builder.CreateBr(bb_merge);
		bb_alt = _builder.GetInsertBlock();

		ASSERT_EQ(vcon->getType(), valt->getType(),
				"'then' block and 'else' block should evaluate the same type");

		// Emit 'merge' block.
		ll_fn->getBasicBlockList().push_back(bb_merge);
		_builder.SetInsertPoint(bb_merge);
		llvm::PHINode *phi = _builder.CreatePHI(vcon->getType(), 2, "ifphi");
		phi->addIncoming(vcon, bb_con);
		phi->addIncoming(valt, bb_alt);
		res = phi;
	}

	return res;
}


// Translate ident expression.
// Identifier expression evaluates l-value.
llvm::Value* TransIR::transExprIdent(ast::ExprIdent* ident) {
	// Find symbol.
	Symbol* symbol = session()->symbol_table()->value(ident->id().symbol_id());
	// Debug check invariants.
	// Symbol should already have been bound with a llvm value.
	ASSERT_EQ(symbol->name_id(), ident->id().name_id(), SRCPOS);
	ASSERT_NE(symbol->llvm_val(), NULL, SRCPOS);
	// Take coressponding llvm value.
	llvm::Value* val = symbol->llvm_val();
	return val;
}


// Translate literal expression.
llvm::Value* TransIR::transExprLiteral(ast::ExprLiteral* lit) {
	Type* ty = session()->type_table()->getType(lit->type_id());
	if (lit->isBasic()) {
		ExprLiteralBasic* basic_lit = static_cast<ExprLiteralBasic*>(lit);
		if (ty->isInt()) {
			return getConstantInt(basic_lit->str_id());
		}
	} else if (lit->isArray()) {

	}
	return NULL;
}


// Translate member exression.
llvm::Value* TransIR::transExprMember(ast::ExprMember* member) {
	FATAL("Not implemented: %s", "transExprMember");
}


// Translate call expression.
llvm::Value* TransIR::transExprCall(ast::ExprCall* call) {
	// Translate callee.
	llvm::Function* ll_callee = llvm::dyn_cast<llvm::Function>(transExpr(call->callee()));
	ASSERT_EQ(ll_callee->arg_size(), call->args().size(),
			"Argument size mismatched: ast_id %d", call->node_id());
	// Function arguments.
	vector<llvm::Value*> args;
	for_each (call->args().begin(), call->args().end(), [this, &args] (Expr* expr_arg) {
		llvm::Value* ll_arg = transExprAsRval(expr_arg);
		args.push_back(ll_arg);
	});
	// Create call instruction.
	return _builder.CreateCall(ll_callee, args);
}


// Translate unary expression.
llvm::Value* TransIR::transExprUnary(ast::ExprUnary* unary) {
	FATAL("Not implemented: %s", "transExprUnary");
}


// Translate binary expression.
llvm::Value * TransIR::transExprBinary(ast::ExprBinary* bi) {
	// Translate operands.
	llvm::Value* op1 = transExprAsRval(bi->left());
	llvm::Value* op2 = transExprAsRval(bi->right());
	ASSERT_NE(op1, NULL, SRCPOS);
	ASSERT_NE(op2, NULL, SRCPOS);

	// Create instruction according to operator.
	switch (bi->op()) {
		case BO_ADD: return _builder.CreateAdd(op1, op2, "addtmp");
		case BO_SUB: return _builder.CreateSub(op1, op2, "subtmp");
		case BO_MUL: return _builder.CreateMul(op1, op2, "multmp");
		case BO_DIV: return _builder.CreateSDiv(op1, op2, "sdivtmp");
		case BO_EXP: return NULL;
		case BO_EQ: return _builder.CreateICmpEQ(op1, op2, "icmpeqtmp");
		case BO_NE: return _builder.CreateICmpNE(op1, op2, "icmpnetmp");
		case BO_LT: return _builder.CreateICmpSLT(op1, op2, "icmpslttmp");
		case BO_LE: return _builder.CreateICmpSLE(op1, op2, "icmpsletmp");
		case BO_GE: return _builder.CreateICmpSGE(op1, op2, "icmpsgetmp");
		case BO_GT: return _builder.CreateICmpSGT(op1, op2, "icmpsgttmp");
	}
	FATAL("Unknown binary operator %d for: expr %d", bi->op(), bi->node_id());
}


// Translate logical expression.
llvm::Value* TransIR::transExprLogical(ast::ExprLogical* logical) {
	FATAL("Not implemented: %s", "transExprLogical");
}


// Translate conditional expression.
llvm::Value* TransIR::transExprConditional(ast::ExprConditional* cond) {
	FATAL("Not implemented: %s", "transExprConditional");
}


// Translate assignment expression.
llvm::Value* TransIR::transExprAssignment(ast::ExprAssignment* assign) {
	llvm::Value* lv = transExprAsLval(assign->left());
	llvm::Value* rv = transExprAsRval(assign->right());
	_builder.CreateStore(rv, lv);
	return rv;
}


// Translate type.
llvm::Type* TransIR::transType(TypeId type_id) {
	Type* type = session()->type_table()->getType(type_id);
	if (type) {
		return transType(type);
	} else {
		FATAL("Unknown type_id: %d", type_id);
	}
}


llvm::Type* TransIR::transType(ast::Type* type) {
	ASSERT_NE(type, NULL, SRCPOS);
	if (type->isPrimitiveType()) {
		return transPrimitiveType(static_cast<ast::PrimitiveType*>(type));
	} else {
		return transFunctionType(static_cast<ast::FunctionType*>(type));
	}
}


// Translate primitive type.
llvm::Type* TransIR::transPrimitiveType(ast::PrimitiveType* prim_type) {
	switch (prim_type->primitive_type()) {
		case ast::PRIM_TYPE_NIL: return _builder.getVoidTy();
		case ast::PRIM_TYPE_INT: return _builder.getInt32Ty();
		case ast::PRIM_TYPE_BOOL: return _builder.getInt32Ty();
	}
	FATAL("Unknown primitive type: %d", prim_type->primitive_type());
}


// Translate function type.
llvm::FunctionType* TransIR::transFunctionType(ast::FunctionType* fn_type) {
	vector<llvm::Type*> params;
	for_each(fn_type->args().begin(), fn_type->args().end(), [this, &params] (TypeId arg_type_id){
		params.push_back(transType(session()->type_table()->getType(arg_type_id)));
	});
	return llvm::FunctionType::get(
			transType(session()->type_table()->getType(fn_type->ret())),
			params,
			false);
}


// Translate function prototype.
llvm::Function* TransIR::transFunctionProto(
		ast::FunctionType* fn_type,
		vector<Ident>& args,
		NameId name_id,
		bool is_external) {
	// Create function.
	llvm::FunctionType* ll_fn_type = transFunctionType(fn_type);
	llvm::Function* ll_fn = llvm::Function::Create(
			ll_fn_type,
			llvm::Function::ExternalLinkage,
			session()->str(name_id),
			_module);
	// Set argument name
	int idx = 0;
	for (auto it = ll_fn->arg_begin();it != ll_fn->arg_end(); ++idx, ++it) {
		it->setName(session()->str(args[idx].name_id()));
	}
	// Return the function;
	return ll_fn;
}


// Translate statments
llvm::Value* TransIR::transStmts(vector<Stmt*>& stmts) {
	llvm::Value* res = NULL;
	// Translate each statement.
	for_each(stmts.begin(), stmts.end(), [this, &res] (Stmt* stmt) {
		res = transStmt(stmt);
	});
	// The last one is the value of the statements.
	return res;
}


// Create an alloca instr on the function entry.
llvm::AllocaInst* TransIR::createEntryBlockAlloca(llvm::Function* ll_fn, SymbolId symbol_id) {
	// Take symbol from symbol table.
	Symbol* symbol = session()->symbol_table()->value(symbol_id);
	ASSERT_EQ(symbol->llvm_val(), NULL, SRCPOS);
	// Create alloca.
	llvm::BasicBlock* bb_entry = &ll_fn->getEntryBlock();
	llvm::IRBuilder<> builder(bb_entry, bb_entry->begin());
	llvm::AllocaInst* alloca = builder.CreateAlloca(
			transType(symbol->type_id()), 0, session()->str(symbol->name_id()));
	// Set symbol alloca.
	symbol->llvm_val(alloca);
	return alloca;
}


// Create alloca instrs for function arguments.
void TransIR::CreateArgumentAllocas(ast::ExprFn* fn, llvm::Function* ll_fn) {
	ASSERT_EQ(ll_fn->arg_size(), fn->args().size(),
			"Argument size mismatched: ast_id %d", fn->node_id());
	auto it = ll_fn->arg_begin();
	int idx = 0;
	// For each function argument...
	while (it != ll_fn->arg_end()) {
		// Create alloca instr.
		llvm::AllocaInst* alloca = createEntryBlockAlloca(ll_fn, fn->args()[idx].symbol_id());
		_builder.CreateStore(it, alloca);
		++it;
		++idx;
	}
}


// Create constant int value.
llvm::Constant* TransIR::getConstantInt(int v, int bits, bool is_signed) {
	LOG(LOG_DEBUG, "Trans constant int: value %d", v);
	return llvm::ConstantInt::get(llvm::IntegerType::get(llvmContext(), bits),	v, is_signed);
}


// Create constant int value.
llvm::Constant* TransIR::getConstantInt(StrId str_id, int bits, uint8_t radix) {
	LOG(LOG_DEBUG, "Trans constant int: value %s, str_id %d", session()->str(str_id).c_str(), str_id);
	return llvm::ConstantInt::get(
			llvm::IntegerType::get(llvmContext(), bits),
			llvm::StringRef(session()->str(str_id)),
			radix);
}


llvm::LLVMContext& TransIR::llvmContext() {
	return llvm::getGlobalContext();
}


llvm::Function* TransIR::llvmCurrFn() {
	return _builder.GetInsertBlock()->getParent();
}


ExprValType TransIR::exprValType(Expr* expr) {
	switch (expr->node_type()) {
		case AST_EXPR_EMPTY:
		case AST_EXPR_FN:
		case AST_EXPR_BLOCK:
		case AST_EXPR_IF:
		case AST_EXPR_LITERAL:
		case AST_EXPR_CALL:
		case AST_EXPR_UNARY:
		case AST_EXPR_BINARY:
		case AST_EXPR_LOGICAL:
		case AST_EXPR_CONDITIONAL:
		case AST_EXPR_ASSIGNMENT:
			return EXPR_VAL_TYPE_R;
		case AST_EXPR_MEMBER:
			return EXPR_VAL_TYPE_L;
		case AST_EXPR_IDENT:
			ExprIdent* ident = static_cast<ExprIdent*>(expr);
			Symbol* symbol = session()->symbol_table()->value(ident->id().symbol_id());
			ASSERT_NE(symbol, NULL, SRCPOS);
			Type* type = session()->type_table()->getType(symbol->type_id());
			ASSERT_NE(type, NULL, SRCPOS);
			if (type->isFunctionType()) {
				return EXPR_VAL_TYPE_R;
			} else {
				return EXPR_VAL_TYPE_L;
			}
	}
	FATAL("Unknown expr type: %d", expr->node_type());
}
