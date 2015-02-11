#include "resolve_symbol.h"
#include "scope.h"
#include <algorithm>
using namespace ring::ringc;
using namespace std;


SymbolResolver::SymbolResolver(Resolver* resolver)
		: _resolver(resolver)
		, _session(resolver->session()) {
}


void SymbolResolver::resolve(AstNode* node) {
	visit(node);
}


// Create a new scope for given AST node.
ScopeId SymbolResolver::newScope(AstNode* node) {
	// Ast node should not be NULL!
	assert(node != NULL);

	Session* sess = session();
	// Check if desired scope already exists.
	if (sess->scope_node_map()->containsR(node->node_id())) {
		// There is! get the scope.
		ScopeId scope_id = sess->scope_node_map()->fromR(node->node_id());
		Scope* scope = sess->scope_table()->value(scope_id);
		assert(scope->scope_id() == scope_id);
		assert(scope->node_id() == node->node_id());
		return scope->scope_id();
	} else {
		// There isn't! create new one.
		Scope* scope = new Scope(sess, NULL, node->node_id());
		scope->scope_id(sess->scope_table()->add(scope));
		sess->scope_node_map()->add(scope->scope_id(), node->node_id());
		return scope->scope_id();
	}
}


// Find most-closely nesting scope of given AST node.
ScopeId SymbolResolver::findNearestScope(AstNode* node) {
	// Ast node should not be NULL!
	assert(node != NULL);

	Session* sess = session();
	ScopeId scope_id = sess->scope_node_map()->fromR(node->node_id());
	if (scope_id.some()) {
		Scope* scope = sess->scope_table()->value(scope_id);
		assert(scope != NULL && scope->scope_id() == scope_id);
		return scope->scope_id();
	} else {
		return findNearestScope(node->parent());
	}
}


// Find symbol for given name from the most-closely nesting scope of given AST node.
SymbolId SymbolResolver::findSymbol(AstNode* node, NameId name_id) {
	if (node == NULL) {
		// This means that there is no definition for the symbol in this program
		return -1;
	}

	Session* sess = session();

	// First, find the nearest enclosing scope.
	ScopeId scope_id = findNearestScope(node);
	Scope* scope = sess->scope_table()->value(scope_id);
	assert(scope != NULL);

	// Get symbol(AST node id for the definition of this name) from the scope.
	SymbolId symbol_id = scope->findSymbol(name_id);
	if (symbol_id.some()) {
		// If the scope has symbol for the name, returns the ID for the symbol.
		return symbol_id;
	} else {
		// If there is no symbol for this name, find from further scope.
		// AstNode for current socpe.
		NodeId node_id = sess->scope_node_map()->fromL(scope->scope_id());
		assert(node_id.some());
		AstNode* node = sess->ast_table()->value(node_id);
		assert(node != NULL);
		// Resume search from parent node of this scope.
		return findSymbol(node->parent(), name_id);
	}
}


// Add symbol.
SymbolId SymbolResolver::addSymbol(
		ScopeId scope_id, NodeId node_id, NameId name_id, TypeId ty) {
	assert(scope_id.some() && name_id.some());

	Scope* scope = session()->scope_table()->value(scope_id);

	// Check if there were the same symbol in the scope.
	if (scope->findSymbol(name_id).some()) {
		ERROR("Duplicated symbol: %s", session()->str_table()->value(name_id).c_str());
		return SymbolId();
	} else {
		return scope->addSymbol(node_id, name_id, ty);
	}
}


void SymbolResolver::onVisitPreModule(Module* module) {
	newScope(module);
}


void SymbolResolver::onVisitPreUse(Use* use) {
}


void SymbolResolver::processExtern(Extern* ext) {
	// Add new symbol for this extern on enclosing scope.
	ScopeId scope_id = findNearestScope(ext);
	ASSERT(scope_id.some(), "No scope for: extern %d", ext->node_id());
	SymbolId symbol_id = addSymbol(
			scope_id, ext->node_id(), ext->name().name_id(), ext->ty());
	ext->name(Ident(ext->name().name_id(), symbol_id));
}


void SymbolResolver::onVisitPreExtern(Extern* ext) {
	processExtern(ext);
}


void SymbolResolver::processLet(Let* let) {
	// Add new symbol for this let binding on enclosing scope.
	ScopeId scope_id = findNearestScope(let);
	ASSERT(scope_id.some(), "No scope for: let %d", let->node_id());
	SymbolId symbol_id = addSymbol(
			scope_id, let->node_id(), let->name().name_id(), let->ty());
	let->name(Ident(let->name().name_id(), symbol_id));
}


// Let binding for function should treated before function itself since there may be recursive call.
void SymbolResolver::onVisitPreLet(Let* let) {
	if (let->ty().none()) {
		if (let->expr()->isExprFn()) {
			ExprFn* fn = static_cast<ExprFn*>(let->expr());
			ASSERT(fn->ty().some(), SRCPOS);
			let->ty(fn->ty());
		}
	}
	if (session()->type_table()->isFuncType(let->ty())) {
		processLet(let);
		// Check if this function is a "main".
		if (session()->str_table()->value(let->name().name_id()) == MainName) {
			if (!session()->setMain(let->node_id())) {
				ERROR("Duplicated main function: %d %d", session()->main(), let->node_id());
			}
		}
	}
}


// Let binding for except function should treated after its assignee.
void SymbolResolver::onVisitPostLet(Let* let) {
	if (!session()->type_table()->isFuncType(let->ty())) {
		processLet(let);
	}
}


void SymbolResolver::onVisitPreExprBlock(ExprBlock* block) {
	// Block creates new scope.
	ScopeId scope_id = newScope(block);
	NodeId block_id = block->node_id();

	// If this block is a function block, add function arguments symbols to the scope.
	if (block->parent()->isExprFn()) {
		ExprFn* fn = static_cast<ExprFn*>(block->parent());
		TypeFunc* fn_type = session()->type_table()->getFuncType(fn->ty());
		const auto& ty_args = fn_type->ty_args();
		auto& args = fn->args();
		if (ty_args.size() != args.size()) {
			FATAL("Number of argument type and name are not the same: fn expr %d", fn->node_id());
		}
		for (int i = 0; i < ty_args.size(); ++i) {
			Ident name = args[i];
			TypeId ty = ty_args[i];
			SymbolId symbol_id = this->addSymbol(scope_id, NodeId(), name.name_id(), ty);
			args[i] = Ident(name.name_id(), symbol_id);
		}
	}
}



void SymbolResolver::onVisitPreExprIdent(ExprIdent* ident) {
	// Find symbol for this name.
	SymbolId symbol_id = findSymbol(ident, ident->id().name_id());
	if (symbol_id.some()) {
		// Set symbol id for this identifier.
		Ident id = ident->id();
		id.symbol_id(symbol_id);
		ident->id(id);
	} else {
		// There is no symbol for this name. Report it.
		ERROR("Unresolved name `%s`", session()->str_table()->value(ident->id().name_id()).c_str());
	}
}
