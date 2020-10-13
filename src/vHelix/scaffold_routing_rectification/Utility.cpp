#include "Utility.h"

// Code from https://bitbucket.org/sinbad/ogre/src/9db75e3ba05c/OgreMain/include/OgreVector3.h#cl-651
physics::quaternion_type rotationFromTo(physics::vec3_type from, physics::vec3_type to) {
	// Based on Stan Melax's article in Game Programming Gems
	from.normalize();
	to.normalize();

	const physics::real_type d = from.dot(to);
	// If dot == 1, vectors are the same
	if (d >= physics::real_type(1.0))
		return physics::quaternion_identity;
	else if (d < physics::real_type(1e-6 - 1.0)) {
		// Generate an axis
		physics::vec3_type axis = kPosXAxis.cross(from);
		if (axis.magnitudeSquared() <= physics::real_type(1e-6)) // pick another if colinear
			axis = kPosYAxis.cross(from);
		return physics::quaternion_type(physics::real_type(M_PI), axis.getNormalized());
	}
	else {
		physics::quaternion_type q;
		const physics::real_type s = std::sqrt((1 + d) * 2);
		const physics::real_type invs = 1 / s;
		const physics::vec3_type c(from.cross(to));

		q.x = c.x * invs;
		q.y = c.y * invs;
		q.z = c.z * invs;
		q.w = s / 2;
		return q.getNormalized();
	}
}
