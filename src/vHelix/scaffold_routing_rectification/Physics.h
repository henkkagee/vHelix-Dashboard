#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <PxPhysicsAPI.h>

#include <cassert>
#include <initializer_list>
#include <tuple>

/*
 * All NVIDIA PhysX stuff should reside in here. For easy switching of physics engines in the future.
 */

#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425

class physics {
public:
	typedef physx::PxQuat quaternion_type;
	typedef physx::PxReal real_type;
	typedef physx::PxTransform transform_type;
	typedef physx::PxVec3 vec3_type;

	typedef physx::PxRigidDynamic rigid_body_type;
	typedef physx::PxDistanceJoint spring_joint_type;
    typedef physx::PxSphereGeometry sphere_geometry_type;
	typedef physx::PxCapsuleGeometry capsule_geometry_type;

	static const quaternion_type quaternion_identity;

	struct settings_type {
		real_type kStaticFriction, kDynamicFriction, kRestitution;
		real_type kRigidBodySleepThreshold;

		bool visual_debugger;
	};

	physics(const settings_type & settings); // throws exceptions.
	~physics();

	/*
	 * Creates a rigid body with the given geometries as variadic template arguments.
	 * For every geometry argument, std::get<0> will be used to get the PxGeometry, std::get<1> will be used to obtain the PxTransform. Use std::pair or std::tuple.
	 */
	template<typename GeometryT, typename... GeometryListT>
    rigid_body_type *create_rigid_body(const transform_type & transform, real_type density, const GeometryT & geometry, GeometryListT &... geometries) {
		physx::PxRigidDynamic *rigid_body(physx::PxCreateDynamic(*pxphysics, transform, std::get<0>(geometry), *material, density, std::get<1>(geometry)));
		assert(rigid_body != nullptr);

		rigid_body->setSleepThreshold(settings.kRigidBodySleepThreshold);

		internal_rigid_body_attach_geometry(*rigid_body, geometries...);

		scene->addActor(*rigid_body);
		return rigid_body;
	}

	sphere_geometry_type create_sphere_geometry(real_type radius);
	capsule_geometry_type create_x_axis_capsule(real_type radius, real_type half_height);

	spring_joint_type *create_spring_joint(rigid_body_type *rigid_body1, const transform_type & transform1, rigid_body_type *rigid_body2, const transform_type & transform2, real_type stiffness, real_type damping);

	inline void destroy_spring_joint(spring_joint_type *) {} // physx takes care of joints. Note that this does not detach the spring from the rigid body when called!
	inline void destroy_rigid_body(rigid_body_type *rigid_body) const { scene->removeActor(*rigid_body); rigid_body->release(); }

	inline static vec3_type transform(const transform_type & transform, const vec3_type & vec) {
		return physx::PxMat44(transform).transform(vec);
	}

	inline static transform_type world_transform(const rigid_body_type & rigid_body) {
		return rigid_body.getGlobalPose();
	}

public: // DEBUG make private.
	physx::PxDefaultAllocator allocator;
	physx::PxDefaultErrorCallback errorCallback;

	physx::PxScene *scene;
	physx::PxFoundation *foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics *pxphysics;

    physx::PxVisualDebuggerConnection *connection;
	physx::PxDefaultCpuDispatcher*	dispatcher;
	physx::PxMaterial *material = NULL;

	const settings_type settings;

private:

	template<typename GeometryT, typename... GeometryListT>
	inline void internal_rigid_body_attach_geometry(rigid_body_type & rigid_body, const GeometryT & geometry, GeometryListT &... geometries) {
		rigid_body.createShape(std::get<0>(geometry), *material)->setLocalPose(std::get<1>(geometry));
		internal_rigid_body_attach_geometry(rigid_body, geometries...);
	}

	inline void internal_rigid_body_attach_geometry(rigid_body_type & rigid_body) {}
};

#endif /* N _PHYSICS_H_ */
