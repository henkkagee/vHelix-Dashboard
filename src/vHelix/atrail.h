#ifndef ATRAIL_H
#define ATRAIL_H
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#include "include/lemon/list_graph.h"
#include "include/lemon/matching.h"
#include "include/lemon/concepts/graph.h"
#include "include/lemon/full_graph.h"
#include "design.h"
#include "physxrelaxation.h"


namespace undir {

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

}

typedef lemon::FullGraph::EdgeMap<signed int> Distances;
typedef lemon::MaxWeightedPerfectMatching<lemon::FullGraph, Distances> MWPM;
typedef struct {
    size_t vertex;
    signed long int edge;
}vertex_edge_pair;






class Atrail : public Design
{
public:
    Atrail(const std::string meshpath, const std::string meshname);
    int createGraph();

    // Postman_tour
    bool is_even();
    int postman_tour();
    void reindex_edges();

    // make_embedding_eulerian
    int make_embedding_eulerian();

    // Atrail_search

    // Checks whether the multigraph G has an Eulerian trail. The output is true if the graph has even degree vertices and is connected upto isolated points.
    bool has_eulerian_trail();

    std::list<undir::Vertex> find_eulerian_trail(const undir::Graph &G);

    // Finds an A-trail for the graph based on the embedding edge_code
    // Preconditions: Graph Gr is Eulerian, is connected, has no self loops. The edges of Gr must be coded with an index from 0 to m-1;
    //				  P is a proper edge code embedding of G.
    bool Atrail_search();

    // Recursively splits the vertices of the branch nodes(bn) according to parity par and checks the graph remains connected.
    // The graph must be simple (i.e) no multi edges and loops.
    bool split_and_check(undir::Graph &G, const std::vector<std::vector<undir::Vertex> > & P, std::vector<unsigned int> bn, unsigned int dep, parity par,
                    std::map<undir::Vertex, std::vector<undir::Vertex> > & bn_new_nodes_map, std::vector<parity> parity_vec);

    // Intersections at nodes of degree 4 can be fixed locally. Vertices with degree 6 or more (branch nodes) need not.
    // This function enumerates the high degree nodes so that the split and check is minimized by ensuring incorrect
    // parity configurations are discarded quickly. Close by (graph theoretic sense) branch nodes should be split and checked as early
    // as possible.
    void enumerate_intersectable_nodes(const undir::Graph &G, std::vector<unsigned int> & bn, undir::Vertex s=0);

    bool write_trail();
    bool Atrail_verify();

    int main();

    int relax(const QVector<QVariant> args);

private:
    std::list<undir::Vertex> nodetrail;
    undir::Graph graph;
    embedding_t ecode;
    std::list<std::size_t> trail_edgelist;
};

#endif // ATRAIL_H
