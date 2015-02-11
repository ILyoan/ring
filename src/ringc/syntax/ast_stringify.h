#ifndef RING_RINGC_SYNTAX_AST_STRINGIFY_H
#define RING_RINGC_SYNTAX_AST_STRINGIFY_H


namespace ring {
namespace ringc {
namespace ast {
	class AstStringify;
}}}


#include <string>
using namespace std;

#include "ast.h"
#include "../session/session.h"
#include "../../common.h"


namespace ring {
namespace ringc {
namespace ast {


class AstStringify {
	ADD_PROPERTY_P(session, Session);
public:
	AstStringify(Session* session);

	static string toString(AssignmentOp op);
	static string toString(LogicalOp op);
	static string toString(BinaryOp op);
	static string toString(UnaryOp op);
	static string toString(TypePrimType prim_type);

	string toString(TypeId ty);
	string toString(Type* type);
	string toString(TypePrim* type);
	string toString(TypeFunc* type);
	string toString(TypeArray* type);

	AST_HANDLER_DECL_RET(toString, string)

protected:
	string toStringAst(AstNode* node, const string& body);
	string toStringExpr(Expr* expr, const string& body);
};


} // namespace ast
} // namespace ringc
} // namespace ring


#endif