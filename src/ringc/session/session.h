#ifndef RING_RINGC_SESSION_SESSION_H
#define RING_RINGC_SESSION_SESSION_H


namespace ring {
namespace ringc {
	class Session;
}}


#include "../../common.h"
#include "ast_table.h"
#include "type_table.h"
#include "diagnostic.h"
#include "config.h"
#include "../util/table_map.h"
#include "../util/bi_map.h"
#include "../front/symbol.h"
#include "../front/scope.h"
#include "../syntax/ast.h"
#include "../syntax/ast_factory.h"
#include "../syntax/ast_stringify.h"
using namespace ring::ringc::ast;


namespace ring {
namespace ringc {

typedef TableMap<StrId, string> StrTable;
typedef TableMap<ScopeId, Scope*> ScopeTable;
typedef TableMap<SymbolId, Symbol*> SymbolTable;
typedef BiMap<ScopeId, NodeId> ScopeNodeMap;

class Session {
public:
	Session(Config* config);
	~Session();

	ADD_PROPERTY_P(config, Config)
	ADD_PROPERTY_P(str_table, StrTable)
	ADD_PROPERTY_P(ast_table, AstTable)
	ADD_PROPERTY_P(type_table, TypeTable)
	ADD_PROPERTY_P(scope_table, ScopeTable)
	ADD_PROPERTY_P(symbol_table, SymbolTable)
	ADD_PROPERTY_P(scope_node_map, ScopeNodeMap)
	ADD_PROPERTY_P(diagnostic, Diagnostic)
	ADD_PROPERTY_P(ast_fac, AstFactory)
	ADD_PROPERTY_P(ast_str, AstStringify)
	ADD_PROPERTY(main, NodeId);

public:
	bool setMain(NodeId main);
	const string& str(StrId str_id) const;
};


} // namespace ringc
} // namespace ring


#endif
