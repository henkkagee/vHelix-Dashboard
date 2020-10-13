/*
* Definition.h
*
*  Created on: Feb 10, 2014
*      Author: johan
*/

#ifndef DEFINITION_H_
#define DEFINITION_H_

//#define DEBUG

#define _CRT_SECURE_NO_WARNINGS /* Visual Studios annoying warnings */
#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define NOMINMAX /* Windows defines min, max that messes up std::min, std::max */

#define ANGLE_EPSILON (10e-05)

#include <cstdlib>
#include <cmath> // Needed by PxPhysicsAPI.h under Linux.

#include "Physics.h"

namespace Debug {
	void printf(const char *file, const char *function, size_t line, const char *expr, ...);
	void printf_noreturn(const char *file, const char *function, size_t line, const char *expr, ...);
}

#define PRINT(fmt, ...) ::Debug::printf(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define PRINT_NORETURN(fmt, ...) ::Debug::printf_noreturn(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif /* DEFINITION_H_ */
