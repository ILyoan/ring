#include "util.h"
using namespace ring::ringc;

#include <cstdlib>


string RingcUtil::baseName(const string& path) {
	return path.substr(path.find_last_of("/\\") + 1);
}


string RingcUtil::fileName(const string& path) {
	string filename = baseName(path);
	auto p(filename.find_last_of('.'));
  	return p > 0 && p != string::npos ? filename.substr(0, p) : filename;
}


string RingcUtil::filePath(const string& path) {
	return path.substr(0, path.find_last_of("/\\")+1);
}
