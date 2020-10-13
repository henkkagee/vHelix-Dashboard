#ifndef MAKE_EMBEDDING_EULERIAN
#define MAKE_EMBEDDING_EULERIAN

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

#define NDEBUG
#ifdef DEBUG
#define DEBUGPRINT( x ) x
#else
#define DEBUGPRINT( x )
#endif

// For console output to GUI
// not a good solution but it workss
#include "vhelix.h"

/*
* make_embedding_eulerian.cpp
*
*  Created on: July 14, 2015
*  Author: Abdulmelik Mohammed
*  Creates an eulerian embedding as an edge code from an embedding given as an vertex code and the associated multigraph containing the multiedges.
*  Argument 1: filename of input embedding in vcode format.
*  Argument 2: filename of input multigraph in dimacs format.
*  Argument 3 (optional): filename of output edge code, if not given the input vcode filename with an extension .ecode is used.
*/

namespace make_embedding_eulerian {

	int main(int argc, const char* argv[], vHelix &parent);
	
	typedef std::vector<std::vector<std::size_t> > embedding_t;
	typedef struct {
		size_t vertex;
		signed long int edge;
	}vertex_edge_pair;
}

#endif
