#include "ast_table.h"
#include <algorithm>
using namespace std;
using namespace ring::ringc;


AstTable::AstTable(Session* session)
		: TableMap<NodeId, AstNode*>(NULL)
		, _session(session) {
}


AstTable::~AstTable() {
	for_each(begin(), end(), [](AstNode* node) { delete node; });
}


NodeId AstTable::add(AstNode* node) {
	ASSERT(node->node_id().none(), SRCPOS);
	NodeId id = TableMap<NodeId, AstNode*>::add(node);
	node->node_id(id);
	return id;
}

