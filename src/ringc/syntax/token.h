#ifndef RING_SYNTEX_TOKEN_H
#define RING_SYNTEX_TOKEN_H


#include <string>
#include "../../common.h"
using namespace std;
using namespace ring::ringc;


namespace ring {
namespace ringc {


class Token {
public:
	enum TokenType {
		// invalid token.
		INVALID,
		// expressional operator symbols.
		// assignment operator
		ASSIGN,
		ASSIGN_ADD,
		ASSIGN_SUB,
		ASSIGN_MUL,
		ASSIGN_DIV,
		ASSIGN_MOD,
		ASSIGN_LSH,
		ASSIGN_RSH,
		ASSIGN_BITWISE_OR,
		ASSIGN_BITWISE_XOR,
		ASSIGN_BITWISE_AND,
		// binary operators
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		EXP,
		LSH,
		RSH,
		OR,
		HAT,
		AND,
		EQ,
		NE,
		LT,
		LE,
		GE,
		GT,
		// logical operators
		OROR,
		ANDAND,
		// negation operator
		NOT,
		// bitwise not
		TILDE,
		// structural symbopls.
		SEMICOLON,
		COMMA,
		HOOK,
		COLON,
		DOT,
		DQUOTE,
		LBRACKET,
		RBRACKET,
		LBRACE,
		RBRACE,
		LPAREN,
		RPAREN,
		LARROW,
		RARROW,
		// literals
		LIT_FALSE,
		LIT_TRUE,
		LIT_NUMERIC,
		LIT_CHAR,
		LIT_STRING,
		// Identifier
		IDENT,
		// keywordss
		KEYWORD_ELSE,
		KEYWORD_EXTERN,
		KEYWORD_FN,
		KEYWORD_IF,
		KEYWORD_LET,
		KEYWORD_MUT,
		KEYWORD_PUB,
		KEYWORD_THEN,
		KEYWORD_THIS,
		KEYWORD_USE,
		KEYWORD_TY_BOOL,
		KEYWORD_TY_INT,
		KEYWORD_TY_NIL,
		// end of file
		EoF,
	};

	Token(TokenType type);
	Token(TokenType type, StrId str_id);

	bool operator==(const Token& other) const;

	TokenType type() const;
	StrId str_id() const;

	string toString() const;

	bool isValid() const;
	bool isEof() const;

protected:
	TokenType _type;
	StrId _str_id;
};


class TokenUtil {
public:
	static Token maybeKeyword(const string& str);
	static Token maybeLitBool(const string& str);
	static Token reservedOrIdent(const string& str, StrId str_id);

	static bool isAssigmentOp(Token::TokenType token_type);
	static bool isLogicalOp(Token::TokenType token_type);
	static bool isBinaryOp(Token::TokenType token_type);
	static bool isUnaryOp(Token::TokenType token_type);
	static bool isPrimary(Token::TokenType token_type);
	static bool isLiteral(Token::TokenType token_type);
};


} // namespace ringc
} // namespace ring


#endif
