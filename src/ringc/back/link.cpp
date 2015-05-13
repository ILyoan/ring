#include "link.h"
using namespace ring::ringc;

#include <string>
using namespace std;

#include "llvm/ADT/Triple.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"


Link::Link(Session* session)
		: _session(session) {
	// Initialize targets info.
	initializeLLVMTargets();
}


bool Link::link(ModuleInfo* module) {
	writeModuleObject(module);
	linkOutput(module);

	return true;
}


bool Link::writeModuleObject(ModuleInfo* module) {
	// LLVM pass manager
	llvm::PassManager pm;

	// LLVM target
	string triple(llvm::Triple::normalize("x86_64-pc-linux-gnu"));
	string cpu("generic");
	string err;
	llvm::TargetOptions llvm_target_options;
	const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, err);

	llvm::TargetMachine* tm = target->createTargetMachine(triple, cpu, "", llvm_target_options);

	tm->addAnalysisPasses(pm);

	// Output stream.
	string os_error;
	llvm::raw_fd_ostream os(module->obj_path().c_str(), os_error, llvm::sys::fs::F_Binary);
	llvm::formatted_raw_ostream fos(os);
	tm->addPassesToEmitFile(pm, fos, llvm::TargetMachine::CGFT_ObjectFile, false);

	pm.run(*module->llvm_module());

	delete tm;

	return true;
}


bool Link::linkOutput(ModuleInfo* module) {
	string link_command(session()->config()->linker());
	link_command += " ";
	link_command += module->obj_path();
	link_command += " ";
	link_command += session()->config()->link_opt();
	link_command += " -o ";
	link_command += module->name();
	int res = system(link_command.c_str());
	return true;
}


void Link::initializeLLVMTargets() {
	LLVMInitializeX86TargetInfo();
	LLVMInitializeX86Target();
	LLVMInitializeX86TargetMC();
	LLVMInitializeX86AsmPrinter();
	LLVMInitializeX86AsmParser();
}