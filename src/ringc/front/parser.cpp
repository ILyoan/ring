#include "parser.h"
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;
using namespace ring::ringc;

#define EXPECTED(a, b) ERROR("Expected `%s`, buf found `%s`", a, b);

Parser::Parser(FILE* fp, Session* session)
		: _lexer(fp, session)
		, _curr_token(Token::INVALID)
		, _next_token(Token::INVALID)
		, _session(session) {
	eat(); // _next_token to be the first token.
	eat(); // _curr_token to be the first token, _next_token to be the next.
}


Parser::Parser(const string& src, Session* session)
		: _lexer(src, session)
		, _curr_token(Token::INVALID)
		, _next_token(Token::INVALID)
		, _session(session) {
	eat(); // _next_token to be the first token.
	eat(); // _curr_token to be the first token, _next_token to be the next.
}


Module* Parser::parseProgram() {
	ScopeTracer tracer(_session, "parseProgram()");
	return parseModule();
}


Expr* Parser::parseExpression() {
	ScopeTracer tracer(_session, "parseExpression()");
	return parseExpr();
}


Stmt* Parser::parseStatement() {
	ScopeTracer tracer(_session, "parseStatement()");
	return parseStmt(false);
}


// Parse Module.
Module* Parser::parseModule() {
	ScopeTracer tracer(_session, "parseModule()");

	Module* module = ast_fac()->createModule();

	// Parse uses.
	while (!isEof() && isType(Token::KEYWORD_USE)) {
		module->addUse(parseUse());
		eatType(Token::SEMICOLON);
	}
	// Parse externs.
	while (!isEof() && isType(Token::KEYWORD_EXTERN)) {
		module->addExtern(parseExtern());
		eatType(Token::SEMICOLON);
	}
	// Parse let bindings.
	while (!isEof()) {
		Let* let = parseLet(true);
		if (session()->type_table()->isPrimType(let->type_id())) {
			eatType(Token::SEMICOLON);
		}
		module->addDecl(let);
	}

	return module;
}


// TODO: Parse use.
//   use = "pub" ? "use" ident [ "as" ident ] ? ;
Use* Parser::parseUse() {
}


// Parse extern.
//   extern = "extern" ident ':' type ;
Extern* Parser::parseExtern() {
	ScopeTracer tracer(_session, "parseExtern()");

	// eat "extern".
	eatType(Token::KEYWORD_EXTERN);
	// eat "name".
	Ident name = eatIdent();
	// eat semicolon.
	eatType(Token::COLON);
	// parse type
	TypeId type_id= parseType();

	return ast_fac()->createExtern(name, type_id);
}


// Parse statement.
Stmt* Parser::parseStmt(bool is_global) {
	ScopeTracer tracer(_session, "parseStmt()");

	if (curr().type() == Token::KEYWORD_LET) {
		return parseLet(is_global);
	} else {
		return parseExpr();
	}
}


// Parse let binding.
//   let = let_global | let_local ;
//   let_local = "let" "mut" ? ident ':' type '=' expr ;
//   let_global = "pub" ? "let" ident ':' type '=' expr ;
Let* Parser::parseLet(bool is_global) {
	ScopeTracer tracer(_session, "parseLet()");

	// "pub" ?
	bool is_pub = false;
	if (is_global) {
		is_pub = maybeEatType(Token::KEYWORD_PUB);
	}

	// "let"
	if (!eatType(Token::KEYWORD_LET)) {
		return NULL;
	}

	// "mut" ?
	bool is_mut = false;
	if (!is_global) {
		is_mut = maybeEatType(Token::KEYWORD_MUT);
	}

	// Name.
	Ident name = eatIdent();

	// Parse type.
	TypeId type_id;
	if (maybeEatType(Token::COLON)) {
		type_id = parseType();
	}

	// Eat '=' token.
	eatType(Token::ASSIGN);

	// Parse espression
	Expr* expr = parseExpr();

	if (type_id.none()) {
		type_id = expr->type_id();
	}

	// Return let expression.
	return ast_fac()->createLet(is_pub, is_mut, name, type_id, expr);
}


