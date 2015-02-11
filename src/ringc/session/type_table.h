#ifndef RING_RINGC_SESSION_TYPE_TABLE_H
#define RING_RINGC_SESSION_TYPE_TABLE_H


#include "../../common.h"
#include "../util/table_map.h"
#include "../syntax/ast.h"
using namespace ring::ringc::ast;

namespace ring {
namespace ringc {
	typedef TypePrimType PrimTypeKey;
	typedef TypeId ArrayTypeKey;
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

	Type* getType(TypeId ty);
	TypePrim* getPrimType(TypeId ty);
	TypeFunc* getFuncType(TypeId ty);
	TypeArray* getArrayType(TypeId ty);
	TypeId getPrimTypeId(const PrimTypeKey& prim_type_key);
	TypeId getFuncTypeId(const FuncTypeKey& func_type_key);
	TypeId getArrayTypeId(const ArrayTypeKey& array_type_key);

	bool isFuncType(TypeId ty);
	bool isPrimType(TypeId ty);
	bool isArrayType(TypeId ty);

protected:
	TypeId addType(Type* type);

protected:
	TableMap<TypeId, Type*> _type_table;
	map<PrimTypeKey, TypeId> _prim_type_map;
	map<FuncTypeKey, TypeId> _func_type_map;
	map<ArrayTypeKey, TypeId> _array_type_map;
};


} // namespace ringc
} // namespace ring


#endif