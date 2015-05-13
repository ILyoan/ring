#include "resolve.h"
#include "resolve_ast.h"
#include "resolve_symbol.h"
#include "resolve_type.h"
using namespace ring::ringc;


Resolver::Resolver(Session* session)
		: _session(session) {
}


void Resolver::resolve(AstNode* node) {
	resolveAst(node);
	resolveSymbol(node);
	resolveType(node);
}


void Resolver::resolveAst(AstNode* node) {
	AstResolver resolver;
	resolver.resolve(node);
}


void Resolver::resolveSymbol(AstNode* node) {
	SymbolResolver resolver(this);
	resolver.resolve(node);
}


void Resolver::resolveType(AstNode* node) {
	TypeResolver resolver(this);
	resolver.resolve(node);
}