#ifndef RING_RINGI_ENVIRONMENT_H
#define RING_RINGI_ENVIRONMENT_H


#include "../common.h"
#include "../ringc/session/session.h"
#include "values.h"
#include <map>
#include <stack>
using namespace ring::ringc;
using namespace std;


namespace ring {
namespace ringi {


// A mapping from symbol id to its value.
typedef map<SymbolId, Value*> EnvMap;

// Environment is a map from name to memory(value).
// Environment is created when execution flow enters a scoping structure like module or block, so
// does scope however environments is runtime context whereas scope is lexical context. So don't be
// confused between them.
class Environment {
	ADD_PROPERTY_P(session, Session)
	ADD_PROPERTY_P(parent,Environment)
	ADD_PROPERTY(node_id, NodeId)

public:
	// Constructor of this class.
	// It takes session, parent environment, and ID of the correspoding scoping AST node.
	Environment(Session* session, Environment* parent, NodeId node_id);

	void addSymbol(SymbolId symbol_id, Value* value);
	bool contains(SymbolId symbol_id);
	Value* value(SymbolId symbol_id);
	void value(SymbolId symbol_id, Value* value);

	Value* pop();
	void push(Value* value);
	void clear();
	Value* top();

protected:
	EnvMap _map;
	stack<Value*> _st;
};


} // namespace ringi
} // namespace ring


#endif
