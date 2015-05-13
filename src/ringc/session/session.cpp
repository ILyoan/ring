#include "session.h"
using namespace ring::ringc;


Session::Session(Config* config)
		: _config(config)
		, _str_table(new StrTable(""))
		, _ast_table(new AstTable(this))
		, _type_table(new TypeTable(this))
		, _scope_table(new ScopeTable(NULL))
		, _symbol_table(new SymbolTable(NULL))
		, _scope_node_map(new ScopeNodeMap(ScopeId(), NodeId()))
		, _diagnostic(new Diagnostic(config->log_level()))
		, _ast_fac(new AstFactory(this))
		, _ast_str(new AstStringify(this))
		, _main() {
}


Session::~Session() {
	delete _ast_str;
	delete _ast_fac;
	delete _diagnostic;
	delete _scope_node_map;
	delete _scope_table;
	delete _ast_table;
	delete _str_table;
}


bool Session::setMain(NodeId main) {
	if (_main.none()) {
		_main = main;
		return true;
	} else {
		return false;
	}
}


const string& Session::str(StrId str) const {
	return _str_table->value(str);
}