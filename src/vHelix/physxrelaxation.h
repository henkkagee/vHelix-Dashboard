#ifndef PHYSXRELAXATION_H
#define PHYSXRELAXATION_H

#define _USE_MATH_DEFINES
#define NOMINMAX

#include <PxPhysicsAPI.h>
#include <cassert>
#include <initializer_list>
#include <tuple>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <numeric>
#include <math.h>
#include <set>
#include <list>

#include "relaxation.h"
#include "definitions.h"

//#include "scaffold_routing_rectification/physics.h"


#define PVD_HOST "127.0.0.1"
#define PVD_PORT 5425

namespace PhysXRelax {

namespace DNA {
    const double PITCH = 720.0 / 21.0,													// degrees
        STEP = 0.334,
        RADIUS = 1.0,
        SPHERE_RADIUS = 0.13,																// not dna properties, but visual
        OPPOSITE_ROTATION = 155.0;

    const double HALF_TURN_LENGTH = 180.0 / PITCH * STEP;
    const double FULL_TURN_LENGTH = 360.0 / PITCH * STEP;
    const int APPROXIMATION_RADIUS_MULTIPLIER = 4;										// when approximating the cylinder in PhysX, create these exaggerated end bases to compensate for the capsule shape, where their radius is this multiplier times the base RADIUS.
    const double RADIUS_PLUS_SPHERE_RADIUS = RADIUS + SPHERE_RADIUS;

    /*
     * Returns the rounded number of bases required to occupy the given distance.
     */
    template<typename T>
    int DistanceToBaseCount(T distance) {
        return int(std::floor(distance / T(STEP) + T(0.5)));
    }
    /*
     * Returns the length of a helix with the given number of bases.
     */
    inline double BasesToLength(int bases) {
        return bases * STEP;
    }

    /*
     * Returns the actual length a helix created to match the distance will actually have.
     */
    template<typename T>
    T HelixLength(T distance) {
        return BasesToLength(DistanceToBaseCount(distance));
    }

    inline double BasesToRotation(int bases) {
        return PITCH * bases;
    }

    /*
    * Returns the total rotation done by a with the target length.
    */
    template<typename T>
    T HelixRotation(T distance) {
        return T(BasesToRotation(DistanceToBaseCount(distance)));
    }
}


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

class scene {
    friend class SceneDescription;
public:
    typedef std::vector<Helix> HelixContainer;

    struct settings_type {
        physics::real_type initial_scaling;
        bool discretize_lengths;
    };

    inline scene(const settings_type & settings, const Helix::settings_type & helix_settings) : settings(settings), helix_settings(helix_settings) {}

    // Distinguish the file type by its file ending and parses either a rmesh or a ply file.
    inline bool read(physics & phys, const std::string & filename) {
        if (ends_with(filename, ".ply"))
            return read_ply(phys, std::ifstream(filename), std::ifstream(strip_trailing_string(filename, ".ply") + ".ntrail"));
        else
            return read_ply(phys, std::ifstream(strip_trailing_string(filename, ".ntrail") + ".ply"), std::ifstream(filename));
    }


    // Read a mesh using the .ply and .ntrail formats
    bool read_ply(physics &phys, std::istream & ply_file, std::istream & ntrail_file);

    void set_data(std::vector<coordinates> input_vertices, std::vector<unsigned int> trail) {
        path = trail;
        path.pop_back();

        std::cerr << "Printing the trail\n";
        for (int i = 0; i < path.size(); i++) {
            std::cout << path[i] << " ";
        }
        std::cout << "\n";
        vertices.reserve(input_vertices.size());
        std::cout << "Scaling = " << settings.initial_scaling << "\n";
        for (unsigned int i = 0; i < input_vertices.size(); i++) {
            std::cout << input_vertices[i].x << " " << input_vertices[i].y << " " << input_vertices[i].z << "\n";
            Vertex vertex(physics::vec3_type(input_vertices[i].x,input_vertices[i].y,input_vertices[i].z)*settings.initial_scaling);
            vertices.push_back(vertex);
        }
    }

    void set_sf_data(std::vector<coordinates> input_vertices, std::vector<std::vector<unsigned int>> input_trails) {
        trails = input_trails;
        for (int i = 0; i < trails.size(); i++) {
            trails[i].pop_back();
        }
        vertices.reserve(input_vertices.size());
        for (unsigned int i = 0; i < input_vertices.size(); i++) {
            Vertex vertex(physics::vec3_type(input_vertices[i].x,input_vertices[i].y,input_vertices[i].z)*settings.initial_scaling);
            vertices.push_back(vertex);
        }

    }
    void print_data() {
        std::cerr << "Printing the trails\n";
        for (int i = 0; i < trails.size(); i++) {
            for (int j = 0; j < trails[i].size(); j++) {
                std::cerr << trails[i][j] << " ";
            }
            std::cerr << "\n";
        }
    }
    inline HelixContainer & getHelices() {
        return helices;
    }
    //bool read_data(std::vector<coordinates> vertices, std::list<unsigned long long> nodetrail);

