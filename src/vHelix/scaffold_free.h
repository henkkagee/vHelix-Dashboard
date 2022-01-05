#include "design.h"
#include "physxrelaxation.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>





namespace dir {

using namespace boost;

typedef adjacency_list< listS, vecS, directedS,
        property<vertex_index_t, int>,
        property<edge_index_t, std::size_t,
        property< edge_weight_t, int,
        property< edge_weight2_t, int > > > > DiGraph;

typedef adjacency_list< listS, vecS, undirectedS,
        property<vertex_index_t, int>,
        property<edge_index_t, std::size_t,
        property< edge_weight_t, int,
        property< edge_weight2_t, int > > > > Graph;

typedef graph_traits < Graph >::edge_iterator UEdgeIterator;

typedef graph_traits < DiGraph >::vertex_descriptor Vertex;
typedef graph_traits < DiGraph >::edge_descriptor Edge;
typedef graph_traits < DiGraph >::vertices_size_type Size;
typedef graph_traits < DiGraph >::vertex_iterator VertexIterator;
typedef graph_traits < DiGraph >::edge_iterator EdgeIterator;
typedef graph_traits < DiGraph >::out_edge_iterator OutEdgeIterator;
typedef graph_traits < DiGraph >::in_edge_iterator InEdgeIterator;
typedef graph_traits < DiGraph >::adjacency_iterator AdjacencyIterator;

// A typedef for the storage - a vector of vectors of edge descriptors
typedef std::vector< std::vector< graph_traits<DiGraph>::edge_descriptor > > planar_embedding_storage_t;
// A typedef for the iterator property map, assuming the graph has an interior vertex index map
typedef boost::iterator_property_map< planar_embedding_storage_t::iterator, property_map<DiGraph, vertex_index_t>::type> planar_embedding_t;


void print_embedding(const planar_embedding_t & planar_embedding, const DiGraph & G, std::vector<std::vector<Vertex> > & embedding_as_vertex_order);
// Converts a planar_embedding given by boyer_myrvold_planarity_test to an edge code. It needs an edge index for the edges.
std::string print_walk(std::vector<Vertex> Trail, std::string label);
std::string to_string_graph(const DiGraph & G, std::string lineheader = "", std::string liststart=": ", std::string adj_sep = " ", std::string line_sep ="\n");
std::string to_string_graph_edgelist(const DiGraph & G, std::string pair_sep= ",", std::string edge_frame_left="{", std::string edge_frame_right="}", std::string list_sep = " ");

bool read_dimacs(std::string input_filename, DiGraph & G);
bool write_dimacs(std::string output_filename, DiGraph & G);
bool write_weighed_dimacs(std::string output_filename, DiGraph & G);
bool read_vertex_code(std::string input_filename, std::vector<std::vector<std::size_t> > & edge_code);
bool write_vertex_code(std::string filename, std::vector<std::vector<std::size_t> > edge_code);
}

class Scaffold_free : public Design
{
public:
    Scaffold_free(const std::string meshpath, const std::string meshname);
    int createGraph();
    int find_trail();
    int main();
    int relax(const QVector<QVariant> args);
    int create_strands();

private:
    std::vector<std::vector<dir::Vertex>> nodetrail;
    dir::DiGraph graph;
};