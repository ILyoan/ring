#ifndef RING_RINGI_VALUES_H
#define RING_RINGI_VALUES_H


namespace ring {
namespace ringi {
	class ValInt;
	class ValBool;
}}


#include "../common.h"
#include <string>
using namespace ring::ringc;
using namespace std;


namespace ring {
namespace ringi {


enum ValueType {
	VALUE_INVALID,
	VALUE_FN,
	VALUE_INT,
	VALUE_BOOL,
};


class Value {
	ADD_PROPERTY(value_type, ValueType)

public:
	Value(ValueType value_type);

	static Value* NoneVal();
	static Value* UndefinedVal();
	static Value* UninitailizedVal();
	static Value* Nil();

	bool none();
	bool undefined();
	bool uninitialized();
	bool valid();
	bool nil();

	virtual string toString() const = 0;

	virtual Value* add(const Value* other) const;
	virtual Value* sub(const Value* other) const;
	virtual Value* mul(const Value* other) const;
	virtual Value* div(const Value* other) const;
	virtual Value* exp(const Value* other) const;

	virtual ValBool* eq(const Value* other) const;
	virtual ValBool* ne(const Value* other) const;
	virtual ValBool* lt(const Value* other) const;
	virtual ValBool* le(const Value* other) const;
	virtual ValBool* ge(const Value* other) const;
	virtual ValBool* gt(const Value* other) const;
};


class ValFn : public Value {
	ADD_PROPERTY(fn, NodeId)

public:
	ValFn(NodeId fn);

	string toString() const;
};


class ValNum : public Value {
public:
	ValNum(ValueType value_type);
};


class ValInt: public ValNum {
	ADD_PROPERTY(value, int)

public:
	ValInt(int v);
	ValInt(const string& v);

	string toString() const;

	Value* add(const Value* other) const;
	Value* sub(const Value* other) const;
	Value* mul(const Value* other) const;
	Value* div(const Value* other) const;
	Value* exp(const Value* other) const;

	ValBool* eq(const Value* other) const;
	ValBool* ne(const Value* other) const;
	ValBool* lt(const Value* other) const;
	ValBool* le(const Value* other) const;
	ValBool* ge(const Value* other) const;
	ValBool* gt(const Value* other) const;
};


class ValBool : public Value {
	ADD_PROPERTY(value, bool)

public:
	static ValBool* True();
	static ValBool* False();
	static ValBool* Bool(bool v);
	static ValBool* Bool(const string& v);

	string toString() const;

	ValBool* eq(const Value* other) const;
	ValBool* ne(const Value* other) const;

protected:
	ValBool(bool v);
	~ValBool();
	static ValBool* _true;
	static ValBool* _false;
};



} // namespace ringi
} // namespace ring


#endif
