#include "interpret.h"
#include "eval.h"
#include "../ringc/session/diagnostic.h"
#include "../ringc/syntax/ast_printer.h"
#include "../ringc/front/parser.h"
#include "../ringc/front/resolve.h"
#include <algorithm>
#include <cassert>
using namespace ring::ringi;
using namespace ring::ringc;
using namespace std;


Interpret::Interpret(Session* session)
		: _session(session)
		, _env_base(new Environment(_session, NULL, NodeId()))
		, _env_curr(_env_base)
		, _eval(NULL)
		, _repl_module(NULL)
		, _repl_block(NULL) {
}


bool Interpret::EvaluateProgram(FILE* fp) {
	// Parses the program.
	Parser parser(fp, session());
	return EvaluateProgram(&parser);
}


bool Interpret::EvaluateProgram(const string& program) {
	Parser parser(program, session());
	return EvaluateProgram(&parser);
}


bool Interpret::EvaluateProgram(Parser* parser) {
	ScopeTracer tracer(session(), "Interpret::Evaluate()");

	// Parses the program.
	Module* module = parser->parseProgram();
	if (session()->diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
		return false;
	}

	// Resolves names.
	Resolver resolver(session());
	resolver.resolve(module);
	if (session()->diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
		return false;
	}

	// Prints AST
	if (session()->config()->print_ast()) {
		AstPrinter astPrinter(session(), stdout, session()->str_table());
		astPrinter.print(module);
	}

	// Check main function.
	if (session()->main().none()) {
		FATAL("%s", "main function not found");
	}

	// Evaluates it.
	Evaluator eval(this);
	_eval = &eval;
	Value* v = eval.evaluate(module);

	if (v) {
		LOG(LOG_INFO, "program evaluates %s", v->toString().c_str());
	}

	return true;
}


void Interpret::initREPL(Module* module, ExprBlock* block) {
	_repl_module = module;
	_repl_block = block;
	Resolver resolver(session());
	resolver.resolve(_repl_module);
	//resolver.resolve(_repl_block);
}


bool Interpret::EvaluateExpression(const string& expr_str) {
	string trim = expr_str;
	trim.erase(trim.find_last_not_of(" \n\r\t")+1);

	if (trim == "") {
		return true;
	}

	ScopeTracer tracer(session(), "Interpret::EvaluateExpression()");
	assert(_repl_module != NULL);
	assert(_repl_block != NULL);

	session()->diagnostic()->resetReport();

	// Parses the expression.
	Parser parser(trim, session());
	Stmt* stmt = parser.parseStatement();
	if (session()->diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
		return false;
	}

	// resolving
	Resolver resolver(session());
	resolver.resolveAst(stmt);

	_repl_block->addStatement(stmt);
	stmt->parent(_repl_block);

	// Resolves names.
	resolver.resolveSymbol(stmt);

	if (session()->diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
		return false;
	}

	if (session()->config()->print_ast()) {
		AstPrinter astPrinter(session(), stdout, session()->str_table());
		astPrinter.print(stmt);
	}

	// Evaluates it.
	Evaluator eval(this);
	_eval = &eval;
	Value* v = eval.evaluate(stmt);

	if (v) {
		printf("%s\n", v->toString().c_str());
	}

	env_curr()->clear();

	return true;
}


// Enter into a block or scoping structure.
// Entring a block incurs creation of new environment.
void Interpret::enterBlock(NodeId node_id) {
	LOG(LOG_TRACE, "Enter block: %d", node_id);

	// Create new environment for the block.
	Environment* new_env = new Environment(session(), env_curr(), node_id);

	// Updates environment stack.
	env_curr(new_env);

	// Check `node_id` to see if there is real block in the AST.
	if (node_id.none()) {
		// if `node_id` is `NoneNode` that means the block is not a acually a AST node.
		// It's a virtual block on interpreting environment.
	} else {
		// There is a block node (scoping structure) in the AST.
		// Find scope for this node.
		ScopeId scope_id = session()->scope_node_map()->fromR(node_id);
		if (scope_id.none()) {
			FATAL("No scope for: block %d", node_id);
		}
		Scope* scope = session()->scope_table()->value(scope_id);
		if (scope == NULL) {
			FATAL("No scope for: block %d, scope %d", node_id, scope_id);
		}
		// Add symbols defined in this scope to environment.
		vector<pair<SymbolId, NameId> > vec = scope->symbol_map().toListR();
		for_each(vec.begin(), vec.end(), [this, new_env](pair<SymbolId, NameId> item) {
			this->addSymbolEnv(new_env, item.first);
		});
	}
}


// Exit from a block or scoping structure.
void Interpret::exitBlock(NodeId node_id) {
	LOG(LOG_TRACE, "Exit block: %d", node_id);

	// The AST node connected to top environment shoud matched with the given node.
	if (env_curr()->node_id() != node_id) {
		FATAL("Environment missmatch: top is %d, but exit from %d", env_curr()->node_id(), node_id);
	}

	// Updates environment stack.
	Environment* curr = env_curr();
	env_curr(const_cast<Environment*>(curr->parent()));
	delete curr;
}


// Interprets a symbol initialized with `value`.
void Interpret::initSymbol(SymbolId symbol_id, Value* value) {
	LOG(LOG_TRACE, "Init symbol %d with value %s", symbol_id, value->toString().c_str());

	if (_repl_module) {
	}
	else if (!env_curr()->contains(symbol_id)) {
		// Invariant: current environment should contain the symbol uninitialized.
		FATAL("No memory for symbol %d in the environment", symbol_id);
	}

	// Set value.
	env_curr()->value(symbol_id, value);
}


// Get the value of a variable defined by `symbol_id` node.
Value* Interpret::getValueFromEnv(SymbolId symbol_id) {
	Environment* env = findEnvForSymbol(symbol_id);
	if (env == NULL) {
		FATAL("No memory for: symbol %d", symbol_id);
	}
	Value* value = env->value(symbol_id);
	if (value == Value::NoneVal() ||
			value == Value::UndefinedVal() ||
			value == Value::UninitailizedVal()) {
		FATAL("Uninitalized value for: symbol %d %p", symbol_id, value);
	}
	return value;
}


void Interpret::addSymbolEnv(Environment* env, SymbolId symbol_id) {
	Symbol* symbol = session()->symbol_table()->value(symbol_id);
	env->addSymbol(symbol_id, Value::UninitailizedVal());
}


// Finds environment where the given node is defined.
// Returns NULL if there is no such environment.
Environment* Interpret::findEnvForSymbol(SymbolId symbol_id) {
	for (Environment* env = env_curr(); env != NULL; env = const_cast<Environment*>(env->parent())) {
		if (env->contains(symbol_id)) {
			return env;
		}
	}
	return NULL;
}
