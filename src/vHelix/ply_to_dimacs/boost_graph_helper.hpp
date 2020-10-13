/*
 * boost_graph_helper.hpp
 *
 *  Created on: Jul 4, 2013
 *      Author: mohamma1
 */

#ifndef BOOST_GRAPH_HELPER_HPP_
#define BOOST_GRAPH_HELPER_HPP_

#include <list>
#include <vector>
#include <sstream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
using namespace boost;

typedef adjacency_list< listS, vecS, undirectedS,
		property<vertex_index_t, int>,
		property<edge_index_t, std::size_t,
		property< edge_weight_t, int,
		property< edge_weight2_t, int > > > > Graph;
// A typedef for the storage - a vector of vectors of edge descriptors
typedef std::vector< std::vector< graph_traits<Graph>::edge_descriptor > > planar_embedding_storage_t;
// A typedef for the iterator property map, assuming the graph has an interior vertex index map
typedef boost::iterator_property_map< planar_embedding_storage_t::iterator, property_map<Graph, vertex_index_t>::type> planar_embedding_t;
typedef graph_traits < Graph >::vertex_descriptor Vertex;
typedef graph_traits < Graph >::edge_descriptor Edge;
typedef graph_traits < Graph >::vertices_size_type Size;
typedef graph_traits < Graph >::vertex_iterator VertexIterator;
typedef graph_traits < Graph >::edge_iterator EdgeIterator;
typedef graph_traits < Graph >::out_edge_iterator OutEdgeIterator;
typedef graph_traits < Graph >::in_edge_iterator InEdgeIterator;
typedef graph_traits < Graph >::adjacency_iterator AdjacencyIterator;

void print_embedding(const planar_embedding_t & planar_embedding, const Graph & G, std::vector<std::vector<Vertex> > & embedding_as_vertex_order);
// Converts a planar_embedding given by boyer_myrvold_planarity_test to an edge code. It needs an edge index for the edges.
void embedding_to_edge_code(const planar_embedding_t & planar_embedding, Graph & G, std::vector<std::vector<std::size_t> > & edge_code);
std::string print_walk(std::list<Vertex> Trail, std::string label);
std::string to_string_graph(const Graph & G, std::string lineheader = "", std::string liststart=": ", std::string adj_sep = " ", std::string line_sep ="\n");
std::string to_string_graph_edgelist(const Graph & G, std::string pair_sep= ",", std::string edge_frame_left="{", std::string edge_frame_right="}", std::string list_sep = " ");

bool read_dimacs(std::string input_filename, Graph & G);
bool write_dimacs(std::string output_filename, Graph & G);
bool write_weighed_dimacs(std::string output_filename, Graph & G);
bool read_edge_code(std::string input_filename, std::vector<std::vector<std::size_t> > & edge_code);
bool write_edge_code(std::string filename, std::vector<std::vector<std::size_t> > edge_code);

#endif /* BOOST_GRAPH_HELPER_HPP_ */
