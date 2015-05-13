#include <cstdio>
#include "../ringc/session/session.h"
#include "../ringc/syntax/ast_factory.h"
#include "../ringi/interpret.h"
using namespace ring::ringc;
using namespace ring::ringc::ast;
using namespace ring::ringi;

#if 0
int main(int argc, char** argv) {
	Config config;
	for (int i = 1; i < argc; ++i) {
		config.parseArgument(argv[i]);
	}

	Session session(&config);
	Interpret interpret(&session);

	if (config.source() != "") {
		// Run program.
		FILE* fp = fopen(config.source().c_str(), "r");
		if (fp == NULL) {
			perror("Could not open input file: ");
		} else {
			interpret.EvaluateProgram(fp);
		}
		fclose(fp);
	} else {
		// REPL.
		// Creates virtual module and block for REPL.
		AstFactory ast_factory(&session);
		Module* module = ast_factory.createModule();
		FunctionType* fn_type = static_cast<FunctionType*>(
				session.type_table()->getType(ast_factory.getFuncTypeId(
					make_pair(vector<TypeId>(), PRIM_TYPE_NIL))));
		ExprFn* fn = ast_factory.createExprFn(
				fn_type, vector<Ident>(), ast_factory.createExprBlock());
		Let* let_fn = ast_factory.createLet(
				true,
				false,
				Ident(0),
				fn_type,
				fn
				);
		module->addDecl(let_fn);
		interpret.initREPL(module, const_cast<ExprBlock*>(fn->body()));
		// Enter into the virtual module and block.
		interpret.enterBlock(module->node_id());
		interpret.enterBlock(fn->body()->node_id());

		// Loop.
		while (true) {
			char expr[1024];
			printf("> ");
			if (fgets(expr, 1024, stdin)) {
				interpret.EvaluateExpression(expr);
			} else {
				break;
			}
		}

		// Exit from the virtual block and module.
		interpret.exitBlock(fn->body()->node_id());
		interpret.exitBlock(module->node_id());
	}

	return 0;
}
#else
int main() {
	return 0;
}
#endif