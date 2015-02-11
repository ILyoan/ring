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


Type* TypeTable::getType(TypeId ty) {
	ASSERT(ty.value() >= 0 && ty.value() < _type_table.length(), SRCPOS);
	return _type_table.value(ty);
}


TypePrim* TypeTable::getPrimType(TypeId ty) {
	ASSERT(isPrimType(ty), SRCPOS);
	return static_cast<TypePrim*>(getType(ty));
}


TypeFunc* TypeTable::getFuncType(TypeId ty) {
	ASSERT(isFuncType(ty), SRCPOS);
	return static_cast<TypeFunc*>(getType(ty));
}


TypeArray* TypeTable::getArrayType(TypeId ty) {
	ASSERT(isArrayType(ty), SRCPOS);
	return static_cast<TypeArray*>(getType(ty));
}


TypeId TypeTable::getPrimTypeId(const PrimTypeKey& prim_type_key) {
	if (_prim_type_map.count(prim_type_key) > 0) {
		return _prim_type_map[prim_type_key];
	} else {
		TypeId ty = addType(new TypePrim(prim_type_key));
		_prim_type_map[prim_type_key] = ty;
		return ty;
	}
}


TypeId TypeTable::getFuncTypeId(const FuncTypeKey& func_type_key) {
	if (_func_type_map.count(func_type_key) > 0) {
		return _func_type_map[func_type_key];
	} else {
		TypeId ty = addType(new TypeFunc(func_type_key.first, func_type_key.second));
		_func_type_map[func_type_key] = ty;
		return ty;
	}
}


TypeId TypeTable::getArrayTypeId(const ArrayTypeKey& array_type_key) {
	if (_array_type_map.count(array_type_key) > 0) {
		return _array_type_map[array_type_key];
	} else {
		TypeId ty = addType(new TypeArray(array_type_key));
		_array_type_map[array_type_key] = ty;
		return ty;
	}
}


bool TypeTable::isPrimType(TypeId ty) {
	if (ty.none()) {
		return false;
	} else {
		ASSERT(ty.value() >= 0 && ty.value() < _type_table.length(), SRCPOS);
		return _type_table.value(ty)->isPrimType();
	}
}


bool TypeTable::isFuncType(TypeId ty) {
	if (ty.none()) {
		return false;
	} else {
		ASSERT(ty.value() >= 0 && ty.value() < _type_table.length(), SRCPOS);
		return _type_table.value(ty)->isFuncType();
	}
}


bool TypeTable::isArrayType(TypeId ty) {
	if (ty.none()) {
		return false;
	} else {
		ASSERT(ty.value() >= 0 && ty.value() < _type_table.length(), SRCPOS);
		return _type_table.value(ty)->isArrayType();
	}
}


TypeId TypeTable::addType(Type* type) {
	ASSERT(type->ty().none(), SRCPOS);
	TypeId ty = _type_table.add(type);
	type->ty(ty);
	return ty;
}