    inline HelixContainer::size_type getHelixCount() const {
        return helices.size();
    }

    // TODO: Move to SceneDescription when Scene can be constructed from a SceneDescription.
    inline physics::real_type getTotalSeparation() const {
        physics::real_type totalSeparation(0);
        for (const Helix & helix : helices)
            totalSeparation += std::accumulate(helix.getJoints().begin(), helix.getJoints().end(), physics::real_type(0), [](physics::real_type & separation, const Helix::Connection & connection) { return separation + (connection ? connection.joint->getDistance() : physics::real_type(0)); });

        return totalSeparation / 2;
    }

    // TODO: Move to SceneDescription when Scene...
    void getTotalSeparationMinMaxAverage(physics::real_type & min, physics::real_type & max, physics::real_type & average, physics::real_type & total) const;

    inline bool isSleeping() const {
        return std::find_if(helices.begin(), helices.end(), std::mem_fun_ref(&Helix::isSleeping)) != helices.end();
    }
   bool setupHelices(physics & phys);
   bool setupscaffoldfreeHelices(physics & phys);
private:


    struct Vertex {
        physics::vec3_type position, normal;
        struct Edge {
            unsigned int index;
            physics::real_type angle; // In RADIANS!

            inline explicit Edge(unsigned int index, physics::real_type angle = physics::real_type(0)) : index(index), angle(angle) {}
            inline bool operator< (const Edge & edge) const { return angle < edge.angle; }
            inline bool operator==(unsigned int index) const { return this->index == index; }
        };
        typedef std::vector<Edge> NeighborContainer;
        NeighborContainer neighbor_edges; // Edge indices. TODO: Change name to edges.
        std::unordered_map<unsigned int, NeighborContainer::const_iterator> edges_lookup;

        // TODO: Rename to optimize_edges or something more suitable.
        inline void generate_lookup() {
            //std::sort(neighbor_edges.begin(), neighbor_edges.end());

            edges_lookup.clear();
            edges_lookup.reserve(neighbor_edges.size());
            for (NeighborContainer::const_iterator it(neighbor_edges.begin()); it != neighbor_edges.end(); ++it)
                edges_lookup.insert(std::make_pair(it->index, it));
        }

        inline Vertex(const physics::vec3_type & position) : position(position) {}
    };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> path;
    std::vector<std::vector<unsigned int>> trails;
    HelixContainer helices;

    struct Edge {
        std::array<unsigned int, 2> vertices;

        static std::hash<unsigned int> hasher;

        // Visual C++ support for initializer_list is awful.
        inline Edge(unsigned int vertices_[2]) : vertices(std::array<unsigned int, 2> { { vertices_[0], vertices_[1] } }) {}
        inline Edge(unsigned int vertex1, unsigned int vertex2) : vertices(std::array<unsigned int, 2> { { vertex1, vertex2 } }) {}

        inline bool operator==(const Edge & e) const {
            //std::cout << "Comparing edges " << e.vertices[0] << " -> " << e.vertices[1] << " and " << vertices[0] << "-> " << vertices[1] << ". Returning " << ((e.vertices[0] == vertices[0] && e.vertices[1] == vertices[1]) || (e.vertices[0] == vertices[1] && e.vertices[1] == vertices[0])) <<"\n";
            //return ((e.vertices[0] == vertices[0] && e.vertices[1] == vertices[1]) || (e.vertices[0] == vertices[1] && e.vertices[1] == vertices[0]));
            return (e.vertices[0] == vertices[0] && e.vertices[1] == vertices[1]);
        }

        inline bool operator<(const Edge &e) const {
            if (vertices[0] == e.vertices[0]) {
                return vertices[1] < e.vertices[1];
            }
            else {
                return vertices[0] < e.vertices[0];

            }
        }

        inline std::hash<unsigned int>::result_type hash() const {
            return hasher(vertices[0]) + hasher(vertices[1]);
        }

        inline unsigned int other(unsigned int vertex) const {
            assert(vertex == vertices[0] || vertex == vertices[1]);
            return vertex == vertices[0] ? vertices[1] : vertices[0];
        }
    };

