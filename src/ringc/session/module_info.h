#ifndef RING_RINGC_SESSION_MODULE_INFO_H
#define RING_RINGC_SESSION_MODULE_INFO_H


namespace ring {
namespace ringc {
	class ModuleInfo;
}}


#include <string>
#include "../syntax/ast.h"
#include "llvm/IR/Module.h"


namespace ring {
namespace ringc {


class ModuleInfo {
	ADD_PROPERTY_P(ast_module, ast::Module)
	ADD_PROPERTY_P(llvm_module, llvm::Module)
	ADD_PROPERTY(name, std::string)
	ADD_PROPERTY(src_path, std::string)
	ADD_PROPERTY(obj_path, std::string)
};


} // namespace ringc
} // namespace ring


#endif