#include "Utility.h"
#include "Physics.h"

const physics::quaternion_type physics::quaternion_identity(physx::PxIdentity);

physics::physics(const settings_type & settings) : settings(settings) {
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
	profileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(foundation);
	pxphysics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, profileZoneManager);

	if (pxphysics->getPvdConnectionManager()) {
        std::cout << "Got pvd connection manager" << std::endl;
		pxphysics->getVisualDebugger()->setVisualizeConstraints(true);
		pxphysics->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		pxphysics->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		pxphysics->getVisualDebugger()->updateCamera("default", physx::PxVec3(0, 0, -60), physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 0));
		connection = physx::PxVisualDebuggerExt::createConnection(pxphysics->getPvdConnectionManager(), PVD_HOST, PVD_PORT, physx::PxVisualDebuggerConnectionFlag::eDEBUG);
    } else {
        PRINT("Did not get pvd connection manager\n");
		connection = nullptr;
    }
	physx::PxSceneDesc sceneDesc(pxphysics->getTolerancesScale());
	PRINT("This CPU has %u cores.", numcpucores());
	dispatcher = physx::PxDefaultCpuDispatcherCreate(numcpucores());
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	scene = pxphysics->createScene(sceneDesc);

	material = pxphysics->createMaterial(settings.kStaticFriction, settings.kDynamicFriction, settings.kRestitution); // What about deletion upon destruction?
}

physics::~physics() {
	scene->release();
	dispatcher->release();
	if (connection)
		connection->release();
	pxphysics->release();
	profileZoneManager->release();
	foundation->release();
}

physics::sphere_geometry_type physics::create_sphere_geometry(real_type radius) {
	return physx::PxSphereGeometry(radius);
}

physics::capsule_geometry_type physics::create_x_axis_capsule(real_type radius, real_type half_height) {
	return physx::PxCapsuleGeometry(radius, half_height);
}

physics::spring_joint_type *physics::create_spring_joint(rigid_body_type *rigid_body1, const transform_type & transform1, rigid_body_type *rigid_body2, const transform_type & transform2, real_type stiffness, real_type damping) {
	physx::PxDistanceJoint *joint(physx::PxDistanceJointCreate(*pxphysics, rigid_body1, transform1, rigid_body2, transform2));
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, true);
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
	joint->setStiffness(stiffness);
	joint->setDamping(damping);

	return joint;
}