Expr* Parser::parseExpr() {
	ScopeTracer tracer(_session, "parseExpr()");

	Expr* expr = NULL;

	switch (curr().type()) {
		case Token::LBRACE: expr = parseExprBlock(); break;
		case Token::KEYWORD_FN: expr = parseExprFn(); break;
		case Token::KEYWORD_IF: expr = parseExprIf(); break;
		default: expr = parseExprAssignment();
	}

	if (expr == NULL) {
		FATAL("parse expression returns NULL - current token: %d", curr().type());
	}

	return expr;
}


// Parse a block expression.
//   block_expr = '{' expr '}' ;
ExprBlock* Parser::parseExprBlock() {
	ScopeTracer tracer(_session, "parseExprBlock()");

	ExprBlock* block = ast_fac()->createExprBlock();

	// Eat '{' token.
	eatType(Token::LBRACE);

	// Until '}'.
	while (true) {
		if (maybeEatType(Token::SEMICOLON)) {
			continue;
		}
		if (isType(Token::RBRACE)) {
			block->addStatement(ast_fac()->createExprEmpty());
			break;
		}
		// statement
		Stmt* stmt = parseStmt(false);
		block->addStatement(stmt);
		if (isType(Token::RBRACE)) {
			break;
		}
		if (needSemicolon(stmt)) {
			eatType(Token::SEMICOLON);
		}
	}
	eatType(Token::RBRACE);

	return block;
}


// Parse a function definition.
//   fn_expr = fn_prototype block_expr ;
//   fn_prototype = "fn" '(' argument_list ')'
//   argument_list = empty | ( ident | ident ',' argument_list ) ;
ExprFn* Parser::parseExprFn() {
	ScopeTracer tracer(_session, "parseExprFn()");

	// Parse function prototype.
	auto proto = parseFunctionType(true);

	// Parse function bady.
	ExprBlock* body = parseExprBlock();

	// Creates functions definition ast node and returns it.
	return ast_fac()->createExprFn(proto.first, proto.second,	body);
}


// Parser a if expression.
//   if_expr = "if" expr block_expr else_tail ? ;
//   else_tail = "else" ( if_expr | block ) ;
ExprIf* Parser::parseExprIf() {
	ScopeTracer tracer(_session, "parseExprIf()");

	// eat "if" keyword.
	eatType(Token::KEYWORD_IF);

	// parse test expression.
	Expr* test = parseExpr();

	// parse consequence block expression.
	Expr* con = parseExprBlock();

	// parse alternate.
	Expr* alt = NULL;
	if (maybeEatType(Token::KEYWORD_ELSE)) {
		if (isType(Token::KEYWORD_IF)) {
			alt = parseExprIf();
		} else if (isType(Token::LBRACE)) {
			alt = parseExprBlock();
		} else {
			EXPECTED("`if` or `{`", curr().toString().c_str());
		}
	}

	return ast_fac()->createExprIf(test, con, alt);
}


// Parse an identifier expression.
ExprIdent* Parser::parseExprIdent() {
	ScopeTracer tracer(_session, "parseExprIdent()");

	return ast_fac()->createExprIdent(eatIdent());
}


ExprLiteral* Parser::parseExprLiteral() {
	ScopeTracer tracer(_session, "parseExprLiteral()");
	ExprLiteral* expr_lit = NULL;

	if (isType(Token::LBRACKET)) {
		expr_lit = parseExprLiteralArray();
	} else if (TokenUtil::isLiteral(curr().type())) {
		expr_lit = parseExprLiteralBasic();
	} else {
		EXPECTED("literal", curr().toString().c_str());
	}
	ASSERT_NE(expr_lit, NULL, SRCPOS);
	if (maybeEatType(Token::COLON)) {
		expr_lit->type_id(parseType());
	}
	return expr_lit;
}


