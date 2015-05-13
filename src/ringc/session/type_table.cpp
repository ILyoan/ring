#include "type_table.h"
#include <algorithm>
using namespace std;
using namespace ring::ringc;


TypeTable::TypeTable(Session* session)
		: _session(session)
		, _type_table(NULL) {
}


TypeTable::~TypeTable() {
	for_each(_type_table.begin(), _type_table.end(), [](Type* type) { delete type; });
}


Type* TypeTable::getType(TypeId type_id) {
	ASSERT(type_id.value() >= 0 && type_id.value() < _type_table.length(), SRCPOS);
	return _type_table.value(type_id);
}


PrimitiveType* TypeTable::getPrimType(TypeId type_id) {
	ASSERT(isPrimType(type_id), SRCPOS);
	return static_cast<PrimitiveType*>(getType(type_id));
}


FunctionType* TypeTable::getFuncType(TypeId type_id) {
	ASSERT(isFuncType(type_id), SRCPOS);
	return static_cast<FunctionType*>(getType(type_id));
}


TypeId TypeTable::getPrimTypeId(const PrimTypeKey& prim_type_key) {
	if (_prim_type_map.count(prim_type_key) > 0) {
		return _prim_type_map[prim_type_key];
	} else {
		TypeId type_id = addType(new PrimitiveType(prim_type_key));
		_prim_type_map[prim_type_key] = type_id;
		return type_id;
	}
}


TypeId TypeTable::getFuncTypeId(const FuncTypeKey& func_type_key) {
	if (_func_type_map.count(func_type_key) > 0) {
		return _func_type_map[func_type_key];
	} else {
		TypeId type_id = addType(new FunctionType(func_type_key.first, func_type_key.second));
		_func_type_map[func_type_key] = type_id;
		return type_id;
	}
}


bool TypeTable::isPrimType(TypeId type_id) {
	if (type_id.none()) {
		return false;
	} else {
		ASSERT(type_id.value() >= 0 && type_id.value() < _type_table.length(), SRCPOS);
		return _type_table.value(type_id)->isPrimitiveType();
	}
}


bool TypeTable::isFuncType(TypeId type_id) {
	if (type_id.none()) {
		return false;
	} else {
		ASSERT(type_id.value() >= 0 && type_id.value() < _type_table.length(), SRCPOS);
		return _type_table.value(type_id)->isFunctionType();
	}
}


TypeId TypeTable::addType(Type* type) {
	ASSERT(type->type_id().none(), SRCPOS);
	TypeId type_id = _type_table.add(type);
	type->type_id(type_id);
	return type_id;
}

