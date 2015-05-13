#ifndef RING_RINGI_INTERPRET_H
#define RING_RINGI_INTERPRET_H


namespace ring {
namespace ringi {
	class Interpret;
}}


#include "../common.h"
#include "../ringc/session/session.h"
#include "../ringc/front/parser.h"
#include "../ringc/syntax/ast.h"
#include "environment.h"
#include "eval.h"
using namespace ring::ringc;
using namespace ring::ringc::ast;
using namespace ring::ringi;

namespace ring {
namespace ringi {


class Interpret {
	ADD_PROPERTY_P(session, Session)
	ADD_PROPERTY_P(env_base, Environment)
	ADD_PROPERTY_P(env_curr, Environment)

public:
	Interpret(Session* session);

	// Evaluates the given program.
	bool EvaluateProgram(const string& program);
	bool EvaluateProgram(FILE* fp);

	// Evaluates the given expression.
	bool EvaluateExpression(const string& expr);

	// Enter into a block or scoping structure.
	void enterBlock(NodeId node_id);

	// Exit from a block or scoping structure.
	void exitBlock(NodeId node_id);

	// Interprets a symbol initialized with `value`.
	void initSymbol(SymbolId symbol_id, Value* value);

	// Find the value of a symbol from environment.
	Value* getValueFromEnv(SymbolId symbol_id);


	void initREPL(Module* module, ExprBlock* block);

protected:
	bool EvaluateProgram(Parser* parser);
	void addSymbolEnv(Environment* env, SymbolId symbol_id);
	Environment* findEnvForSymbol(SymbolId symbol_id);

protected:
	Evaluator* _eval;

	Module* _repl_module;
	ExprBlock* _repl_block;
};


} // namespace ringi
} // namespace ring


#endif