// NOTE: The type of basic literal SHELL be set in parser.
ExprLiteralBasic* Parser::parseExprLiteralBasic() {
	ScopeTracer tracer(_session, "parseExprLiteralBasic()");
	ASSERT(TokenUtil::isLiteral(curr().type()), SRCPOS);

	Token lit = currAndEat();
	ExprLiteralBasic* expr_lit = ast_fac()->createExprLiteralBasic(lit.str_id());
	switch (lit.type()) {
		case Token::LIT_FALSE:
		case Token::LIT_TRUE:
			expr_lit->type_id(ast_fac()->getPrimTypeId(PRIM_TYPE_BOOL));
			break;
		case Token::LIT_NUMERIC:
			expr_lit->type_id(ast_fac()->getPrimTypeId(PRIM_TYPE_INT));
			break;
	}
	return expr_lit;
}


ExprLiteralArray* Parser::parseExprLiteralArray() {
	ScopeTracer tracer(_session, "parseExprLiteralArray()");
	ASSERT(isType(Token::LBRACKET), SRCPOS);
	eatType(Token::LBRACKET);
	vector<Expr*> arr;
	while (true) {
		arr.push_back(parseExpr());
		if (maybeEatType(Token::RBRACKET)) {
			break;
		}
		if (!eatType(Token::COMMA)) {
			break;
		}
	}
	return ast_fac()->createExprLiteralArray(arr);
}


// Parse a group expression.
//   group = '(' expr ')' ;
Expr* Parser::parseExprGroup() {
	ScopeTracer tracer(_session, "parseExprGroup()");

	eatType(Token::LPAREN);
	Expr* expr = parseExpr();
	eatType(Token::RPAREN);
	return expr;
}


// TODO: Parse a primary expression.
//   primary_expr = "this"
//                | ident
//                | literal
//                | group ;
Expr* Parser::parseExprPrimary() {
	ScopeTracer tracer(_session, "parseExprPrimary()");

	if (isType(Token::KEYWORD_THIS)) {

	} else if (isType(Token::IDENT)) {
		return parseExprIdent();
	} else if (isType(Token::LBRACKET) || TokenUtil::isLiteral(curr().type())) {
		return parseExprLiteral();
	} else if (isType(Token::LPAREN)) {
		return parseExprGroup();
	} else {
		EXPECTED("primary expression", curr().toString().c_str());
	}
}


// TODO: Parse a left hand side expression.
// Original syntax is as following:
//   member_expr = primary_expr
//               | member_expr "[" expr "]"
//               | member_expr "." ident ;
//   arguments = '(' ')' | '(' argument_list ')' ;
//   call_expr = member_expr arguments
//             | call_expr arguments
//             | call_expr "[" expr "]"
//             | call_expr "." ident ;
//   left_hand_side_expr = member_expr | call_expr ;
//
// Eliminating left recursive results:
//   member_R = '[' expr ']' member_R
//            | '.' ident member_R
//            | empty ;
//   member_expr = primary_expr member_R ;
//   arguments = '(' ')' | '(' argument_list ')' ;
//   call_R = arguments call_R
//          | '[' expr ']' call_R
//          | '.' ident call_R
//          | empty ;
//   call_expr = member_expr arguments call_expr_R ;
//   left_hand_side_expr = member_expr | call_expr ;
Expr* Parser::parseExprLeftHandSide() {
	ScopeTracer tracer(_session, "parseExprLeftHandSide()");

	// left_hand_side_expr = member_expr | call_expr ;
	// ->
	// left_hand_side_expr = prmary_expr member_R [ arguments call_R ] ?
	Expr* primary = parseExprPrimary();
	Expr* object = parseExprMemberOrCall(primary);

	return object;
}


// Parse a member expression.
//   call_R = '[' expr ']' call_R
//          | '.' ident call_R
//          | arguments call_R
//          | empty ;
//   member_or_callexpr = primary_expr call_R ;
Expr* Parser::parseExprMemberOrCall(Expr* primary) {
	ScopeTracer tracer(_session, "parseExprMember()");

	Expr* res = primary;
	while (isType(Token::LBRACKET) || isType(Token::DOT) || isType(Token::LPAREN)) {
		if (maybeEatType(Token::LBRACKET)) {
			res = ast_fac()->createExprMember(res, Property(parseExpr()));
			eatType(Token::RBRACKET);
		} else if (maybeEatType(Token::DOT)) {
			res = ast_fac()->createExprMember(res, Property(eatIdent().name_id()));
		} else if (maybeEatType(Token::LPAREN)) {
			res = ast_fac()->createExprCall(res, parseArguments());
			eatType(Token::RPAREN);
		}
	}
	return res;
}


