/*
* Utility.h
*
*  Created on: Feb 10, 2014
*      Author: johan
*/

#ifndef UTILITY_H_
#define UTILITY_H_

#include "Definition.h"
#include "Physics.h"

#include <cassert>
#include <vector>
#include <iostream>

/*
 * Helper string methods
 */

inline bool ends_with(const std::string & haystack, const std::string & needle) {
	return haystack.find_last_of(needle) == haystack.length() - 1;
}

inline std::string strip_trailing_string(const std::string & str, const std::string & trailing) {
	return str.substr(0, str.length() - trailing.length());
}

#ifdef __GNUC__

#ifndef likely
#define likely(cond)   (__builtin_expect(!!(cond), 1))
#endif /* N likely */

#ifndef unlikely
#define unlikely(cond) (__builtin_expect(!!(cond), 0))
#endif /* N unlikely */

#else
#define likely(cond) cond
#define unlikely(cond) cond

#endif /* N __GNUC__ */

/*
* Missing useful math methods.
*/

template<typename T>
int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template<typename T>
int sgn_nozero(T val) {
	return (T(0) <= val) - (val < T(0));
}

template<typename T>
T toRadians(T degrees) {
	return T(degrees * M_PI / 180);
}

template<typename T>
T toDegrees(T radians) {
	return T(radians * 180 / M_PI);
}

/*
* Project vector 'from' onto 'onto'.
*/
inline physx::PxVec3 proj(const physx::PxVec3 & from, const physx::PxVec3 & onto) {
	return onto * (from.dot(onto) / onto.magnitudeSquared());
}

static const physx::PxVec3 kPosXAxis(1, 0, 0);
static const physx::PxVec3 kPosYAxis(0, 1, 0);
static const physx::PxVec3 kPosZAxis(0, 0, 1);

static const physx::PxVec3 kNegXAxis(-1, 0, 0);
static const physx::PxVec3 kNegYAxis(0, -1, 0);
static const physx::PxVec3 kNegZAxis(0, 0, -1);

static const physx::PxVec3 kZeroVec(0, 0, 0);
static const physx::PxVec3 kOneVec(1, 1, 1);

inline auto signedAngle(physx::PxVec3 from, physx::PxVec3 to, const physx::PxVec3 & normal)->decltype(from.dot(to)) {
	from.normalize();
	to.normalize();
	const physx::PxReal tofrom(to.dot(from));

	if (abs(tofrom) >= physx::PxReal(1.0 - 1e-06))
		return (1 - sgn_nozero(tofrom)) * physx::PxReal(M_PI / 2);
	else {
		const int sign = sgn_nozero((normal.cross(from)).dot(to));
		return sign * std::acos(tofrom);
	}
}

physx::PxQuat rotationFromTo(physx::PxVec3 from, physx::PxVec3 to);

template<typename U, typename V>
V circular_index(U index, V length) {
	return (index + length) % length;
}

template<typename IteratorT, typename ContainerT>
IteratorT circular_increment(IteratorT it, ContainerT & container) {
	return ++it == container.end() ? container.begin() : it;
}

template<typename IteratorT, typename ContainerT>
IteratorT circular_decrement(IteratorT it, ContainerT & container) {
	return it == container.begin() ? container.end() - 1 : --it;
}

template<typename U, typename V>
inline U apothem(U s, V n) {
	return s / U(2 * std::tan(M_PI / n));
}

/*
 * Code for system independency.
 */

// Seed the random number generator with the current time.
void seed();

#ifdef _WINDOWS

#include <windows.h>

typedef void (*FuncPtr)();

// Do not use, internal only, use setinterrupthandler() below.
template<typename FuncPtr>
BOOL WINAPI __InterruptHandlerRoutine(_In_  DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT)
		FuncPtr();
	return TRUE;
}

// Set interrupt handler ^C by template argument to function pointer.
template<typename FuncPtr>
void setinterrupthandler() {
	SetConsoleCtrlHandler(__InterruptHandlerRoutine<FuncPtr>, TRUE);
}

// Sleep at least the number of given milliseconds.
template<typename IntegerT>
inline void sleepms(IntegerT ms) {
	Sleep(ms);
}

inline void seed() {
	srand(GetTickCount());
}

inline unsigned int numcpucores() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
    return (unsigned int)(sysinfo.dwNumberOfProcessors);
}

#else

// Do not use, internal only, use setinterrupthandler below.
template<typename void(*FuncPtr)()>
void __interrupthandlerroutine(int s) {
	FuncPtr();
}

// Set the interrupt handler ^C by template argument to function pointer.
template<typename void(*FuncPtr)()>
inline void setinterrupthandler() {
	struct sigaction sigint_handler;
	sigint_handler.sa_handler = __interrupthandlerroutine<FuncPtr>;
	sigemptyset(&sigint_handler.sa_mask);
	sigint_handler.sa_flags = 0;

	sigaction(SIGINT, &sigint_handler, NULL);
}

// Sleep at least the number of given milliseconds.
template<typename IntegerT>
inline void sleepms(IntegerT ms) {
	usleep(ms * 1000);
}

// Seed the random number generator with the current time.
inline void seed() {
	srand(time(NULL));
}

inline unsigned int numcpucores() {
	return unsigned int(sysconf(_SC_NPROCESSORS_ONLN));
}

#endif /* N _WINDOWS */

#endif /* UTILITY_H_ */
