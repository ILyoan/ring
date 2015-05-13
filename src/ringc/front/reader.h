#ifndef RING_RINGC_FRONT_READER_H
#define RING_RINGC_FRONT_READER_H


#include <string>
using namespace std;


namespace ring {
namespace ringc {


const char NIL = 0xff;

struct Position {
public:
	Position();
	Position(char ch, int ix, int row, int col);

	bool isValid() const;
	void invalidate();

	int ix() const;
	char ch() const;

	void moveNextIdx();
	void moveNextCol();
	void moveNextRow();
	void setChar(char ch);

protected:
	char _ch;
	int _ix;
	int _row;
	int _col;
};


class Reader {
public:
	Reader(FILE* fp);
	Reader(const string& src);

	char curr() const;

	bool isEof() const;

	char bump();
	bool bump_if(char ch);
	char consumeWhitespaceAndComments();

protected:
	Position nextPosition(const Position& pos) const;
	bool readChunk();
	void Initialize();

protected:
	string _src;
	size_t _len;
	Position _pos;
	Position _pos_n1;
	Position _pos_n2;
};


class LexUtil {
public:
	static bool isNewLine(char ch);
	static bool isWhitespace(char ch);
	static bool isIdentStart(char ch);
	static bool isIdentContinue(char ch);
	static bool isAlpha(char ch);
	static bool isLowerAlpha(char ch);
	static bool isUpperAlpha(char ch);
	static bool isDecimalDigit(char ch);
	static bool isAlphaNum(char ch);
	static bool isDoubleQuote(char ch);
};


} // namespace ringc
} // namespace ring

#endif
