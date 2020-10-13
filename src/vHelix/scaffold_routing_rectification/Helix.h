#ifndef _HELIX_H_
#define _HELIX_H_

#include "Utility.h"

#include <array>
#include <utility>

namespace Phys {
class Helix {
public:
	struct Connection {
		Helix *helix;
		physics::spring_joint_type *joint;

		operator bool() const {
			return helix && joint;
		}

		inline Connection(Helix & helix, physics::spring_joint_type & joint) : helix(&helix), joint(&joint) {}
		inline Connection() : helix(NULL), joint(NULL) {}
	};

	typedef std::array<Connection, 4> ConnectionContainer;

	enum AttachmentPoint {
		kForwardThreePrime = 0,
		kForwardFivePrime = 1,
		kBackwardThreePrime = 2,
		kBackwardFivePrime = 3,
		kNoAttachmentPoint = 4
	};

	struct settings_type {
		physics::real_type density, spring_stiffness, fixed_spring_stiffness, spring_damping;
		bool attach_fixed;
	};

	inline Helix(const settings_type & settings, physics & phys, int bases, const physics::transform_type & transform) : settings(settings), initialBases(bases), initialTransform(transform) {
		createRigidBody(phys, bases, transform);
	}

	void recreateRigidBody(physics & phys, int bases, const physics::transform_type & transform);

	inline physics::transform_type getTransform() const {
		return rigidBody->getGlobalPose();
	}

	inline void setTransform(const physics::transform_type & transform) {
		rigidBody->setGlobalPose(transform);
	}

	void attach(physics & phys, Helix & other, AttachmentPoint thisPoint, AttachmentPoint otherPoint);

	physics::real_type getSeparation(AttachmentPoint atPoint) const;

	inline unsigned int getBaseCount() const {
		return bases;
	}

	inline unsigned int getInitialBaseCount() const {
		return initialBases;
	}

	inline const physics::transform_type & getInitialTransform() const {
		return initialTransform;
	}

	inline const ConnectionContainer & getJoints() const {
		return joints;
	}

	inline const Connection & getJoint(AttachmentPoint point) const {
		return joints[point];
	}

	inline bool isSleeping() const {
		return rigidBody->isSleeping();
	}

private:
	void createRigidBody(physics & phys, int bases, const physics::transform_type & transform);
	void destroyRigidBody(physics & phys);

	AttachmentPoint otherPoint(AttachmentPoint point, const Helix & other) const;

	physics::rigid_body_type *rigidBody = NULL;
	physics::spring_joint_type *fixedJoint = NULL;

	ConnectionContainer joints = ConnectionContainer{ { Connection(), Connection(), Connection(), Connection() } }; // Index by AttachmentPoint.

	unsigned int bases;
	const unsigned int initialBases;
	const physics::transform_type initialTransform;

	const settings_type & settings;
};
}
#endif /* _HELIX_H_ */
