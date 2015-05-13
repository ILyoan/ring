#ifndef RING_RINGC_SESSION_DIAGNOSTIC_H
#define RING_RINGC_SESSION_DIAGNOSTIC_H


namespace ring {
namespace ringc {

	enum ReportLevel {
		REPORT_FATAL,
		REPORT_ERROR,
		REPORT_WARNING,
		REPORT_NOTE,
		REPORT_NONE,
	};

	enum LogLevel {
		LOG_NONE,
		LOG_FATAL,
		LOG_INFO,
		LOG_DEBUG,
		LOG_TRACE,
	};

	class Diagnostic;
	class ScopeTracer;

}}


#include <cstdio>
#include <string>
#include "session.h"
using namespace std;
using namespace ring::ringc;


namespace ring {
namespace ringc {


class Diagnostic {
public:
	Diagnostic(LogLevel log_level);

	LogLevel logLevel() const;
	bool isLoggableLevel(LogLevel log_level) const;
	ReportLevel mostSignificantLevel() const;
	void resetReport();

	void report(ReportLevel level, const char* fmt, ...);
	void log(LogLevel level, const char* fmt, ...);

protected:
	void updateLevel(ReportLevel level);

protected:
	ReportLevel _most_report_level;
	LogLevel _log_level;
};


class ScopeTracer {
	public:
		ScopeTracer(Session* session, const string& scope_name);
		~ScopeTracer();
	private:
		Session* _session;
		string _scope_name;
};


} // namespace ringc
} // namespace ring


#endif