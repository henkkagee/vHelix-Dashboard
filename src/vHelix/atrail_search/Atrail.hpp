/*
 * Atrail_search.hpp
 *
 *  Created on: Jul 4, 2013
 *      Author: mohamma1
 */

#ifndef ATRAIL_SEARCH_HPP_
#define ATRAIL_SEARCH_HPP_

#include <vector>
#include <list>
#include "ply_to_dimacs/boost_graph_helper.hpp"

// For console output to GUI
#include "vhelix.h"

enum parity { odd_even, even_odd }; // odd_even = odd follows even (1 follows 0, 3 follows 2, ...), even_odd = even follows odd (2 follows 1, 4 follows 3, ... 0 follows d-1)

// Checks whether the multigraph G has an Eulerian trail. The output is true if the graph has even degree vertices and is connected upto isolated points.
bool has_eulerian_trail(const Graph & G);

std::list<Vertex> find_eulerian_trail(const Graph & G);

// Finds an A-trail for the graph based on the embedding edge_code
// Preconditions: Graph Gr is Eulerian, is connected, has no self loops. The edges of Gr must be coded with an index from 0 to m-1;
//				  P is a proper edge code embedding of G.
bool Atrail_search(Graph & Gr, std::vector<std::vector<std::size_t> > & edge_code, std::list<std::size_t> & trail_edgelist, std::list<Vertex> & new_trail, vHelix &parent);

// Recursively splits the vertices of the branch nodes(bn) according to parity par and checks the graph remains connected.
// The graph must be simple (i.e) no multi edges and loops.
bool split_and_check(Graph & G, const std::vector<std::vector<Vertex> > & P, std::vector<unsigned int> bn, unsigned int dep, parity par,
				std::map<Vertex, std::vector<Vertex> > & bn_new_nodes_map, std::vector<parity> parity_vec);

// Intersections at nodes of degree 4 can be fixed locally. Vertices with degree 6 or more (branch nodes) need not.
// This function enumerates the high degree nodes so that the split and check is minimized by ensuring incorrect
// parity configurations are discarded quickly. Close by (graph theoretic sense) branch nodes should be split and checked as early
// as possible.
void enumerate_intersectable_nodes(const Graph &G, std::vector<unsigned int> & bn, Vertex s=0);

#endif /* ATRAIL_SEARCH_HPP_ */
