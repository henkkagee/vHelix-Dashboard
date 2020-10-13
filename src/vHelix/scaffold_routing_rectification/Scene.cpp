#include "Definition.h"
#include "DNA.h"
#include "Scene.h"
#include "SimulatedAnnealing.h"

#include <array>
#include <cassert>
#include <functional>
#include <string>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <iterator>
#include <algorithm>

bool scene::read_rmesh(physics & phys, std::istream & ifile) {
	physics::vec3_type vertex, zDirection;
	unsigned int edge, numBases;
	unsigned int gcount(0);
	std::string line;

	while (ifile.good()) {
		std::getline(ifile, line);

		if (sscanf(line.c_str(), "e %u", &edge) == 1) {
			assert(edge >= 1);
			path.push_back(edge - 1);
		} else if (sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z) == 3) {
			vertex *= settings.initial_scaling;
			vertices.push_back(vertex);
		} else if (sscanf(line.c_str(), "h %u %f %f %f %f %f %f", &numBases, &vertex.x, &vertex.y, &vertex.z, &zDirection.x, &zDirection.y, &zDirection.z) == 7) {
			const physics::vec3_type cross(kPosZAxis.cross(zDirection));
            helices.push_back(Phys::Helix(helix_settings, phys, numBases, physics::transform_type(vertex, physics::quaternion_type(signedAngle(kPosZAxis, zDirection, cross), cross.getNormalized()))));
		} else if (line[0] == 'g')
			++gcount;
	}

	if (gcount > 1) {
		PRINT("CRITICAL: The importer does not support multiple meshes stored in one file. Aborting.");
		return false;
	}

	if ((!helices.empty() && helices.size() != path.size() - 1) || path.empty() || vertices.empty())
		return false;

	if (!helices.empty()) {
		PRINT("Not implemented yet! Helices can't be provided!");
		return false;
	}

	return helices.empty() ? setupHelices(phys) : true;
}

bool scene::read_ply(physics & phys, std::istream & ply_file, std::istream & ntrail_file) {
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

bool scene::setupHelices(physics & phys) {
	// Find duplicate edges.
	std::unordered_map<Edge, int, std::function<size_t(const Edge &)>> duplicates(path.size() - 1, std::mem_fun_ref(&Edge::hash));

	for (std::vector<unsigned int>::const_iterator it(path.begin()); it != path.end(); ++it) {
		++duplicates[Edge(*it, *circular_increment(it, path))];
		edges.push_back(Edge(*it, *circular_increment(it, path)));
	}	

	for (std::vector<Edge>::const_iterator it(edges.begin()); it != edges.end(); ++it) {
		for (int i = 0; i < 2; ++i)
			vertices[it->vertices[i]].neighbor_edges.push_back(Vertex::Edge(unsigned int(std::distance(edges.cbegin(), it))));
	}

	// Calculate vertex normal and edge angles.
	for (std::vector<Vertex>::iterator vertex_it(vertices.begin()); vertex_it != vertices.end(); ++vertex_it) {
		Vertex & vertex(*vertex_it);
		const unsigned int vertexIndex(unsigned int(std::distance(vertices.begin(), vertex_it)));

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

	// Correct angles for edges visited multiple times.
	for (std::vector<unsigned int>::const_iterator it(path.begin()); it != path.end(); ++it) {
		const std::vector<unsigned int>::const_iterator next_it(circular_increment(it, path));
		const Edge edge(*it, *next_it);
		const unsigned int it_offset(unsigned int(std::distance(path.cbegin(), it)));
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

	// Connect the scaffold.
	for (HelixContainer::iterator it(helices.begin()); it != helices.end(); ++it)
        circular_decrement(it, helices)->attach(phys, *it, Phys::Helix::kForwardThreePrime, Phys::Helix::kForwardFivePrime);

	// Connect the staples.

	for (std::vector<Edge>::const_iterator it(edges.begin()); it != edges.end(); ++it) {
		const Vertex & vertex(vertices[it->vertices[1]]);
		const unsigned int it_offset(unsigned int(std::distance(edges.cbegin(), it)));
		const unsigned int next_it_offset(unsigned int(circular_index(it_offset + 1, edges.size())));
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

        helices[it_offset].attach(phys, helices[(vertex.neighbor_edges.begin() + staple_edge)->index], Phys::Helix::kBackwardFivePrime, Phys::Helix::kBackwardThreePrime);
	}

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

void scene::getTotalSeparationMinMaxAverage(physics::real_type & min, physics::real_type & max, physics::real_type & average, physics::real_type & total) const {
	min = std::numeric_limits<physics::real_type>::infinity();
	max = physics::real_type(0);
	total = physics::real_type(0);
	size_t helix_offset = 0; // DEBUG
    for (const Phys::Helix & helix : helices) {
        const Phys::Helix::ConnectionContainer & connections(helix.getJoints());
		size_t connection_offset = 0; // DEBUG
        for (const Phys::Helix::Connection & connection : connections) {
			if (connection.joint == nullptr) { // DEBUG ONLY
				PRINT("WARNING: Joint is null, should not happen! connection: %u, helix: %u", connection_offset, helix_offset);
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
    std::unordered_map<const Phys::Helix *, Helix *> helixMap;
	helixMap.reserve(scene.getHelixCount());

    for (const Phys::Helix & helix : scene.helices) {
		helices.push_back(Helix(helix.getBaseCount(), helix.getTransform()));
		helixMap.emplace(&helix, &helices.back());
	}

    for (const Phys::Helix & helix : scene.helices) {
		Helix & newHelix(*helixMap.at(&helix));
		for (int i = 0; i < 4; ++i)
            newHelix.connections[i] = helixMap.at(helix.getJoint(Phys::Helix::AttachmentPoint(i)).helix);
	}
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

        out << "c " << name << " f3' " << helixNames.at(helix.connections[Phys::Helix::kForwardThreePrime]) << " f5'" << std::endl
            << "c " << helixNames.at(helix.connections[Phys::Helix::kBackwardFivePrime]) << " b3' " << name << " b5'" << std::endl;
	}

	out << std::endl << "autostaple" << std::endl << "ps " << helixNames.at(&helices.front()) << " f3'" << std::endl;

	return true;
}
