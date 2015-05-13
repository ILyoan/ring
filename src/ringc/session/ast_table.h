#ifndef RING_RINGC_SESSION_AST_TABLE_H
#define RING_RINGC_SESSION_AST_TABLE_H


namespace ring {
namespace ringc {
	class AstTable;
}};


#include "../../common.h"
#include "../util/table_map.h"
#include "../syntax/ast.h"
#include "session.h"
using namespace ring::ringc;
using namespace ring::ringc::ast;


namespace ring {
namespace ringc {


class AstTable : public TableMap<NodeId, AstNode*> {
	ADD_PROPERTY_P(session, Session);
public:
	AstTable(Session* session);
	virtual ~AstTable();

	// Adds a AST node and returns its id.
	NodeId add(AstNode* node);
};


} // namespace ringc
} // namespace ring


#endif