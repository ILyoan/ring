#ifndef RING_RINGC_FRONT_SYMBOL_H
#define RING_RINGC_FRONT_SYMBOL_H


namespace ring {
namespace ringc {
	class Symbol;
}}


#include "../../common.h"
#include "../syntax/ast.h"
#include "../session/session.h"
using namespace ring::ringc::ast;

#include "llvm/IR/Value.h"


namespace ring {
namespace ringc {


class Symbol {
	ADD_PROPERTY(id, SymbolId)
	ADD_PROPERTY(node_id, NodeId)
	ADD_PROPERTY(name_id, NameId)
	ADD_PROPERTY(scope_id, ScopeId)
	ADD_PROPERTY(type_id, TypeId)
	ADD_PROPERTY_P(llvm_val, llvm::Value)

public:
	Symbol(NodeId node_id, NameId name_id, ScopeId scope_id, TypeId type_id);
};


} // namespace ringc
} // namespace ring


#endif
