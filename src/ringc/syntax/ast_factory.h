#ifndef RING_RINGC_SYNTAX_AST_FACTORY_H
#define RING_RINGC_SYNTAX_AST_FACTORY_H


namespace ring {
namespace ringc {
namespace ast {
	class AstFactory;
}}}


#include "../session/session.h"
#include "ast.h"


namespace ring {
namespace ringc {
namespace ast {


class AstFactory {
public:
	AstFactory(Session* session);

	Module* createModule();
	Use* createUse(Ident module_name, Ident module_as, Module* module);
	Extern* createExtern(Ident name, TypeId type_id);
	Let* createLet(bool is_pub, bool is_mut, Ident name, TypeId type_id, Expr* expr);
	ExprEmpty* createExprEmpty();
	ExprBlock* createExprBlock();
	ExprFn* createExprFn(TypeId type_id, const vector<Ident>& args, ExprBlock* body);
	ExprIf* createExprIf(Expr* test, Expr* con, Expr* alt);
	ExprIdent* createExprIdent(Ident id);
	ExprLiteralBasic* createExprLiteralBasic(StrId str_id);
	ExprLiteralArray* createExprLiteralArray(const vector<Expr*>& arr);
	ExprMember* createExprMember(Expr* object, Property property);
	ExprCall* createExprCall(Expr* callee, const vector<Expr*>& args);
	ExprUnary* createExprUnary(UnaryOp op, Expr* expr);
	ExprBinary* createExprBinary(BinaryOp op, Expr* left, Expr* right);
	ExprLogical* createExprLogical(LogicalOp op, Expr* left, Expr* right);
	ExprConditional* createExprConditional(Expr* test, Expr* con, Expr* alt);
	ExprAssignment* createExprAssignment(AssignmentOp op, Expr* left, Expr* right);

	TypeId getPrimTypeId(const PrimTypeKey& prim_type_key);
	TypeId getFuncTypeId(const FuncTypeKey& func_type_key);
	TypeId getArrayTypeId(const ArrayTypeKey& array_type_key);

protected:
	AstNode* assignNode(AstNode* node);

protected:
	Session* _session;
};


} // namespace ast
} // namespace ringc
} // namespace ring


#endif
