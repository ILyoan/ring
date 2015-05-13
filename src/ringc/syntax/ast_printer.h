#ifndef RING_RINGC_SYNTAX_AST_PRINTER_H
#define RING_RINGC_SYNTAX_AST_PRINTER_H


#include "../session/session.h"
#include "ast_visitor.h"
#include "ast_stringify.h"
using namespace ring::ringc;


namespace ring {
namespace ringc {
namespace ast {


class AstPrinter : public AstVisitor {
	ADD_PROPERTY_P(session, Session)

public:
	AstPrinter(Session* session, FILE* fd, StrTable* str_table);

	void print(AstNode* node);

protected:
	ADD_VISITOR_PRE(Module);
	ADD_VISITOR_PRE(Use);
	ADD_VISITOR_PRE(Extern);
	ADD_VISITOR_PRE(Let);
	ADD_VISITOR_PRE(ExprEmpty);
	ADD_VISITOR_PRE(ExprBlock);
	ADD_VISITOR_PRE(ExprFn);
	ADD_VISITOR_PRE(ExprIf);
	ADD_VISITOR_PRE(ExprIdent);
	ADD_VISITOR_PRE(ExprLiteral);
	ADD_VISITOR_PRE(ExprMember);
	ADD_VISITOR_PRE(ExprCall);
	ADD_VISITOR_PRE(ExprUnary);
	ADD_VISITOR_PRE(ExprBinary);
	ADD_VISITOR_PRE(ExprLogical);
	ADD_VISITOR_PRE(ExprConditional);
	ADD_VISITOR_PRE(ExprAssignment);

	void onVisitPreNode(AstNode* node);
	void onVisitPostNode(AstNode* node);

protected:
	FILE* _fd;
	int _indent;
};


} // namespace ast
} // namespace ringc
} // namespace ring


#endif