vector<Expr*> Parser::parseArguments() {
	vector<Expr*> res;
	if (isType(Token::RPAREN)) return res;
	do {
		res.push_back(parseExpr());
	} while (maybeEatType(Token::COMMA));
	return res;
}


// Parse an unary expression.
//   unary_expression = ( '+' | '-' | '!' | '~' ) unary_expression
//                    | left_hand_side_expr
Expr* Parser::parseExprUnary() {
	ScopeTracer tracer(_session, "parseExprUnary()");

	if (TokenUtil::isUnaryOp(curr().type())) {
		UnaryOp op = convertUnaryOp(currAndEat().type());
		return ast_fac()->createExprUnary(op, parseExprUnary());
	} else {
		return parseExprLeftHandSide();
	}
}


// Parse an exponential expression.
//   exponential_expr = unary_expression ( "**" exponential_expr ) ? ;
Expr* Parser::parseExprExponential() {
	ScopeTracer tracer(_session, "parseExprExponential()");

	Expr* expr = parseExprUnary();
	while (maybeEatType(Token::EXP)) {
		expr = ast_fac()->createExprBinary(BO_EXP, expr, parseExprUnary());
	}
	return expr;
}


// Parse a multiplicative expression.
//   multiplicative_expr = ( multiplicative_expr ( '*' | '/' | '&' ) ) ? exponential_expr ;
Expr* Parser::parseExprMultiplicative() {
	ScopeTracer tracer(_session, "parseExprMultiplicative()");

	Expr* expr = parseExprExponential();
	while (isType(Token::MUL) || isType(Token::DIV) || isType(Token::MOD)) {
		BinaryOp op = convertBinaryOp(currAndEat().type());
		expr = ast_fac()->createExprBinary(op, expr, parseExprExponential());
	}
	return expr;
}


// Parse an additive expression.
//   additive_expr = ( additive_expr ( '+' | '-' ) ) ? multiplicative_expr ;
Expr* Parser::parseExprAdditive() {
	ScopeTracer tracer(_session, "parseExprAdditive()");

	Expr* expr = parseExprMultiplicative();
	while (isType(Token::ADD) || isType(Token::SUB)) {
		BinaryOp op = convertBinaryOp(currAndEat().type());
		expr = ast_fac()->createExprBinary(op, expr, parseExprMultiplicative());
	}
	return expr;
}


// Parse a shift expression.
//   shift_expr = ( shift_expr ( "<<" | ">>" ) ) ? additive_expr ;
Expr* Parser::parseExprShift() {
	ScopeTracer tracer(_session, "parseExprShift()");

	Expr * expr = parseExprAdditive();
	while (isType(Token::LSH) || isType(Token::RSH)) {
		BinaryOp op = convertBinaryOp(currAndEat().type());
		expr = ast_fac()->createExprBinary(op, expr, parseExprAdditive());
	}
	return expr;
}


// Parse a relational expression.
//   relational_expr = ( relational_expr ( '<' | "<=" | ">=" | '>' ) ) ? shift_expr ;
Expr* Parser::parseExprRelational() {
	ScopeTracer tracer(_session, "parseExprRelational()");

	Expr* expr = parseExprShift();
	while (isType(Token::LT) || isType(Token::LE) || isType(Token::GE) || isType(Token::GT)) {
		BinaryOp op = convertBinaryOp(currAndEat().type());
		expr = ast_fac()->createExprBinary(op, expr, parseExprShift());
	}
	return expr;
}

// Parse an equality expression.
//   equality_expr = ( equality_expr ( "==" | "!=" ) ) ? relational_expr ;
Expr* Parser::parseExprEquality() {
	ScopeTracer tracer(_session, "parseExprEquality()");

	Expr* expr = parseExprRelational();
	if (isType(Token::EQ) || isType(Token::NE)) {
		BinaryOp op = convertBinaryOp(currAndEat().type());
		expr = ast_fac()->createExprBinary(op, expr, parseExprRelational());
	}
	return expr;
}


