#ifndef RING_RINGC_TRANS_TRANS_H
#define RING_RINGC_TRANS_TRANS_H


#include "../session/session.h"
#include "../syntax/ast.h"
#include "llvm/IR/Module.h"


namespace ring {
namespace ringc {


class Trans {
	ADD_PROPERTY_P(session, Session)

public:
	Trans(Session* session);

	llvm::Module* trans(ast::Module* module);
};


} // namespace ringc
} // namespace ring


#endif
