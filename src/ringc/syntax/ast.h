#ifndef RING_RINGC_SYNTAX_AST_H
#define RING_RINGC_SYNTAX_AST_H


#include <string>
#include <vector>
#include "../../common.h"
#include "token.h"
using namespace std;


namespace ring {
namespace ringc {
namespace ast {


class Program;
class Module;
class Use;
class Extern;
class Stmt;
class Let;
class Expr;
class ExprEmpty;
class ExprAssign;
class ExprBinary;
class ExprIf;
class ExprBlock;
class Ident;
class BiOp;

class Type;
class PrimitiveType;
class FunctionType;

class AstFactory;


enum AstNodeType {
	AST_MODULE,
	AST_USE,
	AST_EXTERN,
	AST_LET,
	AST_EXPR_EMPTY,
	AST_EXPR_BLOCK,
	AST_EXPR_FN,
	AST_EXPR_IF,
	AST_EXPR_IDENT,
	AST_EXPR_LITERAL,
	AST_EXPR_MEMBER,
	AST_EXPR_CALL,
	AST_EXPR_UNARY,
	AST_EXPR_BINARY,
	AST_EXPR_LOGICAL,
	AST_EXPR_CONDITIONAL,
	AST_EXPR_ASSIGNMENT,
};


enum AssignmentOp {
	AO_ASSIGN,
	AO_ADD,
	AO_SUB,
	AO_MUL,
	AO_DIV,
	AO_MOD,
	AO_LSH,
	AO_RSH,
	AO_BITWISE_OR,
	AO_BITWISE_XOR,
	AO_BITWISE_AND,
};


enum LogicalOp {
	LO_OR,
	LO_AND,
};


enum BinaryOp {
	BO_ADD,
	BO_SUB,
	BO_MUL,
	BO_DIV,
	BO_MOD,
	BO_EXP,
	BO_BITWISE_OR,
	BO_BITWISE_XOR,
	BO_BITWISE_AND,
	BO_LSH,
	BO_RSH,
	BO_EQ,
	BO_NE,
	BO_LT,
	BO_LE,
	BO_GE,
	BO_GT,
};


enum UnaryOp {
	UO_PLUS,
	UO_MINUS,
	UO_NOT,
	UO_BITWISE_NOT,
};


enum LiteralType {
	LIT_FALSE,
	LIT_TRUE,
	LIT_CHAR,
	LIT_STRING,
	LIT_NUM,
};


enum TypeType {
	TYPE_PRIMITIVE,
	TYPE_FUNCTION,
};


enum PrimitiveTypeType {
	PRIM_TYPE_NIL,
	PRIM_TYPE_INT,
	PRIM_TYPE_BOOL,
};


enum PropertyType {
	PROP_INDEX,
	PROP_IDENT,
};


typedef pair<Ident, Type*> NameTypePair;

AssignmentOp convertAssignmentOp(Token::TokenType token_type);
LogicalOp convertLogicalOp(Token::TokenType token_type);
BinaryOp convertBinaryOp(Token::TokenType token_type);
UnaryOp convertUnaryOp(Token::TokenType token_type);
LiteralType convertLiteralType(Token::TokenType token_type);


// Identifier.
class Ident {
public:
	Ident(NameId name_id);
	Ident(NameId name_id, SymbolId symbol_id);

	ADD_PROPERTY(name_id, NameId);
	ADD_PROPERTY(symbol_id, SymbolId)
};


// Type of data
class Type {
	ADD_PROPERTY(type_id, TypeId)
	ADD_PROPERTY(type, TypeType)

public:
	Type(TypeType type);

	bool isPrimitiveType() const;
	bool isFunctionType() const;

	bool isNil() const;
};


// Primiteve type
class PrimitiveType : public Type {
	ADD_PROPERTY(primitive_type, PrimitiveTypeType)

public:
	PrimitiveType(PrimitiveTypeType primitive_type);

	bool isNil() const;
};


// Function type
//   function_type = '(' type_list  ')' "->" type
class FunctionType : public Type {
	ADD_PROPERTY_R(args, vector<TypeId>)
	ADD_PROPERTY(ret, TypeId)

public:
	FunctionType(const vector<TypeId>& args, TypeId ret);
};


class Property {
	ADD_PROPERTY(property_type, PropertyType)
	ADD_PROPERTY_P(index, Expr)
	ADD_PROPERTY(ident, StrId)

public:
	Property(Expr* index);
	Property(StrId ident);

