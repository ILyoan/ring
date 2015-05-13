#ifndef RING_RINGC_FRONT_RESOLVE_AST_H
#define RING_RINGC_FRONT_RESOLVE_AST_H


namespace ring {
namespace ringc {
	class AstResolver;
}}


#include "../syntax/ast_visitor.h"
#include "../syntax/ast.h"
#include <stack>
using namespace ring::ringc::ast;
using namespace std;


namespace ring {
namespace ringc {


class AstResolver : public AstVisitor {
public:
	AstResolver();

	// Takes an ast node and start resolving.
	void resolve(AstNode* node);

protected:
	void onVisitPreNode(AstNode* node);
	void onVisitPostNode(AstNode* node);

protected:
	stack<const AstNode*> _st;
};


} // namespace ringc
} // namespace ring

#endif
