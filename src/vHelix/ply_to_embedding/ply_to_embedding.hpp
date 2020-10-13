#ifndef PLY_TO_EMBEDDING
#define PLY_TO_EMBEDDING

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <set>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <assert.h>

// For console output to GUI
// not a good solution but it works
#include "vhelix.h"

#define NDEBUG
#ifdef DEBUG
#define DEBUGPRINT( x ) x
#else
#define DEBUGPRINT( x )
#endif


namespace ply_to_embedding {

    // the next and previous vertices of a vertex in a face description
    typedef struct {
        size_t forw;
        size_t back;
    } neighbours;

    typedef std::vector<std::vector<size_t> > embedding_t;


    int main(int argc, const char* argv[], vHelix &parent);
    bool ply_to_embedding(std::string input_filename, embedding_t& embedding, vHelix &parent);
}

#endif
