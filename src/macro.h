#ifndef RING_MACRO_H
#define RING_MACRO_H

#include <stdarg.h>
#include <cassert>


#define ADD_PROPERTY(name, type)\
	public: type name() const { return _##name; } \
	public: void name(const type name) { _##name = name; }\
	protected: type _##name;

#define ADD_PROPERTY_R(name, type)\
	public: type& name() { return _##name; } \
	public: void name(const type& name) { _##name = name; }\
	protected: type _##name;


#define ADD_PROPERTY_P(name, type)\
	public: type* name() {return _##name; }\
	public: void name(const type* name) { _##name = const_cast<type*>(name); }\
	protected: type* _##name;


#define DIAG_REPORT(diagnostic, level, ...) \
	do { diagnostic->report(level, __VA_ARGS__); } while(0)

#define DIAG_LOG(diagnostic, level, ...) \
	do {\
		if (diagnostic->isLoggableLevel(level)) {\
			diagnostic->log(level, __VA_ARGS__);\
		}\
	} while (0)

#define REPORT(level, ...) DIAG_REPORT(session()->diagnostic(), level, __VA_ARGS__)
#define FATAL(...) REPORT(REPORT_FATAL, __VA_ARGS__)
#define ERROR(...) REPORT(REPORT_ERROR, __VA_ARGS__)
#define LOG(level, ...) DIAG_LOG(session()->diagnostic(), level, __VA_ARGS__)

#define ASSERT(x, ...) do { if (!(x)) FATAL(__VA_ARGS__); } while (0)
#define ASSERT_EQ(x, y, ...) ASSERT((x)==(y), __VA_ARGS__)
#define ASSERT_NE(x, y, ...) ASSERT((x)!=(y), __VA_ARGS__)

#define SRCPOS "%s %s:%d",__PRETTY_FUNCTION__,__FILE__,__LINE__

#define DEF_ID_TYPE(name) \
	struct name {\
		name() : _v(-1) {}\
		name(int v) : _v(v) {}\
		int value() const { return _v; }\
		bool none() const { return _v == -1; }\
		bool some() const { return _v != -1; }\
		bool operator < (const name& other) const { return _v < other._v; }\
		bool operator ==(const name& other) const { return _v == other._v; }\
		bool operator !=(const name& other) const { return _v != other._v; }\
		int _v;\
	};\

#endif
