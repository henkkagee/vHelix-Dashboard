#include "physxrelaxation.h"

using namespace PhysXRelax;

const PhysXRelax::physics::quaternion_type PhysXRelax::physics::quaternion_identity(physx::PxIdentity);

PhysXRelax::physics::physics(const PhysXRelax::physics::settings_type & settings) : settings(settings) {
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
        //PRINT("Did not get pvd connection manager\n");
        connection = nullptr;
    }
    physx::PxSceneDesc sceneDesc(pxphysics->getTolerancesScale());
    //PRINT("This CPU has %u cores.", numcpucores());
    dispatcher = physx::PxDefaultCpuDispatcherCreate(numcpucores());
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

    scene = pxphysics->createScene(sceneDesc);

    material = pxphysics->createMaterial(settings.kStaticFriction, settings.kDynamicFriction, settings.kRestitution); // What about deletion upon destruction?
}

PhysXRelax::physics::~physics() {
    scene->release();
    dispatcher->release();
    if (connection)
        connection->release();
    pxphysics->release();
    profileZoneManager->release();
    foundation->release();
}

PhysXRelax::physics::sphere_geometry_type PhysXRelax::physics::create_sphere_geometry(PhysXRelax::physics::real_type radius) {
    return physx::PxSphereGeometry(radius);
}

PhysXRelax::physics::capsule_geometry_type PhysXRelax::physics::create_x_axis_capsule(real_type radius, real_type half_height) {
    return physx::PxCapsuleGeometry(radius, half_height);
}

PhysXRelax::physics::spring_joint_type *PhysXRelax::physics::create_spring_joint(rigid_body_type *rigid_body1, const transform_type & transform1, rigid_body_type *rigid_body2, const transform_type & transform2, real_type stiffness, real_type damping) {
    physx::PxDistanceJoint *joint(physx::PxDistanceJointCreate(*pxphysics, rigid_body1, transform1, rigid_body2, transform2));
    joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, true);
    joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
    joint->setStiffness(stiffness);
    joint->setDamping(damping);

    return joint;
}

PhysXRelax::physics::quaternion_type PhysXRelax::rotationFromTo(PhysXRelax::physics::vec3_type from, PhysXRelax::physics::vec3_type to) {
    // Based on Stan Melax's article in Game Programming Gems
    from.normalize();
    to.normalize();

    const PhysXRelax::physics::real_type d = from.dot(to);
    // If dot == 1, vectors are the same
    if (d >= PhysXRelax::physics::real_type(1.0))
        return PhysXRelax::physics::quaternion_identity;
    else if (d < PhysXRelax::physics::real_type(1e-6 - 1.0)) {
        // Generate an axis
        PhysXRelax::physics::vec3_type axis = kPosXAxis.cross(from);
        if (axis.magnitudeSquared() <= PhysXRelax::physics::real_type(1e-6)) // pick another if colinear
            axis = kPosYAxis.cross(from);
        return PhysXRelax::physics::quaternion_type(PhysXRelax::physics::real_type(M_PI), axis.getNormalized());
    }
    else {
        PhysXRelax::physics::quaternion_type q;
        const PhysXRelax::physics::real_type s = std::sqrt((1 + d) * 2);
        const PhysXRelax::physics::real_type invs = 1 / s;
        const PhysXRelax::physics::vec3_type c(from.cross(to));

        q.x = c.x * invs;
        q.y = c.y * invs;
        q.z = c.z * invs;
        q.w = s / 2;
        return q.getNormalized();
    }
}