    std::vector<Edge> edges;

    const settings_type settings;
    const Helix::settings_type helix_settings;
};

/*
 * SceneDescription: Slightly more compact class describing a Scene. Generated by an arbitrary Scene, a SceneDescriptor can be written to a file.
 * It is used by the SimulatedAnnealing when a solution is found.
 */

class SceneDescription {
public:
    explicit SceneDescription(scene & scene);
    inline SceneDescription() : totalSeparation(0) {}
    SceneDescription(const SceneDescription & scene) = delete; // Only allow rvalue move operator. The lookup pointer table will not work if we allow copies.
    SceneDescription(SceneDescription && scene) { totalSeparation = scene.totalSeparation; helices = std::move(scene.helices); }
    SceneDescription & operator=(const SceneDescription & scene) = delete;
    inline SceneDescription & operator=(SceneDescription && scene) { totalSeparation = scene.totalSeparation; helices = std::move(scene.helices); return *this; }

    bool write(std::ostream & out) const;
    bool SceneDescription::write_2(rpoly& target) const;
    bool write_scaffold_free(std::ostream & out) const;
    bool SceneDescription::write_scaffold_free_2(rpoly& target) const;
    const physics::real_type getTotalSeparation() const {
        return totalSeparation;
    }

private:
    physics::real_type totalSeparation;

    struct Helix {
        std::array<Helix *, 4> connections;
        const unsigned int baseCount;
        const physics::transform_type transform;

        explicit inline Helix(unsigned int baseCount, const physics::transform_type & transform) : baseCount(baseCount), transform(transform) {}

        inline const physics::vec3_type & getPosition() const {
            return transform.p;
        }

        inline const physics::quaternion_type & getOrientation() const {
            return transform.q;
        }
    };

    std::vector<Helix> helices;
};

inline bool parse_settings(const double *dblArgs, const bool *boolArgs,physics::settings_type &physics_settings,scene::settings_type &scene_settings,Helix::settings_type &helix_settings) {

    //parse_settings::parse(argc, argv, physics_settings, scene_settings, helix_settings, input_file, output_file);
    scene_settings.initial_scaling = dblArgs[0];
    physics_settings.kRestitution = dblArgs[7];
    physics_settings.kStaticFriction = dblArgs[5];
    physics_settings.visual_debugger = boolArgs[2];
    physics_settings.kDynamicFriction = dblArgs[6];
    physics_settings.kRigidBodySleepThreshold = dblArgs[8];
    scene_settings.discretize_lengths = boolArgs[0];
    helix_settings.density = dblArgs[1];
    helix_settings.attach_fixed = boolArgs[1];
    helix_settings.spring_damping = dblArgs[4];
    helix_settings.spring_stiffness = dblArgs[2];
    helix_settings.fixed_spring_stiffness = dblArgs[3];
    return true;
}

/*
 * Does a simple rectification of the structure without modification.
 */

class PhysXRelaxation : public Relaxation
{


public:
    PhysXRelaxation(std::string &name,physics::settings_type &physics_settings, scene::settings_type &scene_settings, Helix::settings_type &helix_settings, const int &iterations, double timeout);
    int main();
    template<typename RunningFunctorT>
    SceneDescription simulated_rectification(RunningFunctorT running_functor) ; //not used
    template<typename StoreBestFunctorT, typename RunningFunctorT>
    void gradient_descent(int minbasecount, StoreBestFunctorT store_best_functor, RunningFunctorT running_functor, const int &iterations);
    void handle_exit() {
        running = false;
    }
    template<typename StoreBestFunctorT, typename RunningFunctorT>
    void simulated_annealing(int kmax, float emax, unsigned int minbasecount, int baserange,
            StoreBestFunctorT store_best_functor, RunningFunctorT running_functor); // not used
    inline void optim_wrapper(SceneDescription &best_scene, bool &hasresult);

    int scaffold_main(std::vector<coordinates> &vertices, std::vector<unsigned int> &nodetrail);
    int scaffold_main2(std::vector<coordinates> &vertices, std::vector<unsigned int> &nodetrail, rpoly &rpoly, bool &hasresult);
    int scaffold_free_main(std::vector<coordinates> &vertices, std::vector<std::vector<unsigned int>> &nodetrail);
    int scaffold_free_main2(std::vector<coordinates> &vertices, std::vector<std::vector<unsigned int>> &nodetrail, rpoly &rpoly);

protected:
    scene mesh;
    physics phys;
    bool running;
    int iterations;
    double timeout;
};
}
#endif // PHYSXRELAXATION_H
