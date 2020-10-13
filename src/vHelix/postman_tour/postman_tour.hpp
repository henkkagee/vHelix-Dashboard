#ifndef POSTMAN_TOUR
#define POSTMAN_TOUR

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include "ply_to_dimacs/boost_graph_helper.hpp"

// Lemon compiled for x64 Windows systems...
#include "lemon/list_graph.h"
#include "lemon/matching.h"
#include "lemon/concepts/graph.h"
#include "lemon/full_graph.h"

// For console output to GUI
// not a good solution but it works
#include "vhelix.h"

#define NDEBUG
#ifdef DEBUG
#define DEBUGPRINT( x ) x
#else
#define DEBUGPRINT( x )
#endif

namespace postman_tour {

    typedef lemon::FullGraph::EdgeMap<signed int> Distances;
    typedef lemon::MaxWeightedPerfectMatching<lemon::FullGraph, Distances> MWPM;

    int main(int argc, const char* argv[], vHelix &parent);
    bool is_even(Graph G);
}

#endif
