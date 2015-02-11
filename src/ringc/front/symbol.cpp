#include "symbol.h"
using namespace ring::ringc;



Symbol::Symbol(NodeId node_id, NameId name_id, ScopeId scope_id, TypeId ty)
		: _id()
		, _node_id(node_id)
		, _name_id(name_id)
		, _scope_id(scope_id)
		, _ty(ty)
		, _llvm_val(NULL) {
}
