#include "design.h"
#include "physxrelaxation.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <QQuaternion>

#include "dna.h"

#include "model.h"





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

class Create_strands
{
public:
    Create_strands();
    std::vector<Model::Helix> &readRpoly(const char *filename, int nicking_min_length = 0, int nicking_max_length = 0);
    void writeOxView(const char *fileName);
    void autofillStrandGaps();
    struct Base {
        std::string name, helixName, materialName;
        std::vector<double> position;
        std::string label;

        inline Base(const char *name, const std::vector<double> & position, const char *materialName, std::string & label) : name(name), materialName(materialName), position(position), label(label) {}
        inline Base() {}
    };
    struct Connection {
        enum Type {
            kForwardThreePrime,
            kForwardFivePrime,
            kBackwardThreePrime,
            kBackwardFivePrime,
            kNamed,
            kInvalid
        } fromType, toType;

        std::string fromHelixName, toHelixName, fromName, toName; // Only used when fromType/toType are kNamed.

        inline Connection(const char *fromHelixName, const char *fromName, const char *toHelixName, const char *toName, Type fromType, Type toType) : fromType(fromType), toType(toType), fromHelixName(fromHelixName), toHelixName(toHelixName), fromName(fromName), toName(toName) {}

        static Type TypeFromString(const char *type);
    };
    #if defined(WIN32) || defined(WIN64)
        typedef std::unordered_map<std::string, DNA::Name> explicit_base_labels_t;
    #else
        typedef std::tr1::unordered_map<std::string, DNA::Name> explicit_base_labels_t;
    #endif /* Not windows */
    std::string rpoly_;
    std::vector<Model::Helix> helices;
    std::vector<Connection> connections;
    std::vector<Model::Strand> strands;
    std::vector<Model::Base> bases;
    // bases_ext - vector for storing autofilled strand gaps. This is the only case where existing parts of a structure are extended in the original intended use of this software
    // Since the structure is navigated through direct pointers for efficiency, the existing vectors of bases should not be extended due to memory relocation.
    std::vector<Model::Base> bases_ext;

    std::vector<Base> explicitBases; // Bases explicitly created with the 'b' command.
    explicit_base_labels_t explicitBaseLabels;

private:

    friend std::string getBaseFromConnectionType(Model::Helix & helix, Connection::Type type, Model::Base & base);

};

class Scaffold_free : public Design
{
public:
    Scaffold_free(const std::string meshpath, const std::string meshname);
    int createGraph();
    int find_trail();
    bool write_trails();
    int main();
    int relax(const QVector<QVariant> args);
    int create_strands();
    int create_random_sequences();

private:
    std::vector<std::vector<dir::Vertex>> nodetrail;
    dir::DiGraph graph;
};