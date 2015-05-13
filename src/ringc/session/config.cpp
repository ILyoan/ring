#include "config.h"
#include <cstdio>
#include <string>
#include <algorithm>
using namespace ring::ringc;
using namespace std;


pair<string, string> splitArgKeyValue(string arg) {
	int pos = arg.find("=");
	if (pos == string::npos) {
		return make_pair(arg, "");
	} else {
		string val = arg.substr(pos+1);
		transform(val.begin(), val.end(), val.begin(), ::tolower);
		return make_pair(arg.substr(0, pos), val);
	}
}

Config::Config()
		: _source("")
		, _print_ast(false)
		, _log_level(LOG_INFO)
		, _linker("g++")
		, _link_opt("-lringrt") {
}



void Config::parseArgument(char* arg) {
	if (arg[0] == '-') {
		auto res = splitArgKeyValue(&arg[1]);
		if (res.first == "log-level") {
			if (res.second == "info") {
				log_level(LOG_INFO);
			} else if (res.second == "debug") {
				log_level(LOG_DEBUG);
			} else if (res.second == "trace") {
				log_level(LOG_TRACE);
			} else {
				fprintf(stderr, "ringci: log-level should be one of: INFO, DEBUG, TRACE\n");
			}
		} else if (res.first == "print-ast") {
			if (res.second == "" || res.second == "true") {
				print_ast(true);
			} else if (res.second == "false") {
				print_ast(false);
			} else {
				fprintf(stderr, "ringci: log-level should be one of: TRUE, FLASE\n");
			}
		} else {
			fprintf(stderr, "ringci: unknown argument: %s\n", res.first.c_str());
		}
	} else {
		source(arg);
	}
}
