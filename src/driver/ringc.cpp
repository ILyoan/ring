#include <cstdio>
#include "../ringc/session/session.h"
#include "../ringc/session/module_info.h"
#include "../ringc/front/parser.h"
#include "../ringc/front/resolve.h"
#include "../ringc/trans/trans.h"
#include "../ringc/back/link.h"
#include "../ringc/syntax/ast_printer.h"
#include "../ringc/util/util.h"
#include "llvm/IR/Module.h"
using namespace ring::ringc;


int main(int argc, char** argv) {
	Config config;
	for (int i = 1; i < argc; ++i) {
		config.parseArgument(argv[i]);
	}

	if (config.source() != "") {
		// Run program.
		FILE* fp = fopen(config.source().c_str(), "r");
		if (fp == NULL) {
			perror("Could not open input file: ");
		}

		// Session.
		Session session(&config);

		// Module Info
		ModuleInfo module;
		module.name(RingcUtil::fileName(config.source()));
		module.src_path(config.source());
		module.obj_path(module.name() + ".o");

		// Parses the program.
		Parser parser(fp, &session);
		module.ast_module(parser.parseProgram());
		fclose(fp);
		if (session.diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
			return 1;
		}

		// Prints AST
		if (session.config()->print_ast()) {
			fprintf(stdout, "Parser -> AST\n");
			AstPrinter astPrinter(&session, stdout, session.str_table());
			astPrinter.print(module.ast_module());
		}

		// Resolve.
		Resolver resolver(&session);
		resolver.resolve(module.ast_module());
		if (session.diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
			return 1;
		}

		// Prints AST
		if (session.config()->print_ast()) {
			fprintf(stdout, "Resolver -> AST\n");
			AstPrinter astPrinter(&session, stdout, session.str_table());
			astPrinter.print(module.ast_module());
		}

		// Trans
		Trans trans(&session);
		module.llvm_module(trans.trans(module.ast_module()));
		if (session.diagnostic()->mostSignificantLevel() <= REPORT_ERROR) {
			return 1;
		}

		// Link
		Link link(&session);
		link.link(&module);

	} else {
		fprintf(stderr, "Usage: ringc [OPTIONS] INPUT\n");
	}
	return 0;
}
