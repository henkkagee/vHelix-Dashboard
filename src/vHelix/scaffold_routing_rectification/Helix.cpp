#include "Definition.h"
#include "DNA.h"
#include "Helix.h"

#include <cassert>
#include <initializer_list>

using namespace Phys;

physics::vec3_type localFrame(Helix::AttachmentPoint point, int bases) {
	const physics::vec3_type offset(0, 0, physics::real_type(DNA::BasesToLength(bases) / 2));

	switch (point) {
	case Helix::kForwardThreePrime:
		return physics::quaternion_type(physics::real_type(toRadians(DNA::BasesToRotation(bases))), kPosZAxis).rotate(physics::vec3_type(0, physics::real_type(DNA::RADIUS), 0)) + offset;
	case Helix::kForwardFivePrime:
		return physics::vec3_type(0, physics::real_type(DNA::RADIUS), 0) - offset;
	case Helix::kBackwardThreePrime:
		return physics::quaternion_type(physics::real_type(toRadians(-DNA::OPPOSITE_ROTATION)), kPosZAxis).rotate(physics::vec3_type(0, physics::real_type(DNA::RADIUS), 0)) - offset;
	case Helix::kBackwardFivePrime:
		return physics::quaternion_type(physics::real_type(toRadians(DNA::BasesToRotation(bases) - DNA::OPPOSITE_ROTATION)), kPosZAxis).rotate(physics::vec3_type(0, physics::real_type(DNA::RADIUS), 0)) + offset;
	default:
		assert(0);
		return physics::vec3_type();
	}
}

std::initializer_list<Helix::AttachmentPoint> opposites(Helix::AttachmentPoint point) {
	switch (point) {
	case Helix::kForwardThreePrime:
	case Helix::kBackwardThreePrime:
		return{ Helix::kForwardFivePrime, Helix::kBackwardFivePrime };
	case Helix::kForwardFivePrime:
	case Helix::kBackwardFivePrime:
		return { Helix::kForwardThreePrime, Helix::kBackwardThreePrime };
	default:
		assert(0);
		return{ Helix::kNoAttachmentPoint, Helix::kNoAttachmentPoint };
	}
}

void Helix::createRigidBody(physics & phys, int bases, const physics::transform_type & transform) {
	assert(rigidBody == nullptr);

	const physics::real_type length(physics::real_type(DNA::BasesToLength(bases)));
	//assert(length > DNA::RADIUS_PLUS_SPHERE_RADIUS * 2);
	if (length <= 0/*DNA::RADIUS_PLUS_SPHERE_RADIUS * 2*/)
		throw std::runtime_error("Helix length is too short. Rescale the structure so that the length of the structure is at least the diameter of the cylinder approximation. This is because PhysX forces us to approximate helices as capsules.");

	const physics::real_type radius(physics::real_type(DNA::SPHERE_RADIUS * DNA::APPROXIMATION_RADIUS_MULTIPLIER));
	const physics::real_type offset(physics::real_type(DNA::RADIUS - radius + DNA::SPHERE_RADIUS));

	const physics::sphere_geometry_type sphereGeometry(radius);
	if (length > DNA::RADIUS_PLUS_SPHERE_RADIUS * 2) {
		rigidBody = phys.create_rigid_body(transform, settings.density,
			std::make_pair(physics::capsule_geometry_type(physics::real_type(DNA::RADIUS_PLUS_SPHERE_RADIUS), length / 2 - physics::real_type(DNA::RADIUS_PLUS_SPHERE_RADIUS)), physics::transform_type(physics::quaternion_type(physics::real_type(M_PI_2), kNegYAxis))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::vec3_type(0, offset, -length / 2 + radius))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::quaternion_type(physics::real_type(toRadians(-DNA::OPPOSITE_ROTATION)), physics::vec3_type(0, 0, 1)).rotate(physics::vec3_type(0, offset, -length / 2 + radius)))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::quaternion_type(physics::real_type(toRadians(DNA::PITCH * bases)), physics::vec3_type(0, 0, 1)).rotate(physics::vec3_type(0, offset, length / 2 - radius)))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::quaternion_type(physics::real_type(toRadians(DNA::PITCH * bases - DNA::OPPOSITE_ROTATION)), physics::vec3_type(0, 0, 1)).rotate(physics::vec3_type(0, offset, length / 2 - radius)))));
	} else {
		rigidBody = phys.create_rigid_body(transform, settings.density,
			std::make_pair(sphereGeometry, physics::transform_type(physics::vec3_type(0, offset, -length / 2 + radius))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::quaternion_type(physics::real_type(toRadians(-DNA::OPPOSITE_ROTATION)), physics::vec3_type(0, 0, 1)).rotate(physics::vec3_type(0, offset, -length / 2 + radius)))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::quaternion_type(physics::real_type(toRadians(DNA::PITCH * bases)), physics::vec3_type(0, 0, 1)).rotate(physics::vec3_type(0, offset, length / 2 - radius)))),
			std::make_pair(sphereGeometry, physics::transform_type(physics::quaternion_type(physics::real_type(toRadians(DNA::PITCH * bases - DNA::OPPOSITE_ROTATION)), physics::vec3_type(0, 0, 1)).rotate(physics::vec3_type(0, offset, length / 2 - radius)))));
	}
	assert(rigidBody != nullptr);

	if (settings.attach_fixed) {
		fixedJoint = phys.create_spring_joint(rigidBody, physics::transform_type(kZeroVec), NULL, transform, settings.fixed_spring_stiffness, settings.spring_damping);
		assert(rigidBody != nullptr);
	}

	this->bases = bases;
}

