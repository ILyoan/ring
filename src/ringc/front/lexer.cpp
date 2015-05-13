#include "lexer.h"
#include <cassert>
using namespace ring::ringc;


Lexer::Lexer(FILE* fp, Session* session)
		: _reader(fp)
		, _session(session) {
}


Lexer::Lexer(const string& src, Session* session)
		: _reader(src)
		, _session(session) {
}


// Returns next token.
// TODO: scan string and character.
Token Lexer::nextToken() {
	char ch = _reader.consumeWhitespaceAndComments();

	if (_reader.isEof()) {
		return Token(Token::EoF);
	}

	if (LexUtil::isIdentStart(ch)) {
		return scanIdent();
	} else if (LexUtil::isDecimalDigit(ch)) {
		return scanNumber();
	} else if (ch == '"') {
		return scanString();
	} else {
		return scanOperatorOrStructure();
	}
}


// Scans identifier.
Token Lexer::scanIdent() {
	assert(LexUtil::isIdentStart(_reader.curr()));

	string token_str = "";
	token_str += _reader.bump();
	while (LexUtil::isIdentContinue(_reader.curr())) {
		token_str += _reader.bump();
	}

	return TokenUtil::reservedOrIdent(token_str, addToStrTable(token_str));
}


// Scans numeric literal.
// TODO: scan haxadecimal numbers.
// TODO: scan rational numbers.
Token Lexer::scanNumber() {
	assert(LexUtil::isDecimalDigit(_reader.curr()));

	return Token(Token::LIT_NUMERIC, addToStrTable(scanDigits()));
}


// Scans string literal
// TODO: handle escape characters
Token Lexer::scanString() {
	assert(LexUtil::isDoubleQuote(_reader.curr()));

	_reader.bump();

	string token_str = "";
	token_str += _reader.bump();
	while (!LexUtil::isDoubleQuote(_reader.curr())) {
		token_str += _reader.bump();
	}

	assert(LexUtil::isDoubleQuote(_reader.curr()));

	_reader.bump();

	return Token(Token::LIT_STRING, addToStrTable(token_str));
}


// Scans digits.
// TODO: scan haxadecimal numbers.
string Lexer::scanDigits() {
	string res = "";
	while (!_reader.isEof()) {
		if (LexUtil::isDecimalDigit(_reader.curr())) {
			res += _reader.bump();
		} else {
			break;
		}
	}
	return res;
}


Token Lexer::scanOperatorOrStructure() {
	assert(!LexUtil::isIdentStart(_reader.curr())
		&& !LexUtil::isDecimalDigit(_reader.curr()));

	char ch = _reader.bump();

	if (ch == '=') {
		if (_reader.bump_if('=')) {
			return Token(Token::EQ);
		} else {
			return Token(Token::ASSIGN);
		}
	} else if (ch == '!') {
		if (_reader.bump_if('=')) {
			return Token(Token::NE);
		} else {
			return Token(Token::NOT);
		}
	} else if (ch == '<') {
		if (_reader.bump_if('-')) {
			return Token(Token::LARROW);
		} else if (_reader.bump_if('=')) {
			return Token(Token::LE);
		} else if (_reader.bump_if('<')) {
			if (_reader.bump_if('=')) {
				return Token(Token::ASSIGN_LSH);
			} else {
				return Token(Token::LSH);
			}
		} else {
			return Token(Token::LT);
		}
	} else if (ch == '>') {
		if (_reader.bump_if('=')) {
			return Token(Token::GE);
		} else if (_reader.bump_if('>')) {
			if (_reader.bump_if('=')) {
				return Token(Token::ASSIGN_RSH);
			} else {
				return Token(Token::RSH);
			}
		} else {
			return Token(Token::GT);
		}
	} else if (ch == '+') {
		if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_ADD);
		} else {
			return Token(Token::ADD);
		}
	} else if (ch == '-') {
		if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_SUB);
		} else if (_reader.bump_if('>')) {
			return Token(Token::RARROW);
		} else {
			return Token(Token::SUB);
		}
	} else if (ch == '*') {
		if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_MUL);
		} else if (_reader.bump_if('*')) {
			return Token(Token::EXP);
		} else {
			return Token(Token::MUL);
		}
	} else if (ch == '/') {
		if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_DIV);
		} else {
			return Token(Token::DIV);
		}
	} else if (ch == '%') {
		if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_MOD);
		} else {
			return Token(Token::MOD);
		}
	} else if (ch == '&') {
		if (_reader.bump_if('&')) {
			return Token(Token::ANDAND);
		} else if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_BITWISE_AND);
		} else {
			return Token(Token::AND);
		}
	} else if (ch == '|') {
		if (_reader.bump_if('|')) {
			return Token(Token::OROR);
		} else if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_BITWISE_OR);
		} else {
			return Token(Token::OR);
		}
	} else if (ch == '^') {
		if (_reader.bump_if('=')) {
			return Token(Token::ASSIGN_BITWISE_XOR);
		} else {
			return Token(Token::HAT);
		}
	} else if (ch == '~') {
		return Token(Token::TILDE);
	} else if (ch == ';') {
		return Token(Token::SEMICOLON);
	} else if (ch == ',') {
		return Token(Token::COMMA);
	} else if (ch == '?') {
		return Token(Token::HOOK);
	} else if (ch == ':') {
		return Token(Token::COLON);
	} else if (ch == '.') {
		return Token(Token::DOT);
	} else if (ch == '[') {
		return Token(Token::LBRACKET);
	} else if (ch == ']') {
		return Token(Token::RBRACKET);
	} else if (ch == '{') {
		return Token(Token::LBRACE);
	} else if (ch == '}') {
		return Token(Token::RBRACE);
	} else if (ch == '(') {
		return Token(Token::LPAREN);
	} else if (ch == ')') {
		return Token(Token::RPAREN);
	} else if (ch == '"') {
		return Token(Token::DQUOTE);
	} else {
		_session->diagnostic()->report(REPORT_ERROR, "Unexpedted token %c", ch);
		return Token(Token::INVALID);
	}
}


StrId Lexer::addToStrTable(const string& str) {
	return _session->str_table()->add(str);
}
