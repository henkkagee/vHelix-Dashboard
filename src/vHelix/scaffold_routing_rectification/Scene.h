#ifndef _H_SCENE_H_
#define _H_SCENE_H_

#include "Utility.h"

#include <array>
#include <cassert>
#include <fstream>
#include <functional>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include "Helix.h"

class scene {
	friend class SceneDescription;
public:
    typedef std::vector<Phys::Helix> HelixContainer;

	struct settings_type {
		physics::real_type initial_scaling;
		bool discretize_lengths;
	};

    inline scene(const settings_type & settings, const Phys::Helix::settings_type & helix_settings) : settings(settings), helix_settings(helix_settings) {}

	// Distinguish the file type by its file ending and parses either a rmesh or a ply file.
	inline bool read(physics & phys, const std::string & filename) {
        std::ifstream ifs = std::ifstream(filename);
        std::ifstream ifstrail1 = std::ifstream(strip_trailing_string(filename, ".ply") + ".ntrail");
        std::ifstream ifstrail2 = std::ifstream(strip_trailing_string(filename, ".ntrail") + ".ply");
		if (ends_with(filename, ".ply"))
            return read_ply(phys, ifs, ifstrail1);
		else if (ends_with(filename, ".ntrail"))
            return read_ply(phys, ifstrail2, ifs);
		else
            return read_rmesh(phys, ifs);
	}

	// Reads a mesh in the .rmsh "Routed mesh" text based format from the scaffold-routing Maya exporter project.
	// TODO: Move to SceneDescription?
	bool read_rmesh(physics & phys, std::istream & ifile);

	// Read a mesh using the .ply and .ntrail formats
	bool read_ply(physics &phys, std::istream & ply_file, std::istream & ntrail_file);

	inline HelixContainer & getHelices() {
		return helices;
	}

	inline HelixContainer::size_type getHelixCount() const {
		return helices.size();
	}

	// TODO: Move to SceneDescription when Scene can be constructed from a SceneDescription.
	inline physics::real_type getTotalSeparation() const {
		physics::real_type totalSeparation(0);
        for (const Phys::Helix & helix : helices)
            totalSeparation += std::accumulate(helix.getJoints().begin(), helix.getJoints().end(), physics::real_type(0), [](physics::real_type & separation, const Phys::Helix::Connection & connection) { return separation + (connection ? connection.joint->getDistance() : physics::real_type(0)); });

		return totalSeparation / 2;
	}

	// TODO: Move to SceneDescription when Scene...
	void getTotalSeparationMinMaxAverage(physics::real_type & min, physics::real_type & max, physics::real_type & average, physics::real_type & total) const;

	inline bool isSleeping() const {
        return std::find_if(helices.begin(), helices.end(), std::mem_fun_ref(&Phys::Helix::isSleeping)) != helices.end();
	}

private:
	bool setupHelices(physics & phys);

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
	HelixContainer helices;

	struct Edge {
		std::array<unsigned int, 2> vertices;

		static std::hash<unsigned int> hasher;

		// Visual C++ support for initializer_list is awful.
		inline Edge(unsigned int vertices_[2]) : vertices(std::array<unsigned int, 2> { { vertices_[0], vertices_[1] } }) {}
		inline Edge(unsigned int vertex1, unsigned int vertex2) : vertices(std::array<unsigned int, 2> { { vertex1, vertex2 } }) {}

		inline bool operator==(const Edge & e) const {
			return (e.vertices[0] == vertices[0] && e.vertices[1] == vertices[1]) || (e.vertices[0] == vertices[1] && e.vertices[1] == vertices[0]);
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
    const Phys::Helix::settings_type helix_settings;
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

#endif /* _H_SCENE_H_ */
