#ifndef ATRAIL_SEARCH
#define ATRAIL_SEARCH

/**
* Searches for an A-trail from an edge code file.
* Argument 1: the edge code file.
* Argument 2 (Optional): a trail file as a sequence of edge indices. This has an extension '.trail'.
* Argument 3 (Optional): a trail file as a sequence of node indices(zero based). The trail ends with the vertice it began. The output file has extension `.ntrail'.
*/
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/config.hpp>
#include <vector>
#include <list>
#include <string>
#include <algorithm>    // std::find
#include <fstream>
#include "Atrail.hpp"

// For console output to GUI
// not a good solution but it works
#include "vhelix.h"

namespace atrail_search {

	int main(int argc, const char* argv[], vHelix &parent);
	
}

#endif
