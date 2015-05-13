#ifndef RING_RINGC_FRONT_RESOLVE_SYMBOL_H
#define RING_RINGC_FRONT_RESOLVE_SYMBOL_H


namespace ring {
namespace ringc {
	class SymbolResolver;
}}


#include "../../common.h"
#include "../syntax/ast_visitor.h"
#include "../syntax/ast.h"
#include "resolve.h"
using namespace ring::ringc::ast;


namespace ring {
namespace ringc {


// Symbol resolver resovles symbols in a program.
// It takes an AST node and resolves symbols in the subtree rooted from the given node.
// As result, symbol table on the Session object will be filled with resolved symbol definitions.
// And each name in the AST node would refers correnct definition node for it.
class SymbolResolver : public AstVisitor {
	ADD_PROPERTY_P(resolver, Resolver)
	ADD_PROPERTY_P(session, Session)

public:
	SymbolResolver(Resolver* resolver);

	// Takes an ast node and start resolving.
	void resolve(AstNode* node);

protected:
	// Create a new scope for given AST node.
	ScopeId newScope(AstNode* node);

	// Find most-closely nesting scope of given AST node.
	ScopeId findNearestScope(AstNode* node);

	// Find symbol for given name from the most-closely nesting scope of given AST node.
	SymbolId findSymbol(AstNode* node, NameId name_id);

	// Add symbol.
	SymbolId addSymbol(ScopeId scope_id, NodeId node_id, NameId name_id, TypeId type_id);

protected:
	ADD_VISITOR_PRE(Module);
	ADD_VISITOR_PRE(Use);
	ADD_VISITOR_PRE(Extern);
	ADD_VISITOR_PRE(Let);
	ADD_VISITOR_POST(Let);
	ADD_VISITOR_PRE(ExprBlock);
	ADD_VISITOR_PRE(ExprIdent);

	void processLet(Let* let);
	void processExtern(Extern* ext);
};


} // namespace ringc
} // namespace ring


#endif
