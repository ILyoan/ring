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
	static string toString(LiteralType lit_type);
	static string toString(PrimitiveTypeType prim_type);

	string toString(TypeId type_id);
	string toString(Type* type);
	string toString(PrimitiveType* type);
	string toString(FunctionType* type);

	AST_HANDLER_DECL_RET(toString, string)

protected:
	string toStringAst(AstNode* node, const string& body);
	string toStringExpr(Expr* expr, const string& body);
};


} // namespace ast
} // namespace ringc
} // namespace ring


#endif