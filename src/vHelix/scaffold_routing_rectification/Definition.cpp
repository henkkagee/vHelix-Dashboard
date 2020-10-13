/*
* Definition.cpp
*
*  Created on: Feb 10, 2014
*      Author: johan
*/

#include "Definition.h"
#include <cstdarg>
#include <cstdio>

namespace Debug {
	void printf(const char *file, const char *function, size_t line, const char *expr, ...) {
		va_list vl;
		va_start(vl, expr);

		fprintf(stderr, "%s(%lu): in %s: ", file, line, function);
		vfprintf(stderr, expr, vl);
		fprintf(stderr, "\n");

		va_end(vl);
	}

	void printf_noreturn(const char *file, const char *function, size_t line, const char *expr, ...) {
		va_list vl;
		va_start(vl, expr);

		fprintf(stderr, "%s(%lu): in %s: ", file, line, function);
		vfprintf(stderr, expr, vl);

		va_end(vl);
	}
}


