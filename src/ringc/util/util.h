#ifndef RING_RINGC_UTIL_UTIL_H
#define RING_RINGC_UTIL_UTIL_H

#include <string>
using namespace std;

namespace ring {
namespace ringc {


class RingcUtil {
public:
	static string baseName(const string& path);
	static string fileName(const string& path);
	static string filePath(const string& path);
};


} // namespace ringc
} // namespace ring


#endif