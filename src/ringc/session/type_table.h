#ifndef RING_RINGC_SESSION_TYPE_TABLE_H
#define RING_RINGC_SESSION_TYPE_TABLE_H


#include "../../common.h"
#include "../util/table_map.h"
#include "../syntax/ast.h"
using namespace ring::ringc::ast;

namespace ring {
namespace ringc {
	typedef PrimitiveTypeType PrimTypeKey;
	typedef pair<vector<TypeId>, TypeId> FuncTypeKey;
	class TypeTable;
}}


#include "session.h"


namespace ring {
namespace ringc {


class TypeTable {
	ADD_PROPERTY_P(session, Session);
public:
	TypeTable(Session* session);
	virtual ~TypeTable();

	Type* getType(TypeId type_id);
	PrimitiveType* getPrimType(TypeId type_id);
	FunctionType* getFuncType(TypeId type_id);
	TypeId getPrimTypeId(const PrimTypeKey& prim_type_key);
	TypeId getFuncTypeId(const FuncTypeKey& func_type_key);

	bool isFuncType(TypeId type_id);
	bool isPrimType(TypeId type_id);

protected:
	TypeId addType(Type* type);

protected:
	TableMap<TypeId, Type*> _type_table;
	map<PrimTypeKey, TypeId> _prim_type_map;
	map<FuncTypeKey, TypeId> _func_type_map;
};


} // namespace ringc
} // namespace ring


#endif