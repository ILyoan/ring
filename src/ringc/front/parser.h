#ifndef RING_RINGC_FRONT_PARSER_H
#define RING_RINGC_FRONT_PARSER_H


#include <string>
#include "../session/session.h"
#include "../syntax/ast.h"
#include "../syntax/ast_factory.h"
#include "../syntax/ast_stringify.h"
#include "lexer.h"
using namespace std;
using namespace ring::ringc::ast;


namespace ring {
namespace ringc {


class Parser {
	ADD_PROPERTY_P(session, Session)

public:
	Parser(FILE* fp, Session* session);
	Parser(const string& src, Session* session);

	// Parse a program under the assumption that the given source is a program.
	Module* parseProgram();

	// Parse an expression under the assumption that the given source is an expression.
	Expr* parseExpression();

	// Parse an statement under the assumption that the given source is an statement.
	Stmt* parseStatement();

protected:
	Module* parseModule();
	Use* parseUse();
	Extern* parseExtern();

	Stmt* parseStmt(bool is_global);
	Let* parseLet(bool is_global);
	Expr* parseExpr();
	ExprBlock* parseExprBlock();
	ExprFn* parseExprFn();
	ExprIf* parseExprIf();
	ExprIdent* parseExprIdent();
	ExprLiteral* parseExprLiteral();
	Expr* parseExprGroup();
	Expr* parseExprPrimary();
	Expr* parseExprLeftHandSide();
	Expr* parseExprMemberOrCall(Expr* primary);
	Expr* parseExprExponential();
	Expr* parseExprUnary();
	Expr* parseExprMultiplicative();
	Expr* parseExprAdditive();
	Expr* parseExprShift();
	Expr* parseExprRelational();
	Expr* parseExprEquality();
	Expr* parseExprBitwiseAnd();
	Expr* parseExprBitwiseXor();
	Expr* parseExprBitwiseOr();
	Expr* parseExprLogicalAnd();
	Expr* parseExprLogicalOr();
	Expr* parseExprConditional();
	Expr* parseExprAssignment();

	TypeId parseType();
	pair<TypeId, vector<Ident> > parseFunctionType(bool parse_arg_names);
	vector<Expr*> parseArguments();


	bool needSemicolon(Stmt* stmt);

	StrId strId(const string& str);

	bool isEof() const;
	Token curr() const;
	Token next() const;
	bool isToken(const Token& token) const;
	bool isType(Token::TokenType type) const;

	void eat();
	Token currAndEat();
	bool eatToken(const Token& token);
	bool eatType(Token::TokenType type);
	bool maybeEatType(Token::TokenType type);
	Ident eatIdent();

	void log(LogLevel log_level, const string& msg) const;

protected:
	Lexer _lexer;

	Token _curr_token;
	Token _next_token;

	AstFactory* ast_fac();
	AstStringify* ast_str();
};


} // namespace ringc
} // namespace ring


#endif
