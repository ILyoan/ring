#include <cstdio>

extern "C"
int printd(int d) {
	printf("%d\n", d);
	return d;
}