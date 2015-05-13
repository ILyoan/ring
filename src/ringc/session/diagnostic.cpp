#include <unistd.h>
#include <stdarg.h>
#include <execinfo.h>
#include "diagnostic.h"
using namespace ring::ringc;


const char* getReportLevelStr(ReportLevel level) {
	static const char* levels[] = {
		"fatal",
		"error",
		"warning",
		"note",
	};
	return levels[level];
}

const char* getLogLevelStr(LogLevel level) {
	static const char* levels[] = {
		"none",
		"fatal",
		"info",
		"debug",
		"trace",
	};
	return levels[level];
}



Diagnostic::Diagnostic(LogLevel log_level)
		: _most_report_level(REPORT_NONE)
		, _log_level(log_level) {
}


void Diagnostic::report(ReportLevel level, const char* fmt, ...) {
	fprintf(stderr, "[%s] ", getReportLevelStr(level));
	va_list argptr;
	va_start(argptr, fmt);
	vfprintf(stderr, fmt, argptr);
	va_end(argptr);
	fprintf(stderr, "\n\n");

	updateLevel(level);

	if (level == REPORT_FATAL) {
		void* bt[20];
		size_t bts = backtrace(bt, 20);
		backtrace_symbols_fd(bt, bts, STDERR_FILENO);
		exit(1);
	}
}


void Diagnostic::log(LogLevel level, const char* fmt, ...) {
	if (isLoggableLevel(level)) {
		fprintf(stdout, "(%s) ", getLogLevelStr(level));
		va_list argptr;
		va_start(argptr, fmt);
		vfprintf(stdout, fmt, argptr);
		va_end(argptr);
		fprintf(stdout, "\n");
	}
}


ReportLevel Diagnostic::mostSignificantLevel() const {
	return _most_report_level;
}


void Diagnostic::resetReport() {
	_most_report_level = REPORT_NONE;
}


LogLevel Diagnostic::logLevel() const {
	return _log_level;
}


bool Diagnostic::isLoggableLevel(LogLevel log_level) const {
	return log_level <= logLevel();
}


void Diagnostic::updateLevel(ReportLevel level) {
	if (level < _most_report_level) {
		_most_report_level = level;
	}
}



ScopeTracer::ScopeTracer(Session* session, const string& scope_name)
		: _session(session)
		, _scope_name(scope_name) {
	_session->diagnostic()->log(LOG_TRACE, "START %s", _scope_name.c_str());
}


ScopeTracer::~ScopeTracer() {
	_session->diagnostic()->log(LOG_TRACE, "END %s", _scope_name.c_str());
}
