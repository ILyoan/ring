#ifndef RING_RINGC_SESSION_CONFIG_H
#define RING_RINGC_SESSION_CONFIG_H


namespace ring {
namespace ringc {
	class Config;
}}


#include <string>
#include "diagnostic.h"
using namespace std;


namespace ring {
namespace ringc {


class Config {
	ADD_PROPERTY_R(source, string)
	ADD_PROPERTY(print_ast, bool)
	ADD_PROPERTY(log_level, LogLevel)
	ADD_PROPERTY(linker, string)
	ADD_PROPERTY(link_opt, string)

public:
	Config();

	void parseArgument(char* arg);
};


} // namespace ringc
} // namespace ring


#endif