	bool isIndex() const;
	bool isIdent() const;
};


// Base class for all AST nodes.
class AstNode {
	ADD_PROPERTY_P(parent, AstNode)
	ADD_PROPERTY(node_id, NodeId)
	ADD_PROPERTY(node_type, AstNodeType)

public:
	bool isModule() const;
	bool isUse() const;
	bool isExtern() const;
	bool isStmt() const;
	bool isLet() const;
	bool isExpr() const;
	bool isExprFn() const;

protected:
	AstNode(AstNodeType node_type);
};


// AST node for module.
// A module consists of child modules and declarations.
class Module : public AstNode {
	ADD_PROPERTY_R(uses, vector<Use*>)
	ADD_PROPERTY_R(exts, vector<Extern*>)
	ADD_PROPERTY_R(decls, vector<Let*>)
	friend class AstFactory;

public:
	void addUse(Use* use);
	void addExtern(Extern* ext);
	void addDecl(Let* decl);

protected:
	Module();
};


// AST node for use.
//   use = "use" ident ( "as" ident ) ? ;
class Use : public AstNode {
	ADD_PROPERTY(module_name, Ident)
	ADD_PROPERTY(module_as, Ident)
	ADD_PROPERTY_P(module, Module)
	friend class AstFactory;

protected:
	Use(Ident module_name, Ident module_as, Module* module);
};


// AST node for extern
//   extern = "extern" ident ':' type
class Extern : public AstNode {
	ADD_PROPERTY(name, Ident)
	ADD_PROPERTY(type_id, TypeId)
	friend class AstFactory;

protected:
	Extern(Ident name, TypeId type_id);
};


// AST node for statement.
// A statement is one of:
//   - let binding
//   - expression
class Stmt : public AstNode {
protected:
	Stmt(AstNodeType node_type);
};


// AST node for let binding.
//   let = let_global | let_local ;
//   let_local = "let" "mut" ? ident ':' type '=' expr ;
//   let_global = "pub" ? "let" ident ':' type '=' expr ;
class Let : public Stmt {
	ADD_PROPERTY(is_pub, bool)
	ADD_PROPERTY(is_mut, bool)
	ADD_PROPERTY(name, Ident)
	ADD_PROPERTY(type_id, TypeId)
	ADD_PROPERTY_P(expr, Expr)
	friend class AstFactory;

public:
	bool inModuleScope() const;

protected:
	Let(bool is_pub, bool is_mut, Ident name, TypeId type_id, Expr* expr);
};


// AST node for expression.
class Expr : public Stmt {
	ADD_PROPERTY(type_id, TypeId)
protected:
	Expr(AstNodeType node_type);
};


class ExprEmpty : public Expr {
	friend class AstFactory;

protected:
	ExprEmpty();
};


// AST node for block expression.
class ExprBlock : public Expr {
	ADD_PROPERTY_R(stmts, vector<Stmt*>)
	friend class AstFactory;

public:
	void addStatement(Stmt* stmt);

protected:
	ExprBlock();
};


// AST node for function expression.
//   fn_expr = "fn" '(' argument_list ')' "->" return_type block_expr ;
//   argument_list = empty | ( ident ':' type | ident ':' type ',' argument_list ) ;
//   return_type = type ;
class ExprFn : public Expr {
	ADD_PROPERTY_R(args, vector<Ident>)
	ADD_PROPERTY_P(body, ExprBlock)
	friend class AstFactory;

protected:
	ExprFn(TypeId type_id, const vector<Ident>& args, ExprBlock* body);
};


// AST node for if expression.
class ExprIf : public Expr {
	ADD_PROPERTY_P(test, Expr)
	ADD_PROPERTY_P(con, Expr)
	ADD_PROPERTY_P(alt, Expr)
	friend class AstFactory;

protected:
	ExprIf(Expr* test, Expr* con, Expr* alt);
};


// AST node for ident.
class ExprIdent : public Expr {
	ADD_PROPERTY(id, Ident)
	friend class AstFactory;

protected:
	ExprIdent(Ident id);
};


// AST node for literal expression.
class ExprLiteral : public Expr {
	ADD_PROPERTY(lit_type, LiteralType)
	ADD_PROPERTY(str_id, StrId)
	friend class AstFactory;

protected:
	ExprLiteral(LiteralType lit_type, StrId str_id);
};


// AST node for left hand side expression.
//   left_hand_side_expr = member_expr | call_expr ;

// AST node for memer expression.
//   member_expr = primary_expr
//               | member_expr "[" expr "]"
//               | member_expr "." ident ;
class ExprMember : public Expr {
	ADD_PROPERTY_P(object, Expr)
	ADD_PROPERTY(property, Property)
	friend class AstFactory;

protected:
	ExprMember(Expr* object, Property property);
};


// AST node for call expression.
//   call_expr = expr arguments
//   arguments = '(' ')' | '(' argument_list ')' ;
class ExprCall : public Expr {
	ADD_PROPERTY_P(callee, Expr)
	ADD_PROPERTY_R(args, vector<Expr*>)
	friend class AstFactory;

protected:
	ExprCall(Expr* callee, const vector<Expr*>& args);
};


// AST node for unary expression.
//   unary_expression = left_hand_side_expr
//                    | ( '+' | '-' | '!' | '~' ) unary_expression ;
class ExprUnary : public Expr {
	ADD_PROPERTY(op, UnaryOp)
	ADD_PROPERTY_P(expr, Expr)
	friend class AstFactory;

protected:
	ExprUnary(UnaryOp op, Expr* expr);
};


// AST node for binary expression.
//   exponential_expr = unary_expression ( "**" exponential_expr ) ? ;
//   multiplicative_expr = ( multiplicative_expr ( '*' | '/' | '&' ) ) ? exponential_expr ;
//   additive_expr = ( additive_expr ( '+' | '-' ) ) ? multiplicative_expr ;
//   shift_expr = ( shift_expr ( "<<" | ">>" ) ) ? additive_expr ;
//   relational_expr = ( relational_expr ( '<' | "<=" | ">=" | '>' ) ) ? shift_expr ;
//   equality_expr = ( equality_expr ( "==" | "!=" ) ) ? relational_expr ;
//   bitwise_and_expr = ( bitwise_and_expr '&' ) ? equality_expr ;
//   bitwise_xor_expr = ( bitwise_xor_expr '^' ) ? bitwise_and_expr ;
//   bitwise_or_expr = ( bitwise_or_expr '|' ) ? bitwise_xor_expr ;
class ExprBinary : public Expr {
	ADD_PROPERTY(op, BinaryOp)
	ADD_PROPERTY_P(left, Expr)
	ADD_PROPERTY_P(right, Expr)
	friend class AstFactory;

protected:
	ExprBinary(BinaryOp op, Expr* left, Expr* right);
};


// AST node for logical expression.
//   logical_and_expr = ( logical_and_expr "&&" ) ? bitwise_or_expr ;
//   logical_or_expr = ( logical_or_expr "||" ) ? logical_and_expr ;
//   left op right
class ExprLogical : public Expr {
	ADD_PROPERTY(op, LogicalOp)
	ADD_PROPERTY_P(left, Expr)
	ADD_PROPERTY_P(right, Expr)
	friend class AstFactory;

protected:
	ExprLogical(LogicalOp op, Expr* left, Expr* right);
};


// AST node for conditional expression.
//   conditional_expr = logical_or_expr ( '?' expr ':' expr ) ?
//   test ? con : alt
class ExprConditional : public Expr {
	ADD_PROPERTY_P(test, Expr)
	ADD_PROPERTY_P(con, Expr)
	ADD_PROPERTY_P(alt, Expr)
	friend class AstFactory;

protected:
	ExprConditional(Expr* test, Expr* con, Expr* alt);
};


// AST node for assignment expression.
//   assignment_expr = conditional_expr ( '=' expr ) ? ;
//   left op right
class ExprAssignment : public Expr {
	ADD_PROPERTY(op, AssignmentOp)
	ADD_PROPERTY_P(left, Expr)
	ADD_PROPERTY_P(right, Expr)
	friend class AstFactory;

protected:
	ExprAssignment(AssignmentOp op, Expr* left, Expr* right);
};


#define EXPR_HANDLER_DECL_RET(FN, RET) \
	RET FN(Expr* expr);\
	RET FN(ExprEmpty* empty);\
	RET FN(ExprBlock* block);\
	RET FN(ExprFn* fn);\
	RET FN(ExprIf* if_);\
	RET FN(ExprIdent* ident);\
	RET FN(ExprLiteral* literal);\
	RET FN(ExprMember* member);\
	RET FN(ExprCall* call);\
	RET FN(ExprUnary* unary);\
	RET FN(ExprBinary* binary);\
	RET FN(ExprLogical* logical);\
	RET FN(ExprConditional* cond);\
	RET FN(ExprAssignment* assign);\

#define AST_HANDLER_DECL_RET(FN, RET)\
	RET FN(Module* module);\
	RET FN(Use* use);\
	RET FN(Extern* ext);\
	RET FN(Stmt* stmt);\
	RET FN(Let* let);\
	EXPR_HANDLER_DECL_RET(FN, RET)


#define EXPR_HANDLER_DECL(FN)\
	Expr* FN(Expr* expr);\
	ExprEmpty* FN(ExprEmpty* empty);\
	ExprBlock* FN(ExprBlock* block);\
	ExprFn* FN(ExprFn* fn);\
	ExprIf* FN(ExprIf* if_);\
	ExprIdent* FN(ExprIdent* ident);\
	ExprLiteral* FN(ExprLiteral* literal);\
	ExprMember* FN(ExprMember* member);\
	ExprCall* FN(ExprCall* call);\
	ExprUnary* FN(ExprUnary* unary);\
	ExprBinary* FN(ExprBinary* binary);\
	ExprLogical* FN(ExprLogical* logical);\
	ExprConditional* FN(ExprConditional* cond);\
	ExprAssignment* FN(ExprAssignment* assign);\


#define AST_HANDLER_DECL(FN)\
	AstNode* FN(AstNode* node);\
	Module* FN(Module* module);\
	Use* FN(Use* use);\
	Extern * FN(Extern* ext);\
	Stmt* FN(Stmt* stmt);\
	Let* FN(Let* let);\
	EXPR_HANDLER_DECL(FN)


#define AST_DISPATCH(FN, NODE)\
	do {\
		if (NODE->isModule()) {\
			return FN(static_cast<Module*>(NODE));\
		} else if (node->isUse()) {\
			return FN(static_cast<Use*>(NODE));\
		} else if (node->isStmt()) {\
			return FN(static_cast<Stmt*>(NODE));\
		}\
	} while (0);


#define MODULE_DISPATCH(FN, MODULE)\
	do {\
		for_each(MODULE->uses().begin(), MODULE->uses().end(),\
			[this](Use* use) {\
				FN(use);\
			});\
		for_each(MODULE->exts().begin(), MODULE->exts().end(),\
			[this](Extern* ext) {\
				FN(ext);\
			});\
		for_each(MODULE->decls().begin(), MODULE->decls().end(),\
			[this](Let* let) {\
				FN(let);\
			});\
	} while (0);


#define STMT_DISPATCH(FN, STMT)\
	do {\
		if (STMT->isExpr()) {\
			return FN(static_cast<Expr*>(STMT));\
		} else if (STMT->isLet()) {\
			return FN(static_cast<Let*>(STMT));\
		}\
	} while (0);


#define EXPR_DISPATCH(FN, EXPR)\
	do {\
		switch (EXPR->node_type()) {\
			case AST_EXPR_EMPTY: return FN(static_cast<ExprEmpty*>(EXPR));\
			case AST_EXPR_BLOCK: return FN(static_cast<ExprBlock*>(EXPR));\
			case AST_EXPR_FN: return FN(static_cast<ExprFn*>(EXPR));\
			case AST_EXPR_IF: return FN(static_cast<ExprIf*>(EXPR));\
			case AST_EXPR_IDENT: return FN(static_cast<ExprIdent*>(EXPR));\
			case AST_EXPR_LITERAL: return FN(static_cast<ExprLiteral*>(EXPR));\
			case AST_EXPR_MEMBER: return FN(static_cast<ExprMember*>(EXPR));\
			case AST_EXPR_CALL: return FN(static_cast<ExprCall*>(EXPR));\
			case AST_EXPR_UNARY: return FN(static_cast<ExprUnary*>(EXPR));\
			case AST_EXPR_BINARY: return FN(static_cast<ExprBinary*>(EXPR));\
			case AST_EXPR_LOGICAL: return FN(static_cast<ExprLogical*>(EXPR));\
			case AST_EXPR_CONDITIONAL: return FN(static_cast<ExprConditional*>(EXPR));\
			case AST_EXPR_ASSIGNMENT: return FN(static_cast<ExprAssignment*>(EXPR));\
		}\
	} while (0);

} // namespace ast
} // namespace ringc
} // namespace ring


#endif
