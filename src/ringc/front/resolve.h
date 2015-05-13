#ifndef RING_RINGC_FRONT_RESOLVE_H
#define RING_RINGC_FRONT_RESOLVE_H


namespace ring {
namespace ringc {
	class Resolver;
}}


#include "../../common.h"
#include "../session/session.h"
#include "../syntax/ast.h"
#include "scope.h"
#include "resolve_symbol.h"
using namespace ring::ringc::ast;


namespace ring {
namespace ringc {


class Resolver {
	ADD_PROPERTY_P(session, Session)
	friend class SymbolResolver;

public:
	Resolver(Session* session);

	void resolve(AstNode* node);

	// Resolve ast.
	// - Resolve parent of nodes: All node of the AST become refer to their parent.
	void resolveAst(AstNode* node);

	// Resolve symbols.
	// Find name and its definition position.
	// When meet new name definition, create new entry on symbol table in scope chain.
	void resolveSymbol(AstNode* node);

	void resolveType(AstNode* node);
};


} // namespace ringc
} // namespace ring


#endif