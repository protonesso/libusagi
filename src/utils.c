#include "utils.h"

void umsg(unsigned char state, const char *msg, ...) {
	char *status;

	switch (state) {
		case '0': status = "[ OK ]"; break;
		case '1': status = "[WARN]"; break;
		case '2': status = "[FAIL]"; break;
		default:  status = "[ OK ]"; break;
	}

	printf("%s %s\n", status, msg);
}
