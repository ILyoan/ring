#include "environment.h"
using namespace ring::ringi;


Environment::Environment(Session* session, Environment* parent, NodeId node_id)
		: _session(session)
		, _parent(parent)
		, _node_id(node_id) {
}


void Environment::addSymbol(SymbolId symbol_id, Value* value) {
	if (contains(symbol_id)) {
		FATAL("Duplicate definition on a environment %d", symbol_id);
	} else {
		LOG(LOG_TRACE, "Add symbol %d to environment", symbol_id);
	}
	_map[symbol_id] = value;
}


bool Environment::contains(SymbolId symbol_id) {
	return _map.count(symbol_id) > 0;
}


Value* Environment::value(SymbolId symbol_id) {
	if (contains(symbol_id)) {
		return _map[symbol_id];
	} else {
		return Value::UndefinedVal();
	}
}


void Environment::value(SymbolId symbol_id, Value* value) {
	_map[symbol_id] = value;
}


Value* Environment::top() {
	if (_st.empty()) {
		return Value::NoneVal();
	} else {
		return _st.top();
	}
}


Value* Environment::pop() {
	if (_st.empty()) {
		FATAL("%s", "EMPTY value stack");
	}
	Value* res = _st.top();
	_st.pop();
	return res;
}


void Environment::push(Value* value) {
	_st.push(value);
}


void Environment::clear() {
	while (!_st.empty()) {
		_st.pop();
	}
}
