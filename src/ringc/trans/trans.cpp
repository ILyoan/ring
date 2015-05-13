#include "trans.h"
#include "trans_ir.h"
using namespace ring::ringc;


Trans::Trans(Session* session)
		: _session(session) {
}


llvm::Module* Trans::trans(ast::Module* module) {
	TransIR trans_ir(session());
	return trans_ir.trans(module);
}