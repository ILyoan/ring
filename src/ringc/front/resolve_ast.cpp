#include "resolve_ast.h"
using namespace ring::ringc;


AstResolver::AstResolver() {
}


void AstResolver::resolve(AstNode* node) {
	visit(node);
}

void AstResolver::onVisitPreNode(AstNode* node) {
	if (_st.empty()) {
		node->parent(NULL);
	} else {
		node->parent(_st.top());
	}
	_st.push(node);
}


void AstResolver::onVisitPostNode(AstNode* node) {
	_st.pop();
}

