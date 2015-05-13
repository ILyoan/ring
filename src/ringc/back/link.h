#ifndef RING_RINGC_BACK_LINK_H
#define RING_RINGC_BACK_LINK_H


#include "../session/session.h"
#include "../session/module_info.h"


namespace ring {
namespace ringc {


class Link {
	ADD_PROPERTY_P(session, Session)

public:
	Link(Session* session);

	bool link(ModuleInfo* module);
	bool writeModuleObject(ModuleInfo* module);
	bool linkOutput(ModuleInfo* module);

protected:
	void initializeLLVMTargets();
};


} // namespace ringc
} // namespace ring


#endif
