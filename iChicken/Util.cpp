#include "globals.h"
#include "Util.h"

std::string concatstrs(int argnum, ...) {
	va_list  valist;
	va_start(valist, argnum);

	std::string output;

	for (int vli = 0; vli < argnum; vli++) {
		output.append(va_arg(valist, std::string));
	}

	va_end(valist);

	return output;
}

// curdir() adapted from Anvi Jain's article on tutorialspoint
// https://www.tutorialspoint.com/find-out-the-current-working-directory-in-c-cplusplus

std::string curdir() {
	char buf[CURDIR_dirbufsize];
	char* cdbo = curdirbuf(buf, CURDIR_dirbufsize);
	ERRIF(cdbo == NULL, "Couldn't get current directory");
	return std::string(buf);
}