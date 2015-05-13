#include "ast_printer.h"
using namespace ring::ringc::ast;


#define PREFIX()\
	fprintf(_fd, "%*s ", _indent, " ");

#define PRINT(node)\
	PREFIX();\
	fprintf(_fd, "%s\n", session()->ast_str()->toString(node).c_str());


AstPrinter::AstPrinter(Session* session, FILE* fd, StrTable* str_table)
		: _session(session)
		, _fd(fd)
		, _indent(1) {
}


void AstPrinter::print(AstNode* node) {
	visit(node);
}


void AstPrinter::onVisitPreNode(AstNode* node) {
	_indent += 2;
}


void AstPrinter::onVisitPostNode(AstNode* node) {
	_indent -= 2;
}


void AstPrinter::onVisitPreModule(Module* module) {
	PRINT(module);
}


void AstPrinter::onVisitPreUse(Use* use) {
	PRINT(use);
}


void AstPrinter::onVisitPreExtern(Extern* ext) {
	PRINT(ext);
}


void AstPrinter::onVisitPreLet(Let* let) {
	PRINT(let);
}


void AstPrinter::onVisitPreExprEmpty(ExprEmpty* empty) {
	PRINT(empty);
}


void AstPrinter::onVisitPreExprBlock(ExprBlock* block) {
	PRINT(block);
}


void AstPrinter::onVisitPreExprFn(ExprFn* fn) {
	PRINT(fn);
}


void AstPrinter::onVisitPreExprIf(ExprIf* if_) {
	PRINT(if_);
}


void AstPrinter::onVisitPreExprIdent(ExprIdent* ident) {
	PRINT(ident);
}


void AstPrinter::onVisitPreExprLiteral(ExprLiteral* lit) {
	PRINT(lit);
}


void AstPrinter::onVisitPreExprMember(ExprMember* member) {
	PRINT(member);
}


void AstPrinter::onVisitPreExprCall(ExprCall* call) {
	PRINT(call);
}


void AstPrinter::onVisitPreExprUnary(ExprUnary* unary) {
	PRINT(unary);
}


void AstPrinter::onVisitPreExprBinary(ExprBinary* binary) {
	PRINT(binary);
}


void AstPrinter::onVisitPreExprLogical(ExprLogical* logical) {
	PRINT(logical);
}


void AstPrinter::onVisitPreExprConditional(ExprConditional* condition) {
	PRINT(condition);
}


void AstPrinter::onVisitPreExprAssignment(ExprAssignment* assign) {
	PRINT(assign);
}
