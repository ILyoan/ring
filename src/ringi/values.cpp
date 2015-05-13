#include "values.h"
#include <cstdlib>
#include <cmath>
using namespace ring::ringi;


#define UNIMPLEMENTED(op) \
	fprintf(stderr, "Unimplemented operateraion %s for value type %d\n", op, value_type());\
	exit(EXIT_FAILURE);


Value::Value(ValueType value_type)
		: _value_type(value_type) {
}


Value* Value::NoneVal() {
	return NULL;
}


Value* Value::UndefinedVal() {
	return (Value*)-1;
}


Value* Value::UninitailizedVal() {
	return (Value*)-2;
}


Value* Value::Nil() {
	return (Value*)-3;
}


bool Value::none() {
	return this == NoneVal();
}


bool Value::undefined() {
	return this == UndefinedVal();
}


bool Value::uninitialized() {
	return this == UninitailizedVal();
}


bool Value::nil() {
	return this == Nil();
}


bool Value::valid() {
	return !none() && !undefined() && !uninitialized();
}


Value* Value::add(const Value* other) const {
	UNIMPLEMENTED("add");
}


Value* Value::sub(const Value* other) const {
	UNIMPLEMENTED("sub");
}


Value* Value::mul(const Value* other) const {
	UNIMPLEMENTED("mul");
}


Value* Value::div(const Value* other) const {
	UNIMPLEMENTED("div");
}


Value* Value::exp(const Value* other) const {
	UNIMPLEMENTED("exp");
}


ValBool* Value::eq(const Value* other) const {
	UNIMPLEMENTED("eq");
}


ValBool* Value::ne(const Value* other) const {
	UNIMPLEMENTED("ne");
}


ValBool* Value::lt(const Value* other) const {
	UNIMPLEMENTED("lt");
}


ValBool* Value::le(const Value* other) const {
	UNIMPLEMENTED("le");
}


ValBool* Value::ge(const Value* other) const {
	UNIMPLEMENTED("ge");
}


ValBool* Value::gt(const Value* other) const {
	UNIMPLEMENTED("gt");
}


ValFn::ValFn(NodeId fn)
		: Value(VALUE_FN)
		, _fn(fn) {
}


string ValFn::toString() const {
	return "function";
}


ValNum::ValNum(ValueType value_type)
		: Value(value_type) {
}


ValInt::ValInt(int v)
		: ValNum(VALUE_INT)
		, _value(v) {
}


ValInt::ValInt(const string& v)
		: ValNum(VALUE_INT)
		, _value(atoi(v.c_str())) {
}


string ValInt::toString() const {
	char buffer[20] = {0};
	sprintf(buffer, "%d", value());
	return buffer;
}


Value* ValInt::add(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return new ValInt(value() + iother->value());
}


Value* ValInt::sub(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return new ValInt(value() - iother->value());
}


Value* ValInt::mul(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return new ValInt(value() * iother->value());
}


Value* ValInt::div(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return new ValInt(value() / iother->value());
}


Value* ValInt::exp(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return new ValInt(pow(value(), iother->value()));
}


ValBool* ValInt::eq(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return value() == iother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValInt::ne(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return value() != iother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValInt::lt(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return value() < iother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValInt::le(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return value() <= iother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValInt::ge(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return value() >= iother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValInt::gt(const Value* other) const {
	const ValInt* iother = dynamic_cast<const ValInt*>(other);
	return value() > iother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValBool::_true = NULL;
ValBool* ValBool::_false = NULL;


ValBool::ValBool(bool v)
		: Value(VALUE_BOOL)
		, _value(v) {
}


ValBool::~ValBool() {
}


ValBool* ValBool::True() {
	if (_true == NULL) {
		_true = new ValBool(true);
	}
	return _true;
}


ValBool* ValBool::False() {
	if (_false == NULL) {
		_false = new ValBool(false);
	}
	return _false;
}


ValBool* ValBool::Bool(bool v) {
	return v ? True() : False();
}


ValBool* ValBool::Bool(const string& v) {
	return v == "true" ? True() : False();
}


string ValBool::toString() const {
	if (value()) {
		return "true";
	} else {
		return "false";
	}
}


ValBool* ValBool::eq(const Value* other) const {
	const ValBool* bother = dynamic_cast<const ValBool*>(other);
	return value() == bother->value() ? ValBool::True() : ValBool::False();
}


ValBool* ValBool::ne(const Value* other) const {
	const ValBool* bother = dynamic_cast<const ValBool*>(other);
	return value() != bother->value() ? ValBool::True() : ValBool::False();
}
