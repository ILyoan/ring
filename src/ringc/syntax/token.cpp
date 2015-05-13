#include "token.h"
using namespace ring::ringc;


static string KEYWORDS[] = {
	"else",
	"extern",
	"fn",
	"if",
	"let",
	"mut",
	"pub",
	"then",
	"this",
	"use",
	// builtin types
	"int",
	"nil",
};


Token::Token(TokenType type)
		: _type(type) {
}


Token::Token(TokenType type, StrId str_id)
		: _type(type), _str_id(str_id) {
}


bool Token::operator==(const Token& other) const {
	return type() == other.type() && str_id() == other.str_id();
}


Token::TokenType Token::type() const {
	return _type;
}


StrId Token::str_id() const {
	return _str_id;
}


bool Token::isValid() const {
	return _type != INVALID;
}


bool Token::isEof() const {
	return _type == EoF;
}


string Token::toString() const {
	switch (type()) {
		case INVALID: return "INVALID";

		case ASSIGN: return "=";
		case ASSIGN_ADD: return "+=";
		case ASSIGN_SUB: return "-=";
		case ASSIGN_MUL: return "*=";
		case ASSIGN_DIV: return "/=";
		case ASSIGN_MOD: return "%=";
		case ASSIGN_LSH: return "<<=";
		case ASSIGN_RSH: return ">>=";
		case ASSIGN_BITWISE_OR: return "|=";
		case ASSIGN_BITWISE_XOR: return "&=";
		case ASSIGN_BITWISE_AND: return "^=";

		case ADD: return "+";
		case SUB: return "-";
		case MUL: return "*";
		case DIV: return "/";
		case MOD: return "%";
		case EXP: return "**";
		case LSH: return "<<";
		case RSH: return ">>";
		case OR: return "|";
		case HAT: return "^";
		case AND: return "&";
		case EQ: return "==";
		case NE: return "!=";
		case LT: return "<";
		case LE: return "<=";
		case GE: return ">=";
		case GT: return ">";

		case OROR: return "||";
		case ANDAND: return "&&";

		case NOT: return "!";

		case TILDE: return "~";

		case SEMICOLON: return ";";
		case COMMA: return ",";
		case HOOK: return "?";
		case COLON: return ":";
		case DOT: return ".";
		case DQUOTE: return "\"";
		case LBRACKET: return "[";
		case RBRACKET: return "]";
		case LBRACE: return "{";
		case RBRACE: return "}";
		case LPAREN: return "(";
		case RPAREN: return ")";
		case LARROW: return "<-";
		case RARROW: return "->";

		case LIT_FALSE: return "false";
		case LIT_TRUE: return "true";
		case LIT_NUMERIC: {
				char buff[255] = {0};
				sprintf(buff, "%d", _str_id.value());
				return buff;
			}
		case LIT_CHAR: {
				char buff[255] = {0};
				sprintf(buff, "'%d'", _str_id.value());
				return buff;
			}
		case LIT_STRING: {
				char buff[255] = {0};
				sprintf(buff, "\"%d\"", _str_id.value());
				return buff;
			}
		case IDENT: {
				char buff[255] = {0};
				sprintf(buff, "id(%d)", _str_id.value());
				return buff;
			}
		case KEYWORD_ELSE: return "else";
		case KEYWORD_EXTERN: return "extern";
		case KEYWORD_FN: return "fn";
		case KEYWORD_IF: return "if";
		case KEYWORD_LET: return "let";
		case KEYWORD_MUT: return "mut";
		case KEYWORD_PUB: return "pub";
		case KEYWORD_THEN: return "then";
		case KEYWORD_THIS: return "this";
		case KEYWORD_USE: return "use";
		case KEYWORD_TY_INT: return "int";
		case KEYWORD_TY_NIL: return "nil";
		case EoF: return "EOF";
		default: return "UNKNOWN token";
	}
}


Token TokenUtil::maybeKeyword(const string& token_str) {
	int first_keyword = Token::KEYWORD_ELSE;
	int last_keyword = Token::KEYWORD_TY_NIL;

	for (int t = first_keyword; t <= last_keyword; ++t) {
		int ix = t - first_keyword;
		if (KEYWORDS[ix] == token_str) {
			return Token((Token::TokenType)t);
		}
	}
	return Token(Token::INVALID);
}


Token TokenUtil::maybeLitBool(const string& str) {
	if (str == "true") {
		return Token(Token::LIT_TRUE);
	} else if (str == "false") {
		return Token(Token::LIT_FALSE);
	}
	return Token(Token::INVALID);
}


Token TokenUtil::reservedOrIdent(const string& str, StrId str_id) {
	Token res = maybeLitBool(str);
	if (res.isValid()) {
		return res;
	}
	res = maybeKeyword(str);
	if (res.isValid()) {
		return res;
	}
	return Token(Token::IDENT, str_id);
}


bool TokenUtil::isAssigmentOp(Token::TokenType token_type) {
	int first_assign = Token::ASSIGN;
	int last_assign = Token::ASSIGN_BITWISE_AND;
	return first_assign <= token_type && token_type <= last_assign;
}


bool TokenUtil::isLogicalOp(Token::TokenType token_type) {
	return token_type == Token::OROR || token_type == Token::ANDAND;
}


bool TokenUtil::isBinaryOp(Token::TokenType token_type) {
	int first_binary_op = Token::ADD;
	int last_binary_op = Token::GT;
	return first_binary_op <= token_type && token_type <= last_binary_op;
}


bool TokenUtil::isUnaryOp(Token::TokenType token_type) {
	switch (token_type) {
		case Token::ADD:
		case Token::SUB:
		case Token::NOT:
		case Token::TILDE:
			return true;
	}
	return false;
}


bool TokenUtil::isPrimary(Token::TokenType token_type) {
	switch (token_type) {
		case Token::LIT_FALSE:
		case Token::LIT_TRUE:
		case Token::LIT_NUMERIC:
		case Token::LIT_STRING:
		case Token::IDENT:
		case Token::KEYWORD_THIS:
			return true;
	}
	return false;
}


bool TokenUtil::isLiteral(Token::TokenType token_type) {
	int first_literal = Token::LIT_FALSE;
	int last_literal = Token::LIT_STRING;
	return first_literal <= token_type && token_type <= last_literal;
}
