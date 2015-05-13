#ifndef RING_RINGC_FRONT_RESOLVE_TYPE_H
#define RING_RINGC_FRONT_RESOLVE_TYPE_H


namespace ring {
namespace ringc {
	class SymbolType;
}}


#include "../../common.h"
#include "../syntax/ast.h"
#include "resolve.h"
using namespace ring::ringc::ast;


namespace ring {
namespace ringc {


class TypeResolver : public AstVisitor{
	ADD_PROPERTY_P(resolver, Resolver)
	ADD_PROPERTY_P(session, Session)

public:
	TypeResolver(Resolver* resolver);

	void resolve(AstNode* node);

protected:
	ADD_VISITOR_POST(Let);
	ADD_VISITOR_POST(ExprEmpty);
	ADD_VISITOR_POST(ExprBlock);
	ADD_VISITOR_POST(ExprFn);
	ADD_VISITOR_POST(ExprIf);
	ADD_VISITOR_POST(ExprIdent);
	ADD_VISITOR_POST(ExprLiteral);
	ADD_VISITOR_POST(ExprMember);
	ADD_VISITOR_POST(ExprCall);
	ADD_VISITOR_POST(ExprUnary);
	ADD_VISITOR_POST(ExprBinary);
	ADD_VISITOR_POST(ExprLogical);
	ADD_VISITOR_POST(ExprConditional);
	ADD_VISITOR_POST(ExprAssignment);

	TypeId getPrimTypeId(const PrimTypeKey& prim_type_key);
};


} // namespace ringc
} // namespace ring


#endif

