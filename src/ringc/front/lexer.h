#ifndef RING_RINGC_FRONT_LEXER_H
#define RING_RINGC_FRONT_LEXER_H


#include <string>
#include "../session/session.h"
#include "../syntax/token.h"
#include "reader.h"
using namespace std;
using namespace ring::ringc;


namespace ring {
namespace ringc {


class Lexer {
public:
	Lexer(FILE* fp, Session* session);
	Lexer(const string& src, Session* session);

	Token nextToken();

protected:
	Token scanIdent();
	Token scanNumber();
	Token scanString();
	Token scanOperatorOrStructure();

	string scanDigits();

	StrId addToStrTable(const string& str);

protected:
	Reader _reader;
	Session* _session;
};


} // namespace ringc
} // namespace ring


#endif