PhysXRelax::physics::vec3_type localFrame(Helix::AttachmentPoint point, int bases) {
    const PhysXRelax::physics::vec3_type offset(0, 0, PhysXRelax::physics::real_type(PhysXRelax::DNA::BasesToLength(bases) / 2));

    switch (point) {
    case Helix::kForwardThreePrime:
        return PhysXRelax::physics::quaternion_type(PhysXRelax::physics::real_type(toRadians(PhysXRelax::DNA::BasesToRotation(bases))), kPosZAxis).rotate(PhysXRelax::physics::vec3_type(0, PhysXRelax::physics::real_type(PhysXRelax::DNA::RADIUS), 0)) + offset;
    case Helix::kForwardFivePrime:
        return PhysXRelax::physics::vec3_type(0, PhysXRelax::physics::real_type(PhysXRelax::DNA::RADIUS), 0) - offset;
    case Helix::kBackwardThreePrime:
        return PhysXRelax::physics::quaternion_type(PhysXRelax::physics::real_type(toRadians(-PhysXRelax::DNA::OPPOSITE_ROTATION)), kPosZAxis).rotate(PhysXRelax::physics::vec3_type(0, PhysXRelax::physics::real_type(PhysXRelax::DNA::RADIUS), 0)) - offset;
    case Helix::kBackwardFivePrime:
        return PhysXRelax::physics::quaternion_type(PhysXRelax::physics::real_type(toRadians(PhysXRelax::DNA::BasesToRotation(bases) - PhysXRelax::DNA::OPPOSITE_ROTATION)), kPosZAxis).rotate(PhysXRelax::physics::vec3_type(0, PhysXRelax::physics::real_type(PhysXRelax::DNA::RADIUS), 0)) + offset;
    default:
        assert(0);
        return PhysXRelax::physics::vec3_type();
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


void Helix::createRigidBody(PhysXRelax::physics & phys, int bases, const PhysXRelax::physics::transform_type & transform) {
    assert(rigidBody == nullptr);

    const physics::real_type length(physics::real_type(DNA::BasesToLength(bases)));
    //assert(length > DNA::RADIUS_PLUS_SPHERE_RADIUS * 2);
    if (length <= 0/*DNA::RADIUS_PLUS_SPHERE_RADIUS * 2*/) {
        std::cerr << "Helix length = " << length << ". In bases: " << bases << "\n";
        throw std::runtime_error("Helix length is too short. Rescale the structure so that the length of the structure is at least the diameter of the cylinder approximation. This is because PhysX forces us to approximate helices as capsules.");
    }
    const physics::real_type radius(physics::real_type(DNA::SPHERE_RADIUS * DNA::APPROXIMATION_RADIUS_MULTIPLIER));
    const physics::real_type offset(physics::real_type(DNA::RADIUS - radius + DNA::SPHERE_RADIUS));

    const physics::sphere_geometry_type sphereGeometry(radius);
    if (length > DNA::RADIUS_PLUS_SPHERE_RADIUS * 2) {
        rigidBody = phys.create_rigid_body(transform, settings.density,
            std::make_pair(physics::capsule_geometry_type(physics::real_type(DNA::RADIUS_PLUS_SPHERE_RADIUS), length / 2 - physics::real_type(DNA::RADIUS_PLUS_SPHERE_RADIUS)),
            physics::transform_type(physics::quaternion_type(physics::real_type(M_PI_2), kNegYAxis))),
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

void Helix::destroyRigidBody(PhysXRelax::physics & phys) {
    phys.destroy_rigid_body(rigidBody);
    rigidBody = NULL;
    fixedJoint = NULL;
}

void Helix::recreateRigidBody(PhysXRelax::physics & phys, int bases, const PhysXRelax::physics::transform_type & transform) {
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

void Helix::attach(PhysXRelax::physics & phys, Helix & other, AttachmentPoint thisPoint, AttachmentPoint otherPoint) {
    assert(rigidBody);
    //assert(joints[thisPoint].helix == nullptr && joints[thisPoint].joint == nullptr && other.joints[otherPoint].helix == nullptr && other.joints[otherPoint].joint == nullptr);

    physics::spring_joint_type *joint(phys.create_spring_joint(rigidBody, physics::transform_type(localFrame(thisPoint, bases)), other.rigidBody, physics::transform_type(localFrame(otherPoint, other.bases)), settings.spring_stiffness, settings.spring_damping));
    assert(joint);

    joints[thisPoint].helix = &other;
    joints[thisPoint].joint = joint;
    other.joints[otherPoint].helix = this;
    other.joints[otherPoint].joint = joint;
}

PhysXRelax::physics::real_type Helix::getSeparation(AttachmentPoint atPoint) const {
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

bool scene::read_ply(PhysXRelax::physics & phys, std::istream & ply_file, std::istream & ntrail_file) {
    std::string line;
    unsigned int count(0), num_vertices(0);
    bool has_vertex_header(false);
    unsigned int pre_vertices_rows(0);
    physics::vec3_type vertex;

    while (ply_file.good()) {
        std::getline(ply_file, line);

        if (sscanf(line.c_str(), "element vertex %u", &num_vertices) == 1) {
            vertices.reserve(num_vertices);
            has_vertex_header = true;
        } else if (sscanf(line.c_str(), "element %*s %u", &count) == 1) {
            if (!has_vertex_header)
                pre_vertices_rows += count;
        } else if (line.compare(0, strlen("end_header"), "end_header") == 0) {
            for (unsigned int i = 0; i < pre_vertices_rows; ++i)
                std::getline(ply_file, line);
        } else if (sscanf(line.c_str(), "%f %f %f", &vertex.x, &vertex.y, &vertex.z) == 3 && vertices.size() < num_vertices) {
            vertex *= settings.initial_scaling;
            vertices.push_back(vertex);
        }
    }

    std::copy(std::istream_iterator<unsigned int>(ntrail_file), std::istream_iterator<unsigned int>(), std::back_inserter(path));
    path.pop_back(); // Because .ntrail stores the last and the first as the same index, this is implied in the rectification algorithm.

    return setupHelices(phys);
}

std::hash<unsigned int> scene::Edge::hasher;



bool scene::setupHelices(PhysXRelax::physics & phys) {
    // Find duplicate edges.
    std::cerr << "Setting up helices"<< std::endl;
    std::cerr << vertices.size()<< std::endl;
    std::cerr << path.size()<< std::endl;
    std::unordered_map<Edge, int, std::function<size_t(const Edge &)>> duplicates(path.size() - 1, std::mem_fun_ref(&Edge::hash));

    for (std::vector<unsigned int>::const_iterator it(path.begin()); it != path.end(); ++it) {
        ++duplicates[Edge(*it, *circular_increment(it, path))];
        edges.push_back(Edge(*it, *circular_increment(it, path)));
    }
    std::cerr << "Found duplicates"<< std::endl;
    for (std::vector<Edge>::const_iterator it(edges.begin()); it != edges.end(); ++it) {
        for (int i = 0; i < 2; ++i)
            vertices[it->vertices[i]].neighbor_edges.push_back(Vertex::Edge((unsigned int)std::distance(edges.cbegin(), it)));
    }
    std::cerr << "Neighbors set"<< std::endl;
    // Calculate vertex normal and edge angles.
    for (std::vector<Vertex>::iterator vertex_it(vertices.begin()); vertex_it != vertices.end(); ++vertex_it) {
        Vertex & vertex(*vertex_it);
        const unsigned int vertexIndex((unsigned int) std::distance(vertices.begin(), vertex_it));

        vertex.normal = kZeroVec;
        for (Vertex::NeighborContainer::const_iterator it(vertex.neighbor_edges.begin()); it != vertex.neighbor_edges.end(); ++it) {
            const physics::vec3_type edge1(vertex.position - vertices[edges[circular_increment(it, vertex.neighbor_edges)->index].other(vertexIndex)].position);
            const physics::vec3_type edge2(vertex.position - vertices[edges[it->index].other(vertexIndex)].position);
            vertex.normal += edge1.getNormalized().cross(edge2.getNormalized());
        }

        vertex.normal.normalize();

        //std::cerr << "vertex " << vertexIndex << " position: " << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z << std::endl;

        physics::vec3_type tangent((vertices[edges[vertex.neighbor_edges.front().index].other(vertexIndex)].position - vertex.position).getNormalized());
        tangent -= proj(tangent, vertex.normal);

        for (Vertex::Edge & edge : vertex.neighbor_edges) {
            const physics::vec3_type delta(vertices[edges[edge.index].other(vertexIndex)].position - vertex.position);
            //edge.angle = signedAngle(delta - proj(delta, vertex.normal), tangent, vertex.normal);
            edge.angle = signedAngle(tangent, delta - proj(delta, vertex.normal), vertex.normal);
        }
    }
    std::cerr << "Calculates vertex normals"<< std::endl;

    // Correct angles for edges visited multiple times.
    for (std::vector<unsigned int>::const_iterator it(path.begin()); it != path.end(); ++it) {
        const std::vector<unsigned int>::const_iterator next_it(circular_increment(it, path));
        const Edge edge(*it, *next_it);
        const unsigned int it_offset((unsigned int) std::distance(path.cbegin(), it));
        assert(duplicates[edge] >= 1);

        if (duplicates[edge] > 1) {
            Vertex *vertices_[] = { &vertices[edge.vertices[0]], &vertices[edge.vertices[1]] };

            const physics::vec3_type direction(vertices_[1]->position - vertices_[0]->position);

            const unsigned int connecting_vertex_indices[] = { *circular_increment(next_it, path), *circular_decrement(it, path) };

            physics::vec3_type tangent(kZeroVec);
            for (int i = 0; i < 2; ++i) {
                if (connecting_vertex_indices[i] != edge.vertices[i]) {
                    const physics::vec3_type upcoming_direction(vertices[connecting_vertex_indices[i]].position - vertices_[i ^ 1]->position);
                    tangent += upcoming_direction - proj(upcoming_direction, direction);
                }
            }

            const Vertex::NeighborContainer::iterator neighbor_edge_it(std::find(vertices_[0]->neighbor_edges.begin(), vertices_[0]->neighbor_edges.end(), it_offset));
            assert(neighbor_edge_it != vertices_[0]->neighbor_edges.end());

            //neighbor_edge_it->angle += sgn_nozero(direction.cross(vertices_[0]->normal).dot(tangent)) * physics::real_type(ANGLE_EPSILON);
            //neighbor_edge_it->angle -= sgn_nozero(direction.cross(vertices_[0]->normal).dot(tangent)) * physics::real_type(ANGLE_EPSILON);
        }
    }
    std::cerr << "Corrected angles"<< std::endl;

    // Validate angles for debugging
    for (const Edge & edge : edges) {
        const Vertex & vertex(vertices[edge.vertices[1]]);
        for (Vertex::NeighborContainer::const_iterator nit(vertex.neighbor_edges.begin()); nit != vertex.neighbor_edges.end() - 1; ++nit) {
            //if (nit->angle == (nit + 1)->angle)
                //PRINT("Angles are the same for edges %u and %u at vertex %u leading to vertices %u and %u. Vertex normal: %f, %f, %f", 1 + nit->index, 1 + (nit + 1)->index, 1 + edge.vertices[1], 1 + edges[nit->index].other(edge.vertices[1]), 1 + edges[(nit + 1)->index].other(edge.vertices[1]), vertices[edge.vertices[1]].normal.x, vertices[edge.vertices[1]].normal.y, vertices[edge.vertices[1]].normal.z);
            //assert(nit->angle != (nit + 1)->angle);
        }
    }

    for (const Vertex & vertex : vertices)
        assert(vertex.neighbor_edges.size() % 2 == 0);
    std::cerr << "Assertions"<< std::endl;
    // Sort edges by angle and create the edge lookup.
    std::for_each(vertices.begin(), vertices.end(), std::mem_fun_ref(&Vertex::generate_lookup));

    for (std::vector<unsigned int>::const_iterator it(path.begin()); it != path.end(); ++it) {
        const std::vector<unsigned int>::const_iterator next_it(circular_increment(it, path));
        const Edge edge(*it, *next_it);
        Vertex *vertices_[] = { &vertices[edge.vertices[0]], &vertices[edge.vertices[1]] };
        const Vertex::NeighborContainer::iterator neighbor_edge_it(std::find_if(vertices_[0]->neighbor_edges.begin(), vertices_[0]->neighbor_edges.end(), [this, &edge](const Vertex::Edge & vertex_edge) { return edges[vertex_edge.index].other(edge.vertices[0]) == edge.vertices[1]; }));
        assert(neighbor_edge_it != vertices_[0]->neighbor_edges.end());

        const physics::vec3_type origo((vertices_[0]->position + vertices_[1]->position) / 2), direction(vertices_[1]->position - vertices_[0]->position);

        std::set<unsigned int> vertices_unique_neighbors[2];
        for (int i = 0; i < 2; ++i) {
            for (const Vertex::Edge & vertex_edge : vertices_[i]->neighbor_edges)
                vertices_unique_neighbors[i].insert(edges[vertex_edge.index].other(edge.vertices[i]));
        }

        physics::vec3_type tangent(kZeroVec);
        const unsigned int connecting_vertex_indices[] = { *circular_increment(next_it, path), *circular_decrement(it, path) };
        bool cross(false);

        if (duplicates[edge] > 1) {
            for (int i = 0; i < 2; ++i) {
                if (connecting_vertex_indices[i] != edge.vertices[i]) {
                    //const physics::vec3_type upcoming_direction(vertices[connecting_vertex_indices[i]].position - vertices_[i ^ 1]->position);
                    //tangent += upcoming_direction - proj(upcoming_direction, direction);
                    // Fix for neighbouring edges with angle larger than 180 between them, e.g. as in triangular flat sheet with extra protruding traingles on one end
                    /*physics::real_type alpha = 1000, betha = 1000, gamma;
                    physics::real_type diff_ang;*/
                    //edges[std::distance<std::vector<unsigned int>::const_iterator>(path.begin(), it)].;
                    /*for (Vertex::NeighborContainer::const_iterator nit(vertices_[1]->neighbor_edges.begin()); nit != vertices_[1]->neighbor_edges.end(); ++nit) {
                        if (nit->index == std::distance<std::vector<unsigned int>::const_iterator>(path.begin(), it))
                            alpha = nit->angle;
                        if (nit->index == std::distance<std::vector<unsigned int>::const_iterator>(path.begin(), it) + 1)
                            betha = nit->angle;
                        if (alpha != 1000 && betha != 1000 && nit->angle != alpha && nit->angle != betha)
                        {
                            gamma = nit->angle;
                            break;
                        }
                    }

                    if ((alpha >= betha  && betha > gamma) || (alpha >= betha && gamma > alpha))
                        diff_ang = alpha - betha;
                    else if ((betha > alpha && alpha > gamma) || (betha > alpha && gamma > betha))
                        diff_ang = betha - alpha;
                    else if (alpha > gamma && gamma > betha)
                        diff_ang = (M_PI - alpha) + (betha + M_PI);
                    else if (betha > gamma && gamma > alpha)
                        diff_ang = (M_PI - betha) + (alpha + M_PI);

                    if (diff_ang >= M_PI)
                        tangent *= -1;*/

                } else
                    cross = true;
            }
            physics::real_type alpha, betha, gamma, diff_ang;
            for (Vertex::NeighborContainer::const_iterator nit(vertices_[0]->neighbor_edges.begin()); nit != vertices_[0]->neighbor_edges.end(); ++nit) {
                if (nit->index == std::distance<std::vector<unsigned int>::const_iterator>(path.begin(), it))
                {
                    alpha = nit->angle;

                    if (edges[circular_decrement(nit, vertices_[0]->neighbor_edges)->index].other(*it) != edges[nit->index].other(*it))
                    {
                        betha = circular_decrement(nit, vertices_[0]->neighbor_edges)->angle;
                        gamma = circular_increment(circular_increment(nit, vertices_[0]->neighbor_edges), vertices_[0]->neighbor_edges)->angle;
                        tangent = (vertices[edges[circular_decrement(nit, vertices_[0]->neighbor_edges)->index].vertices[0]].position
                            + vertices[edges[circular_decrement(nit, vertices_[0]->neighbor_edges)->index].vertices[1]].position) / 2 - origo;

                    }
                    else if (edges[circular_increment(nit, vertices_[0]->neighbor_edges)->index].other(*it) != edges[nit->index].other(*it))
                    {
                        betha = circular_increment(nit, vertices_[0]->neighbor_edges)->angle;
                        gamma = circular_decrement(circular_decrement(nit, vertices_[0]->neighbor_edges), vertices_[0]->neighbor_edges)->angle;
                        tangent = (vertices[edges[circular_increment(nit, vertices_[0]->neighbor_edges)->index].vertices[0]].position
                            + vertices[edges[circular_increment(nit, vertices_[0]->neighbor_edges)->index].vertices[1]].position) / 2 - origo;

                    }

                    if (alpha > gamma && gamma > betha) diff_ang = (M_PI - alpha) + (betha + M_PI);
                    else if (betha > gamma && gamma > alpha) diff_ang = (M_PI - betha) + (alpha + M_PI);
                    else  diff_ang = (betha - alpha) > 0 ? (betha - alpha) : alpha - betha;
                    if (diff_ang >= M_PI)
                        tangent *= -1;

                    break;
                }

            }

            tangent.normalize();
        } else
            cross = (vertices[connecting_vertex_indices[0]].position - vertices_[1]->position).dot(vertices[connecting_vertex_indices[1]].position - vertices_[0]->position) < 0;

        double length(direction.magnitude() - apothem(2 * DNA::RADIUS, vertices_unique_neighbors[0].size()) - apothem(2 * DNA::RADIUS, vertices_unique_neighbors[1].size()));

        if (settings.discretize_lengths) {
            const double num_half_turns(length / DNA::HALF_TURN_LENGTH);
            if (int(std::floor(num_half_turns)) % 2)
                length = DNA::HALF_TURN_LENGTH * (cross ? std::floor(num_half_turns) : std::ceil(num_half_turns));
            else
                length = DNA::HALF_TURN_LENGTH * (cross ? std::ceil(num_half_turns) : std::floor(num_half_turns));
        }

        helices.emplace_back(
            helix_settings,
            phys, DNA::DistanceToBaseCount(length),
            physics::transform_type((origo +  tangent * physics::real_type((duplicates[edge] - 1) * (DNA::RADIUS + DNA::SPHERE_RADIUS))), rotationFromTo(kPosZAxis, direction)));
    }
    std::cout <<"Sorted\n";
    // Connect the scaffold.
    for (HelixContainer::iterator it(helices.begin()); it != helices.end(); ++it)
        circular_decrement(it, helices)->attach(phys, *it, Helix::kForwardThreePrime, Helix::kForwardFivePrime);
    std::cout << "Scaffold connected\n";
    // Connect the staples.

    for (std::vector<Edge>::const_iterator it(edges.begin()); it != edges.end(); ++it) {
        const Vertex & vertex(vertices[it->vertices[1]]);
        const unsigned int it_offset((unsigned int)std::distance(edges.cbegin(), it));
        const unsigned int next_it_offset((unsigned int)circular_index(it_offset + 1, edges.size()));
        const Vertex::NeighborContainer::const_iterator edge_it(vertex.edges_lookup.at(it_offset));
        const Vertex::NeighborContainer::const_iterator next_edge_it(vertex.edges_lookup.at(next_it_offset));
        assert(edge_it != vertex.neighbor_edges.end() && next_edge_it != vertex.neighbor_edges.end());

        for (Vertex::NeighborContainer::const_iterator nit(vertex.neighbor_edges.begin()); nit != vertex.neighbor_edges.end() - 1; ++nit) {
            //if (nit->angle == (nit + 1)->angle)
                //PRINT("Angles are the same for edges %u and %u at vertex %u leading to vertices %u and %u. Vertex normal: %f, %f, %f", 1 + nit->index, 1 + (nit + 1)->index, 1 + it->vertices[1], 1 + edges[nit->index].other(it->vertices[1]), 1 + edges[(nit + 1)->index].other(it->vertices[1]), vertices[it->vertices[1]].normal.x, vertices[it->vertices[1]].normal.y, vertices[it->vertices[1]].normal.z);
            //assert(nit->angle != (nit + 1)->angle);
        }

        const int delta(int(next_edge_it - edge_it));
        assert(std::abs(delta) == 1 || std::abs(delta) == int(vertex.neighbor_edges.size() - 1));
        const ptrdiff_t edge_it_offset(std::distance(vertex.neighbor_edges.cbegin(), edge_it));
        const size_t staple_edge(circular_index(edge_it_offset + sgn_nozero(delta) * ((std::abs(delta) > 1) * 2 - 1), vertex.neighbor_edges.size()));

        assert(unsigned int((edge_it_offset + vertex.neighbor_edges.size() + sgn_nozero(delta) * ((std::abs(delta) > 1) * 2 - 1)) % vertex.neighbor_edges.size()) == staple_edge);

        helices[it_offset].attach(phys, helices[(vertex.neighbor_edges.begin() + staple_edge)->index], Helix::kBackwardFivePrime, Helix::kBackwardThreePrime);
    }
    std::cout << "Staples connected";

    /*for (std::vector<Edge>::const_iterator prev_it(edges.begin()); prev_it != edges.end(); ++prev_it) {
        const Vertex & vertex(vertices[prev_it->vertices[1]]);

        const size_t prev_it_offset(std::distance(edges.cbegin(), prev_it));
        const std::vector<Edge>::const_iterator next_it(circular_increment(prev_it, edges));
        const size_t next_it_offset(std::distance(edges.cbegin(), next_it));

        const Vertex::NeighborContainer::const_iterator prev_edge_it(vertex.edges_lookup.at(unsigned int(prev_it_offset)));
        const Vertex::NeighborContainer::const_iterator next_edge_it(vertex.edges_lookup.at(unsigned int(next_it_offset)));

        const ptrdiff_t delta(next_edge_it - prev_edge_it);
        assert(std::abs(delta) == 1 || size_t(std::abs(delta)) == vertex.neighbor_edges.size() - 1);

        if (std::abs(delta) == 1) {
            helices[prev_it_offset].attach(phys, helices[prev_it_offset - delta], Helix::kBackwardFivePrime, Helix::kBackwardThreePrime);
        }
    }*/

    return true;
}

bool scene::setupscaffoldfreeHelices(physics & phys) {
    // Find duplicate edges.
    //std::cout << "In setupscaffoldfreehelices\n";
    //std::unordered_map<Edge, int, std::function<size_t(const Edge &)>> duplicates(path.size() - 1, std::mem_fun_ref(&Edge::hash));
    std::cout << "Setting up helices\n";
    for (std::vector<std::vector<unsigned int >>::const_iterator it1(trails.begin()); it1 != trails.end(); it1++) {
        std::vector<unsigned int> trail = *it1;
        for (std::vector<unsigned int>::const_iterator it(trail.begin()); it != trail.end(); ++it) {
            //++duplicates[Edge(*it, *circular_increment(it, path))];
            unsigned int next = *circular_increment(it, trail);
            std::cout << "Adding " << *it << " -> " << next << " to edges\n";
            edges.push_back(Edge(*it, next));
        }
    }

    for (std::vector<Edge>::const_iterator it(edges.begin()); it != edges.end(); ++it) {
        for (int i = 0; i < 2; ++i)
            vertices[it->vertices[i]].neighbor_edges.push_back(Vertex::Edge((unsigned int)std::distance(edges.cbegin(), it)));
    }
    std::cout << "Vertex neighbors set\n";
    // Calculate vertex normal and edge angles.
    for (std::vector<Vertex>::iterator vertex_it(vertices.begin()); vertex_it != vertices.end(); ++vertex_it) {
        Vertex & vertex(*vertex_it);
        const unsigned int vertexIndex((unsigned int)std::distance(vertices.begin(), vertex_it));

        vertex.normal = kZeroVec;
        for (Vertex::NeighborContainer::const_iterator it(vertex.neighbor_edges.begin()); it != vertex.neighbor_edges.end(); ++it) {
            const physics::vec3_type edge1(vertex.position - vertices[edges[circular_increment(it, vertex.neighbor_edges)->index].other(vertexIndex)].position);
            const physics::vec3_type edge2(vertex.position - vertices[edges[it->index].other(vertexIndex)].position);
            vertex.normal += edge1.getNormalized().cross(edge2.getNormalized());
        }

        vertex.normal.normalize();

        std::cout << "vertex " << vertexIndex << " position: " << vertex.position.x << ' ' << vertex.position.y << ' ' << vertex.position.z << std::endl;

        physics::vec3_type tangent((vertices[edges[vertex.neighbor_edges.front().index].other(vertexIndex)].position - vertex.position).getNormalized());
        tangent -= proj(tangent, vertex.normal);

        for (Vertex::Edge & edge : vertex.neighbor_edges) {
            const physics::vec3_type delta(vertices[edges[edge.index].other(vertexIndex)].position - vertex.position);
            //edge.angle = signedAngle(delta - proj(delta, vertex.normal), tangent, vertex.normal);
            edge.angle = signedAngle(tangent, delta - proj(delta, vertex.normal), vertex.normal);
            std::cout << "Edge " << edge.index << " angle " << edge.angle << "\n";
        }
    }

    // Validate angles for debugging
    for (const Edge & edge : edges) {
        const Vertex & vertex(vertices[edge.vertices[1]]);
        for (Vertex::NeighborContainer::const_iterator nit(vertex.neighbor_edges.begin()); nit != vertex.neighbor_edges.end() - 1; ++nit) {
            //if (nit->angle == (nit + 1)->angle)
                //PRINT("Angles are the same for edges %u and %u at vertex %u leading to vertices %u and %u. Vertex normal: %f, %f, %f", 1 + nit->index, 1 + (nit + 1)->index, 1 + edge.vertices[1], 1 + edges[nit->index].other(edge.vertices[1]), 1 + edges[(nit + 1)->index].other(edge.vertices[1]), vertices[edge.vertices[1]].normal.x, vertices[edge.vertices[1]].normal.y, vertices[edge.vertices[1]].normal.z);
            //assert(nit->angle != (nit + 1)->angle);
        }
    }


    for (const Vertex & vertex : vertices)
        assert(vertex.neighbor_edges.size() % 2 == 0);
    //std::cout << "angles and neighbour edges validated\n";

    // Sort edges by angle and create the edge lookup.
    std::for_each(vertices.begin(), vertices.end(), std::mem_fun_ref(&Vertex::generate_lookup));

    auto compareEdges = [](Edge a, Edge b) {
        return (a.vertices[0] <= b.vertices[0]);};

    //std::set<Edge, decltype(compareEdges) > has_forward(compareEdges); // set with custom compare function
    //std::unordered_map<Edge, int, std::function<size_t(const Edge &)>> duplicates(path.size() - 1, std::mem_fun_ref(&Edge::hash));

    std::vector<Edge> has_forward;

    //std::vector<std::vector<bool>> is_forward;
    //std::unordered_map<Edge,Phys::Helix*> edge_helix;
    //std::unordered_map<Edge,Phys::Helix*,std::function<size_t(const Edge &)>> edge_helix;

    std::map<Edge,int> edge_helix; // maps edges to helix indices
    // Creating the helices (assigning lengths, positions and orientations)
    // Out of the bidirectional edges, the first edge (strand) encountered in will be "forward"
    // Connections determine the directions of the strands

    int helix_index = 0;
    for (std::vector<std::vector<unsigned int>>::const_iterator it1(trails.begin()); it1 != trails.end(); ++it1) {
        std::vector<unsigned int> trail = *it1;
        //std::vector<bool> tmp_is_forward;

        //std::cout << "trail starts from " << trail[0] << "\n";
        for (std::vector<unsigned int>::const_iterator it(trail.begin()); it != trail.end(); ++it) {

            const std::vector<unsigned int>::const_iterator next_it(circular_increment(it, trail));
            const Edge edge(*it, *next_it);
            const Edge reverse_edge(*next_it, *it);
            bool forward = true;
            //std::cout << *it << " -> " << *next_it;

            std::vector<Edge>::iterator reverse_edge_in_helix = std::find(has_forward.begin(), has_forward.end(), reverse_edge);
            if (reverse_edge_in_helix == has_forward.end()) { // Edge has overloaded == operator
                //std::cout << ": forward\n";
                has_forward.push_back(edge);
                //tmp_is_forward.push_back(true);
            }
            else {
                //std::cout << ": not forward\n";
                forward = false;
                //tmp_is_forward.push_back(false);
                std::pair<Edge,int> tmp_edge_helix (edge,edge_helix.at(reverse_edge)); // add edge mapping
                edge_helix.insert(tmp_edge_helix);

            }
            Vertex *vertices_[] = { &vertices[edge.vertices[0]], &vertices[edge.vertices[1]] };

            const Vertex::NeighborContainer::iterator neighbor_edge_it(std::find_if(vertices_[0]->neighbor_edges.begin(), vertices_[0]->neighbor_edges.end(), [this, &edge](const Vertex::Edge & vertex_edge) { return edges[vertex_edge.index].other(edge.vertices[0]) == edge.vertices[1]; }));
            assert(neighbor_edge_it != vertices_[0]->neighbor_edges.end());

            std::set<unsigned int> vertices_unique_neighbors[2];
            for (int i = 0; i < 2; ++i) {
                for (const Vertex::Edge & vertex_edge : vertices_[i]->neighbor_edges)
                    vertices_unique_neighbors[i].insert(edges[vertex_edge.index].other(edge.vertices[i]));
            }
            if (forward) {
                //std::cout << "in if\n";
                const physics::vec3_type origo((vertices_[0]->position + vertices_[1]->position) / 2), direction(vertices_[1]->position - vertices_[0]->position);
                //std::cout << "origo calculated\n";
                physics::vec3_type tangent(kZeroVec);
                const unsigned int connecting_vertex_indices[] = { *circular_increment(next_it, trail), *circular_decrement(it, trail) };
                //std::cout << "connecting vertex indices\n";
                bool cross = (vertices[connecting_vertex_indices[0]].position - vertices_[1]->position).dot(vertices[connecting_vertex_indices[1]].position - vertices_[0]->position) < 0;
                //std::cout << "cross calculated\n";
                double length(direction.magnitude() - apothem(2 * DNA::RADIUS, vertices_unique_neighbors[0].size()) - apothem(2 * DNA::RADIUS, vertices_unique_neighbors[1].size()));
                //std::cout << "length calculated\n";
                if (settings.discretize_lengths) {
                    const double num_half_turns(length / DNA::HALF_TURN_LENGTH);
                    if (int(std::floor(num_half_turns)) % 2)
                        length = DNA::HALF_TURN_LENGTH * (cross ? std::floor(num_half_turns) : std::ceil(num_half_turns));
                    else
                        length = DNA::HALF_TURN_LENGTH * (cross ? std::ceil(num_half_turns) : std::floor(num_half_turns));
                }
                //std::cout << "lengths discretized\n";
                /*Phys::Helix helix(helix_settings,
                                  phys, DNA::DistanceToBaseCount(length),
                                  physics::transform_type(origo, rotationFromTo(kPosZAxis, direction)));*/
                //std::cout << "helix contructed\n";
                helices.emplace_back(helix_settings,
                                     phys, DNA::DistanceToBaseCount(length),
                                     physics::transform_type(origo, rotationFromTo(kPosZAxis, direction)));
                //std::cout << "helix pushed back\n";

                std::pair<Edge,int> tmp_edge_helix (edge,helix_index);
                helix_index++;// add edge mapping
                //std::cout << "created helix edge pair\n";
                edge_helix.insert(tmp_edge_helix);
                //std::cout << "created helix edge mapping\n";
                std::cout << "Helix index: " << helix_index-1 << "\nposition: " << (float) origo[0] << " " << (float) origo[1] << " " << (float) origo[2] << "\n";
                std::cout << "rotation: " << direction[0] << " " << direction[1] << " " << direction[2] << "\n";
            }
        }
    }
    // DEBUG: Print the map
    std::cout << "Printing the map\n";
    for (auto it = edge_helix.begin(); it != edge_helix.end(); it++) {
        std::cout << "Edge: " << it->first.vertices[0] << " -> " << it->first.vertices[1] << ": Helix " << it->second << "\n";
    }
    // Create the connections

    //std::map<Phys::Helix,
    std::cout << "Has_forward:\n";
    for (auto it : has_forward) {
        std::cout << it.vertices[0] << " " << it.vertices[1] << "\n";
    }

    std::cout << "\n\nCreating the connections\n\n";
    std::cout << "has_forward length: " << has_forward.size() << "\n";
    for (std::vector<std::vector<unsigned int>>::const_iterator it1(trails.begin()); it1 != trails.end(); ++it1) {

        std::vector<unsigned int> trail = *it1;
        std::cout << "trail starts from " << trail[0] << "\n";
        for (std::vector<unsigned int>::const_iterator it(trail.begin()); it != trail.end(); ++it) {
            const std::vector<unsigned int>::const_iterator next_it(circular_increment(it, trail));
            const std::vector<unsigned int>::const_iterator next_next_it(circular_increment(next_it, trail));

            Edge edge_1(*it,*next_it);
            Edge edge_2(*next_it,*next_next_it);
            //std::cout << "Edge_1: " << edge_1.vertices[0] << " " << edge_1.vertices[1] << "\n";
            //std::cout << "Edge_2: " << edge_2.vertices[0] << " " << edge_2.vertices[1] << "\n";
            bool edge_1_forward = std::find(has_forward.begin(),has_forward.end(),edge_1) != has_forward.end();
            bool edge_2_forward = std::find(has_forward.begin(),has_forward.end(),edge_2) != has_forward.end();
            std::cout << "edges identified: " << edge_1.vertices[0] << " -> " << edge_1.vertices[1] << " (" << edge_1_forward << ") and " << edge_2.vertices[0] << " -> " << edge_2.vertices[1] << " (" << edge_2_forward << ")\n";
            int helix_1 = edge_helix.at(edge_1);
            int helix_2 = edge_helix.at(edge_2);
            //std::cout << "helices identified: " << helix_1+1 << " and " << helix_2+1 << "\n";
            Helix::AttachmentPoint edge_1_conn,edge_2_conn;
            if (edge_1_forward && edge_2_forward) {
                edge_1_conn = Helix::kForwardThreePrime;
                edge_2_conn = Helix::kForwardFivePrime;
                std::cout << helix_1 + 1 << " f3 " << helix_2 + 1 << " f5\n";
            }
            else if (edge_1_forward && !edge_2_forward){
                edge_1_conn = Helix::kForwardThreePrime;
                edge_2_conn = Helix::kBackwardFivePrime;
                std::cout << helix_1 + 1 << " f3 " << helix_2 + 1 << " b5\n";
            }
            else if (!edge_1_forward && edge_2_forward) {
                edge_1_conn = Helix::kBackwardThreePrime;
                edge_2_conn = Helix::kForwardFivePrime;
                std::cout << helix_1 +1 << " b3 " << helix_2 + 1 << " f5\n";
            }
            else{ // both false
                edge_2_conn = Helix::kBackwardFivePrime;
                edge_1_conn = Helix::kBackwardThreePrime;
                std::cout << helix_1 + 1 << " b3 " << helix_2 + 1 << " b5\n";
            }
            //std::cout << "attachment points identified\n";
            helices[helix_1].attach(phys,helices[helix_2],edge_1_conn,edge_2_conn);
            //std::cout << "helices attached\n";
        }
    }
    std::cout << "Finished setting up helices\n";
    return true;
}


void scene::getTotalSeparationMinMaxAverage(PhysXRelax::physics::real_type & min, PhysXRelax::physics::real_type & max, PhysXRelax::physics::real_type & average, PhysXRelax::physics::real_type & total) const {
    min = std::numeric_limits<physics::real_type>::infinity();
    max = physics::real_type(0);
    total = physics::real_type(0);
    size_t helix_offset = 0; // DEBUG
    for (const Helix & helix : helices) {
        const Helix::ConnectionContainer & connections(helix.getJoints());
        size_t connection_offset = 0; // DEBUG
        for (const Helix::Connection & connection : connections) {
            if (connection.joint == nullptr) { // DEBUG ONLY
                //PRINT("WARNING: Joint is null, should not happen! connection: %u, helix: %u", connection_offset, helix_offset);
                continue;
            }
            const physics::real_type separation(connection.joint->getDistance());
            min = std::min(min, separation);
            max = std::max(max, separation);
            total += separation;

            connection_offset++; // DEBUG
        }

        helix_offset++; // DEBUG
    }

    average = total / (helices.size() * 4);
    total /= 2;
}

SceneDescription::SceneDescription(scene & scene) : totalSeparation(scene.getTotalSeparation()) {
    helices.reserve(scene.getHelixCount());
    std::unordered_map<const PhysXRelax::Helix *, Helix *> helixMap;
    helixMap.reserve(scene.getHelixCount());
    std::cerr << "Creating helices"<< std::endl;
    for (const PhysXRelax::Helix & helix : scene.helices) {
        helices.push_back(Helix(helix.getBaseCount(), helix.getTransform()));
        helixMap.emplace(&helix, &helices.back());
    }
    std::cerr << "Creating connections"<< std::endl;
    for (const PhysXRelax::Helix & helix : scene.helices) {
        Helix & newHelix(*helixMap.at(&helix));
        std::cerr << "Creting connections for a helix\n";
        for (int i = 0; i < 4; ++i) {
            std::cerr << "Creating connection " << i << std::endl;
            newHelix.connections[i] = helixMap.at(helix.getJoint(PhysXRelax::Helix::AttachmentPoint(i)).helix);
        }
    }
    std::cerr << "Created scenedescription"<< std::endl;
}

bool SceneDescription::write(std::ostream & out) const {
    std::unordered_map<const Helix *, std::string> helixNames;
    helixNames.reserve(helices.size());

    {
        unsigned int i(1);
        for (const Helix & helix : helices) {
            std::ostringstream namestream;
            namestream << "helix_" << i++;
            const std::string name(namestream.str());
            helixNames.emplace(&helix, name);

            const physics::vec3_type & position(helix.getPosition());
            const physics::quaternion_type & orientation(helix.getOrientation());
            out << "hb " << name << ' ' << helix.baseCount << ' ' << position.x << ' ' << position.y << ' ' << position.z << ' ' << orientation.x << ' ' << orientation.y << ' ' << orientation.z << ' ' << orientation.w << std::endl;
        }
    }

    out << std::endl;

    for (const Helix & helix : helices) {
        const std::string & name(helixNames.at(&helix));

        out << "c " << name << " f3' " << helixNames.at(helix.connections[PhysXRelax::Helix::kForwardThreePrime]) << " f5'" << std::endl
            << "c " << helixNames.at(helix.connections[PhysXRelax::Helix::kBackwardFivePrime]) << " b3' " << name << " b5'" << std::endl;
    }

    out << std::endl << "autostaple" << std::endl << "ps " << helixNames.at(&helices.front()) << " f3'" << std::endl;
    return true;
}

template<typename StoreBestFunctorT, typename RunningFunctorT>
void PhysXRelaxation::simulated_annealing(int kmax, float emax, unsigned int minbasecount, int baserange,
        StoreBestFunctorT store_best_functor, RunningFunctorT running_functor) {

    int modifiedHelix, previousBaseCount;
    PhysXRelax::physics::transform_type previousTransform;

    scene::HelixContainer & helices(mesh.getHelices());
    const scene::HelixContainer::size_type helixCount(helices.size());

    simulated_annealing(mesh,
        [](scene & mesh) { return mesh.getTotalSeparation(); },
        [&helixCount](float k) { return float(std::max(0., (exp(-k) - 1 / M_E) / (1 - 1 / M_E))) * helixCount; },
        [&modifiedHelix, &helices, &helixCount, &previousBaseCount, &previousTransform, &phys, &minbasecount, &baserange, &running_functor](scene & mesh) {
            for (Helix & helix : helices)
                helix.setTransform(helix.getInitialTransform());

            modifiedHelix = rand() % helixCount;
            Helix & helix(helices[modifiedHelix]);
            previousBaseCount = helix.getBaseCount();
            previousTransform = helix.getTransform();

            helix.recreateRigidBody(
                phys, std::max(minbasecount, helix.getInitialBaseCount() + (rand() % 2 * 2 - 1) * (1 + rand() % (baserange))), helix.getInitialTransform());

            while (!mesh.isSleeping() && running_functor()) {
                phys.scene->simulate(1.0f / 60.0f);
                phys.scene->fetchResults(true);
            }
        },
        probability_functor<float, float>(),
        [&modifiedHelix, &helices, &previousBaseCount, &previousTransform, &phys](scene & mesh) {
            Helix & helix(helices[modifiedHelix]);
            helix.recreateRigidBody(phys, previousBaseCount, helix.getInitialTransform());
        },
        store_best_functor,
        running_functor,
        kmax, emax);
}

/*
 * Simple gradient descent implementation: Energy lower? Choose it, if not don't.
 */
template<typename StoreBestFunctorT, typename RunningFunctorT>
void PhysXRelaxation::gradient_descent(int minbasecount, StoreBestFunctorT store_best_functor, RunningFunctorT running_functor, const int &iterations) {
    scene::HelixContainer & helices(mesh.getHelices());

    while (!mesh.isSleeping() && running_functor()) {
        phys.scene->simulate(1.0f / 60.0f);
        phys.scene->fetchResults(true);
    }

    //physics::real_type separation(mesh.getTotalSeparation());
    PhysXRelax::physics::real_type min, max, average, total;
    mesh.getTotalSeparationMinMaxAverage(min, max, average, total);
    store_best_functor(mesh, min, max, average, total);
    if (iterations == 1) {
        return;
    }
    int k = 1;

    for (Helix & helix : helices) {
        for (int i = 0; i < 2; ++i) {
            if (!running_functor() || k >= iterations)
                return;

            helix.recreateRigidBody(phys, std::max(minbasecount, int(helix.getInitialBaseCount() + (i * 2 - 1))), helix.getInitialTransform());

            while (!mesh.isSleeping() && running_functor()) {
                phys.scene->simulate(1.0f / 60.0f);
                phys.scene->fetchResults(true);
            }

            //const physics::real_type newseparation(mesh.getTotalSeparation());
            PhysXRelax::physics::real_type newtotal;
            mesh.getTotalSeparationMinMaxAverage(min, max, average, newtotal);

            if (newtotal < total) {
                total = newtotal;
                store_best_functor(mesh, min, max, average, total);
            } else
                helix.recreateRigidBody(phys, helix.getInitialBaseCount(), helix.getInitialTransform());

            for (PhysXRelax::Helix & helix : helices)
                helix.setTransform(helix.getInitialTransform());
            k++;
        }
    }
}

/*
 * Does a simple rectification of the structure without modification.
 */
template<typename RunningFunctorT>
SceneDescription PhysXRelaxation::simulated_rectification(RunningFunctorT running_functor) {
    while (!mesh.isSleeping() && running_functor()) {
        phys.scene->simulate(1.0f / 60.0f);
        phys.scene->fetchResults(true);
    }

    return SceneDescription(mesh);
}

PhysXRelaxation::PhysXRelaxation(std::string &name,PhysXRelax::physics::settings_type &physics_settings, scene::settings_type &scene_settings, Helix::settings_type &helix_settings,const int &iterations) : Relaxation(name), mesh(scene_settings,helix_settings), phys(physics_settings), running(true), iterations(iterations) {
    seed();
}

int PhysXRelaxation::scaffold_main(std::vector<coordinates> &inputvertices, std::vector<unsigned int> &nodetrail) {
    std::cerr << "In scaffold_main(). number of vertices: " << inputvertices.size() << "\nLength of trail: " << nodetrail.size()<< std::endl;
    mesh.set_data(inputvertices,nodetrail);
    mesh.print_data();
    std::cerr << "Data set up"<< std::endl;
    try {
        if (!mesh.setupHelices(phys)) {
            outstream << "Failed to read scene" << std::endl;
            return 1;
        }
    }
    catch (const std::runtime_error & e) {
        outstream << "POPUP_ERRFailed to read scene\n" << e.what() << "POPUP_END" << std::endl;
        return 1;
    }
    std::cerr << "Helices set up"<< std::endl;
    physics::real_type initialmin, initialmax, initialaverage, initialtotal, min, max, average, total;
    mesh.getTotalSeparationMinMaxAverage(initialmin, initialmax, initialaverage, initialtotal);
    std::cerr << "Got separation"<< std::endl;
    std::cerr << "name" << name.c_str()<< std::endl;
    std::string output_file(name.c_str());
    output_file.append(".rpoly");
    std::cerr << "Creating scenedescription"<< std::endl;
    SceneDescription test_scene(mesh);
    std::stringstream testout;
    test_scene.write(testout);
    std::cerr << testout.str().c_str()<< std::endl;
    outstream << "Running simulation for scene, outputting to " << output_file << "\"." << std::endl
        << "Initial: min: " << initialmin << ", max: " << initialmax << ", average: " << initialaverage << ", total: " << initialtotal << " nm" << std::endl
        << "Connect with NVIDIA PhysX Visual Debugger to " << PVD_HOST << ':' << PVD_PORT << " to visualize the progress. " << std::endl
        << "Press ^C to stop the relaxation...." << std::endl;

    //setinterrupthandler<handle_exit>();
    std::cerr << "Running grad descent";
    SceneDescription best_scene;
    gradient_descent(7,
        [&best_scene, &min, &max, &average, &total](scene & mesh, physics::real_type min_, physics::real_type max_, physics::real_type average_, physics::real_type total_) { min = min_; max = max_; average = average_; total = total_; std::stringstream sstr; sstr << "State: min: " << min << ", max: " << max << ", average: " << average << " total: " << total << " nm" << std::endl; std::string str = sstr.str(); best_scene = SceneDescription(mesh); },
        [this]() { return running; }, iterations);

    std::cerr << "run grad descent"<< std::endl;
    outstream << "Result: min: " << min << ", max: " << max << ", average: " << average << ", total: " << total << " nm" << std::endl;
    std::ofstream outfile(output_file);
    outfile << "# Relaxation of original " << " file. " << mesh.getHelixCount() << " helices." << std::endl
        << "# Total separation: Initial: min: " << initialmin << ", max: " << initialmax << ", average: " << initialaverage << ", total: " << initialtotal << " nm" << ", final: min: " << min << ", max: " << max << ", average: " << average << ", total: " << total << " nm" << std::endl;

    if (!best_scene.write(outfile)) {
        outstream << "Failed to write resulting mesh to \"" << output_file << "\"" << std::endl;
    }
    outfile.close();
    std::cerr << outstream.str().c_str()<< std::endl;
    return 1;
}


int PhysXRelaxation::scaffold_free_main(std::vector<coordinates> &inputvertices, std::vector<std::vector<unsigned int>> &nodetrail) {
    std::cerr << "In scaffold_free_main(). number of vertices: " << inputvertices.size() << "\nLength of trail: " << nodetrail.size()<< std::endl;
    mesh.set_sf_data(inputvertices,nodetrail);
    mesh.print_data();
    std::cerr << "Data set up"<< std::endl;
    try {
        if (!mesh.setupscaffoldfreeHelices(phys)) {
            outstream << "Failed to read scene" << std::endl;
            return 1;
        }
    }
    catch (const std::runtime_error & e) {
        outstream << "POPUP_ERRFailed to read scene\n" << e.what() << "POPUP_END" << std::endl;
        return 1;
    }
    std::cerr << "Helices set up"<< std::endl;
    physics::real_type initialmin, initialmax, initialaverage, initialtotal, min, max, average, total;
    mesh.getTotalSeparationMinMaxAverage(initialmin, initialmax, initialaverage, initialtotal);
    std::cerr << "Got separation"<< std::endl;
    std::cerr << "name" << name.c_str()<< std::endl;
    std::string output_file(name.c_str());
    output_file.append(".rpoly");
    std::cerr << "Creating scenedescription"<< std::endl;
    //SceneDescription test_scene(mesh);
    //std::stringstream testout;
    //test_scene.write(testout);
    //std::cerr << testout.str().c_str()<< std::endl;
    outstream << "Running simulation for scene, outputting to " << output_file << "\"." << std::endl
        << "Initial: min: " << initialmin << ", max: " << initialmax << ", average: " << initialaverage << ", total: " << initialtotal << " nm" << std::endl
        << "Connect with NVIDIA PhysX Visual Debugger to " << PVD_HOST << ':' << PVD_PORT << " to visualize the progress. " << std::endl
        << "Press ^C to stop the relaxation...." << std::endl;

    //setinterrupthandler<handle_exit>();
    std::cerr << "Running grad descent\n";
    SceneDescription best_scene;
    gradient_descent(7,
        [&best_scene, &min, &max, &average, &total](scene & mesh, physics::real_type min_, physics::real_type max_, physics::real_type average_, physics::real_type total_) { min = min_; max = max_; average = average_; total = total_; std::stringstream sstr; sstr << "State: min: " << min << ", max: " << max << ", average: " << average << " total: " << total << " nm" << std::endl; std::string str = sstr.str(); best_scene = SceneDescription(mesh); },
        [this]() { return running; }, iterations);

    std::cerr << "run grad descent"<< std::endl;
    outstream << "Result: min: " << min << ", max: " << max << ", average: " << average << ", total: " << total << " nm" << std::endl;
    std::ofstream outfile(output_file);
    outfile << "# Relaxation of original " << " file. " << mesh.getHelixCount() << " helices." << std::endl
        << "# Total separation: Initial: min: " << initialmin << ", max: " << initialmax << ", average: " << initialaverage << ", total: " << initialtotal << " nm" << ", final: min: " << min << ", max: " << max << ", average: " << average << ", total: " << total << " nm" << std::endl;

    if (!best_scene.write(outfile)) {
        outstream << "Failed to write resulting mesh to \"" << output_file << "\"" << std::endl;
    }
    outfile.close();
    std::cerr << outstream.str().c_str()<< std::endl;
    return 1;
}