#include "reader.h"
using namespace ring::ringc;


Position::Position()
		: _ch(0), _ix(-1), _row(0), _col(0) {
}


Position::Position(char ch, int ix, int row, int col)
		: _ch(ch), _ix(ix), _row(row), _col(col) {
}


bool Position::isValid() const {
	return _ix >= 0;
}


void Position::invalidate() {
	_ix = -1;
}


int Position::ix() const {
	return _ix;
}


char Position::ch() const {
	if (isValid()) return _ch;
	else NIL;
}


void Position::moveNextIdx() {
	_ix += 1;
}


void Position::moveNextCol() {
	_col += 1;
}


void Position::moveNextRow() {
	_col = 0;
	_row += 1;
}

void Position::setChar(char ch) {
	_ch = ch;
}


Reader::Reader(FILE* fp) {
	char buffer[1024];
	do {
		int len = fread(buffer, 1, 1023, fp);
		buffer[len] = 0;
		_src += buffer;
	} while (!feof(fp));
	Initialize();
}


Reader::Reader(const string& src)
		: _src(src) {
	Initialize();
}


void Reader::Initialize() {
	_len = _src.size();
	if (_len > 0) {
		_pos = Position(_src[0], 0, 0, 0);
		_pos_n1 = nextPosition(_pos);
		_pos_n2 = nextPosition(_pos_n1);
	}
}


char Reader::curr() const {
	return _pos.ch();
}


bool Reader::isEof() const {
	return !_pos.isValid();
}


// Consumes a char and returns that char.
char Reader::bump() {
	char res = _pos.ch();
	if (_pos.isValid()) {
		_pos = _pos_n1;
		_pos_n1 = _pos_n2;
		_pos_n2 = nextPosition(_pos_n2);
	}
	return res;
}


// Consumes a char if current char matches the given char, then returns true.
// Otherwise returns false.
bool Reader::bump_if(char ch) {
	if (curr() == ch) {
		bump();
		return true;
	} else {
		return false;
	}
}

// Consumes whitespaces or comments, and then returns the next char.
char Reader::consumeWhitespaceAndComments() {
	while (LexUtil::isWhitespace(curr())) {
		bump();
	}
	return curr();
}


// Returns next position of given position.
// If given position is invalid then returns invalid position.
// If next position reaches eof then returns invalid position.
// Else returns valid next position.
Position Reader::nextPosition(const Position& pos) const {
	Position newPos = pos;
	if (newPos.isValid()) {
		// Given position is valid, move to next position.
		newPos.moveNextIdx();
		if (newPos.ix() < _len) {
			// Next position does not reach eof.
			// Updates row and col.
			newPos.setChar(_src[newPos.ix()]);
			if (LexUtil::isNewLine(newPos.ch())) {
				newPos.moveNextCol();
			} else {
				newPos.moveNextRow();
			}
		} else {
			// Next position reaches eof, make it invalid.
			newPos.invalidate();
		}
	}
	return newPos;
}


bool LexUtil::isNewLine(char ch) {
	return ch == '\n' || ch == '\r';
}


bool LexUtil::isWhitespace(char ch) {
	return isNewLine(ch) || ch == ' ' || ch == '\t';
}


bool LexUtil::isIdentStart(char ch) {
	return isAlpha(ch) || ch == '_';
}


bool LexUtil::isIdentContinue(char ch) {
	return isIdentStart(ch) || isDecimalDigit(ch);
}


bool LexUtil::isAlpha(char ch) {
	return isLowerAlpha(ch) || isUpperAlpha(ch);
}


bool LexUtil::isLowerAlpha(char ch) {
	return ch >= 'a' && ch <= 'z';
}


bool LexUtil::isUpperAlpha(char ch) {
	return ch >= 'A' && ch <= 'Z';
}


bool LexUtil::isDecimalDigit(char ch) {
	return ch >= '0' && ch <= '9';
}


bool LexUtil::isAlphaNum(char ch) {
	return isAlpha(ch) || isDecimalDigit(ch);
}


bool LexUtil::isDoubleQuote(char ch) {
	return ch == '"';
}