// Parse a bitwise and expression.
//   bitwise_and_expr = ( bitwise_and_expr '&' ) ? equality_expr ;
Expr* Parser::parseExprBitwiseAnd() {
	ScopeTracer tracer(_session, "parseExprBitwiseAnd()");

	Expr* expr = parseExprEquality();
	while (maybeEatType(Token::AND)) {
		expr = ast_fac()->createExprBinary(BO_BITWISE_AND, expr, parseExprEquality());
	}
	return expr;
}


// Parse a bitwise xor expression.
//   bitwise_xor_expr = ( bitwise_xor_expr '^' ) ? bitwise_and_expr ;
Expr* Parser::parseExprBitwiseXor() {
	ScopeTracer tracer(_session, "parseExprBitwiseOr()");

	Expr* expr = parseExprBitwiseAnd();
	while (maybeEatType(Token::HAT)) {
		expr = ast_fac()->createExprBinary(BO_BITWISE_XOR, expr, parseExprBitwiseAnd());
	}
	return expr;
}


// Parse a bitwise or expression.
//   bitwise_or_expr = ( bitwise_or_expr '|' ) ? bitwise_xor_expr ;
Expr* Parser::parseExprBitwiseOr() {
	ScopeTracer tracer(_session, "parseExprBitwiseOr()");

	Expr* expr = parseExprBitwiseXor();
	while (maybeEatType(Token::OR)) {
		expr = ast_fac()->createExprBinary(BO_BITWISE_OR, expr, parseExprBitwiseXor());
	}
	return expr;
}


// Parse a logical and expression
//   logical_and_expr = ( logical_and_expr "&&" ) ? bitwise_or_expr ;
Expr* Parser::parseExprLogicalAnd() {
	ScopeTracer tracer(_session, "parseExprLogicalAnd()");

	Expr* expr = parseExprBitwiseOr();
	while (maybeEatType(Token::ANDAND)) {
		expr = ast_fac()->createExprLogical(LO_AND, expr, parseExprBitwiseOr());
	}
	return expr;
}


// Parse a logical or expression
//   logical_or_expr = ( logical_or_expr "||" ) ? logical_and_expr ;
Expr* Parser::parseExprLogicalOr() {
	ScopeTracer tracer(_session, "parseExprLogicalOR()");

	Expr* expr = parseExprLogicalAnd();
	while (maybeEatType(Token::OROR)) {
		expr = ast_fac()->createExprLogical(LO_OR, expr, parseExprLogicalAnd());
	}
	return expr;
}


// Parse a conditional expression
//   conditional_expr = logical_or_expr ( '?' expr ':' expr ) ?
Expr* Parser::parseExprConditional() {
	ScopeTracer tracer(_session, "parseExprConditional()");

	Expr* expr = parseExprLogicalOr();
	if (maybeEatType(Token::HOOK)) {
		Expr* con = parseExprAssignment();
		eatType(Token::COLON);
		Expr* alt = parseExprAssignment();
		return ast_fac()->createExprConditional(expr, con, alt);
	}
	return expr;
}


// Parse an assignment expression
//   assignment_expr = conditional_expr ( '=' expr ) ? ;
Expr* Parser::parseExprAssignment() {
	ScopeTracer tracer(_session, "parseExprAssignment()");

	Expr* expr = parseExprConditional();
	if (TokenUtil::isAssigmentOp(curr().type())) {
		AssignmentOp op = convertAssignmentOp(currAndEat().type());
		return ast_fac()->createExprAssignment(op, expr, parseExpr());
	}
	return expr;
}


// Parse a type.
// TODO: Complete parseType().
TypeId Parser::parseType() {
	ScopeTracer tracer(_session, "parseType()");

	if (isType(Token::KEYWORD_FN)) {
		return parseFunctionType(false).first;
	} else if (isType(Token::LBRACKET)) {
		return parseArrayType();
	} else {
		if (maybeEatType(Token::KEYWORD_TY_INT)) {
			return ast_fac()->getPrimTypeId(PRIM_TYPE_INT);
		} else if (maybeEatType(Token::KEYWORD_TY_BOOL)) {
			return ast_fac()->getPrimTypeId(PRIM_TYPE_BOOL);
		} else if (maybeEatType(Token::KEYWORD_TY_NIL)) {
			return ast_fac()->getPrimTypeId(PRIM_TYPE_NIL);
		} else {
			EXPECTED("type", curr().toString().c_str());
		}
	}
}


