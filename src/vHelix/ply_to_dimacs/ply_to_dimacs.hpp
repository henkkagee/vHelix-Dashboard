#ifndef PLY_TO_DIMACS
#define PLY_TO_DIMACS

#include <boost/graph/connected_components.hpp>


#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <algorithm>
#include <sstream>

#define NDEBUG
#include <assert.h>
#include "ply_to_dimacs/boost_graph_helper.hpp"

// For console output to GUI
// not a good solution but it works
#include "vhelix.h"

namespace ply_to_dimacs {
    bool read_ply(std::string input_filename, Graph &G, vHelix &parent);
    int main(int argc, const char* argv[], vHelix &parent);
}

#endif
