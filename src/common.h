#ifndef RING_COMMON_H
#define RING_COMMON_H

#include "macro.h"
#include <string>

namespace ring {
namespace ringc {


DEF_ID_TYPE(StrId)
typedef StrId NameId;
DEF_ID_TYPE(ScopeId)
DEF_ID_TYPE(NodeId)
DEF_ID_TYPE(TypeId)
DEF_ID_TYPE(SymbolId)

const std::string MainName = "main";

}
}

#endif