// Parse function type
//   "fn" '(' type_list ')' [ "->" type ]
pair<TypeId, vector<Ident> > Parser::parseFunctionType(bool parse_arg_names) {
	ScopeTracer tracer(_session, "parseFunctionType()");

	vector<TypeId> arg_types;
	vector<Ident> arg_names;

	// function type should start with "fn".
	if (!eatType(Token::KEYWORD_FN)) {
		return make_pair(TypeId(), arg_names);
	}

	// Open paranthesis is expected, eat it.
	if (!eatType(Token::LPAREN)) {
		return make_pair(TypeId(), arg_names);
	}

	// Parse argument list.
	if (!isType(Token::RPAREN)) {
		do {
			if (parse_arg_names) {
				arg_names.push_back(eatIdent());
				eatType(Token::COLON);
			}
			arg_types.push_back(parseType());
		} while (maybeEatType(Token::COMMA));
	}

	// Close paranthesis is expected, eat it.
	if (!eatType(Token::RPAREN)) {
		return make_pair(TypeId(), arg_names);
	}

	// Maybe right allow followed by return type.
	// If return type is `nil`. it can be omitted.
	if (maybeEatType(Token::RARROW)) {
		return make_pair(
				ast_fac()->getFuncTypeId(make_pair(arg_types, parseType())),
				arg_names);
	} else {
		return make_pair(
				ast_fac()->getFuncTypeId(
					make_pair(arg_types, ast_fac()->getPrimTypeId(PRIM_TYPE_NIL))),
				arg_names);
	}
}


TypeId Parser::parseArrayType() {
	ScopeTracer tracer(_session, "parseArrayType()");

	if (!eatType(Token::LBRACKET)) {
		return TypeId();
	}
	if (!eatType(Token::RBRACKET)) {
		return TypeId();
	}
	TypeId type_id = parseType();
	return ast_fac()->getArrayTypeId(type_id);
}


// Returns true if given `stmt` needs semicolon at the end.
bool Parser::needSemicolon(Stmt* stmt) {
	switch (stmt->node_type()) {
		case AST_EXPR_BLOCK:
		case AST_EXPR_IF:
			return false;
	}
	return true;
}


StrId Parser::strId(const string& str) {
	if (!_session->str_table()->contains(str)) {
		FATAL("No such string in the string table: %s", str.c_str());
	}
	return _session->str_table()->id(str);
}


bool Parser::isEof() const {
	return isType(Token::EoF);
}


Token Parser::curr() const {
	return _curr_token;
}


Token Parser::next() const {
	return _next_token;
}


bool Parser::isToken(const Token& token) const {
	return curr() == token;
}


bool Parser::isType(Token::TokenType type) const {
	return curr().type() == type;
}


void Parser::eat() {
	_curr_token = _next_token;
	_next_token = _lexer.nextToken();
}


Token Parser::currAndEat() {
	Token res = _curr_token;
	eat();
	return res;
}


bool Parser::eatToken(const Token& expected) {
	if (isToken(expected)) {
		eat();
		return true;
	} else {
		EXPECTED(expected.toString().c_str(), curr().toString().c_str());
		return false;
	}
}


bool Parser::eatType(Token::TokenType expected) {
	if (isType(expected)) {
		eat();
		return true;
	} else {
		EXPECTED(Token(expected).toString().c_str(), curr().toString().c_str());
		return false;
	}
}


bool Parser::maybeEatType(Token::TokenType expected) {
	if (isType(expected)) {
		eat();
		return true;
	} else {
		return false;
	}
}


Ident Parser::eatIdent() {
	if (isType(Token::IDENT)) {
		return Ident(currAndEat().str_id());
	} else {
		EXPECTED(Token(Token::IDENT).toString().c_str(), curr().toString().c_str());
	}
}


AstFactory* Parser::ast_fac() {
	return session()->ast_fac();
}


AstStringify* Parser::ast_str() {
	return session()->ast_str();
}
