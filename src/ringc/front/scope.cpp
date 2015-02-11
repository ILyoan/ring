#include "scope.h"
#include <cassert>
using namespace ring::ringc;
using namespace std;


Scope::Scope(Session* session, Scope* parent, NodeId node_id)
		: _scope_id()
		, _node_id(node_id)
		, _parent(parent)
		, _session(session)
		, _symbol_map(NameId(), SymbolId())
		, _llvm_bb(NULL) {
}


bool Scope::isTopmost() const {
	return _parent == NULL;
}


SymbolId Scope::addSymbol(NodeId node_id, NameId name_id, TypeId ty) {
	assert(!_symbol_map.containsL(name_id));
	// create new symbol and add it to symbol table.
	Symbol* new_symbol = new Symbol(node_id, name_id, _scope_id, ty);
	SymbolId id = session()->symbol_table()->add(new_symbol);
	new_symbol->id(id);
	// add the symbol to this scope.
	_symbol_map.add(name_id, id);
	return id;
}


SymbolId Scope::findSymbol(NameId name_id) {
	return _symbol_map.fromL(name_id);
}
