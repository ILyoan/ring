#ifndef RING_RINGC_FRONT_SCOPE_H
#define RING_RINGC_FRONT_SCOPE_H


namespace ring {
namespace ringc {
	class Scope;
}}


#include "../../common.h"
#include "../syntax/ast.h"
#include "../session/session.h"
#include "symbol.h"
using namespace ring::ringc::ast;

#include "llvm/IR/BasicBlock.h"


namespace ring {
namespace ringc {

// Mapping between name and symbol which is AST node where the name is defined.
typedef BiMap<NameId, SymbolId> NameSymbolMap;

// A Scope represent lexical scope region of source program.
// Ring language's lexical scope depends on block structure. It means each block generates a new
// scope and the scope's lifetime extends ultil the end of the block.
// Each scope contains following information:
//   - Symbol table: a set of "local names that declared in this scope"
class Scope {
	ADD_PROPERTY(scope_id, ScopeId)
	ADD_PROPERTY(node_id, NodeId) // AST node id corresponding to this scope.
	ADD_PROPERTY_P(parent, Scope) // Pointer to parent scope, NULL if this is topmost.
	ADD_PROPERTY_P(session, Session)
	ADD_PROPERTY_R(symbol_map, NameSymbolMap) // A bi-map between name id and symbol.
	ADD_PROPERTY_P(llvm_bb, llvm::BasicBlock)

public:
	// Constructor of this class.
	// It takes session, parent scope, and ID of the correspoding scoping AST node.
	Scope(Session* session, Scope* parent, NodeId node_id);

	// Returns whether this scope is top most.
	bool isTopmost() const;

	// Adds a mapping between name(string) and symbol.
	// This will happen when a definion occurs in this scope.
	SymbolId addSymbol(NodeId node_id, NameId name_id, TypeId ty);

	// Finds symbol for given name from this scope.
	// If there is no mapping in this scope, returns -1.
	SymbolId findSymbol(NameId name_id);
};


} // namespace ringc
} // namespace ring


#endif