void Helix::destroyRigidBody(physics & phys) {
	phys.destroy_rigid_body(rigidBody);
	rigidBody = NULL;
	fixedJoint = NULL;
}

void Helix::recreateRigidBody(physics & phys, int bases, const physics::transform_type & transform) {
	const std::array< std::tuple<Helix *, AttachmentPoint, AttachmentPoint>, 4 > points{ { 
		std::make_tuple(joints[kForwardThreePrime].helix, kForwardThreePrime, otherPoint(kForwardThreePrime, *joints[kForwardThreePrime].helix)),
		std::make_tuple(joints[kForwardFivePrime].helix, kForwardFivePrime, otherPoint(kForwardFivePrime, *joints[kForwardFivePrime].helix)),
		std::make_tuple(joints[kBackwardThreePrime].helix, kBackwardThreePrime, otherPoint(kBackwardThreePrime, *joints[kBackwardThreePrime].helix)),
		std::make_tuple(joints[kBackwardFivePrime].helix, kBackwardFivePrime, otherPoint(kBackwardFivePrime, *joints[kBackwardFivePrime].helix))
	} };

	destroyRigidBody(phys);
	createRigidBody(phys, bases, transform);

	for (int i = 0; i < 4; ++i)
		attach(phys, *std::get<0>(points[i]), std::get<1>(points[i]), std::get<2>(points[i]));
}

void Helix::attach(physics & phys, Helix & other, AttachmentPoint thisPoint, AttachmentPoint otherPoint) {
	assert(rigidBody);
	//assert(joints[thisPoint].helix == nullptr && joints[thisPoint].joint == nullptr && other.joints[otherPoint].helix == nullptr && other.joints[otherPoint].joint == nullptr);

	physics::spring_joint_type *joint(phys.create_spring_joint(rigidBody, physics::transform_type(localFrame(thisPoint, bases)), other.rigidBody, physics::transform_type(localFrame(otherPoint, other.bases)), settings.spring_stiffness, settings.spring_damping));
	assert(joint);

	joints[thisPoint].helix = &other;
	joints[thisPoint].joint = joint;
	other.joints[otherPoint].helix = this;
	other.joints[otherPoint].joint = joint;
}

physics::real_type Helix::getSeparation(AttachmentPoint atPoint) const {
	const physics::vec3_type attachmentPoint0(physics::transform(physics::world_transform(*rigidBody), localFrame(atPoint, bases)));
	const Helix & other(*joints[atPoint].helix);
	const physics::vec3_type attachmentPoint1(physics::transform(physics::world_transform(*other.rigidBody), localFrame(otherPoint(atPoint, other), other.bases)));
	return (attachmentPoint1 - attachmentPoint0).magnitude();
}

Helix::AttachmentPoint Helix::otherPoint(Helix::AttachmentPoint point, const Helix & other) const {
	for (Helix::AttachmentPoint otherPoint : opposites(point)) {
		if (other.joints[otherPoint].helix == this)
			return otherPoint;
	}

	assert(0);
	return kNoAttachmentPoint;
}
