#ifndef DESIGN_H
#define DESIGN_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>
#include <map>
#include <random>

#include "happly-master/happly.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <QVariant>
#include <QVector>
#include <include/json/single_include/nlohmann/json.hpp>
#include "relaxation.h"
#include "physxrelaxation.h"
#include "definitions.h"
#include "model.h"
#include "modelhandler.h"



typedef struct {
    size_t forw;
    size_t back;
} neighbours;

typedef std::vector<std::vector<size_t> > embedding_t;

enum parity { odd_even, even_odd }; // odd_even = odd follows even (1 follows 0, 3 follows 2, ...), even_odd = even follows odd (2 follows 1, 4 follows 3, ... 0 follows d-1)

class Design
{
public:
    Design(const std::string meshpath, const std::string meshname);
    virtual ~Design() {}

    int read3Dobject();
    int readPLY();
    int readOBJ();

    int createEmbedding();
    std::string getoutstream();
    bool writeEmbedding();
    int writeOxView();
    virtual int main() { // Overloaded in subclasses, e.g. Atrail
        outstream << "ERROR: Desing method not specified\n";
        return 0;
    }
    virtual int relax(const QVector<QVariant> args) { // Overloaded in subclasses, e.g. Atrail
        outstream << "ERROR: Desing method not specified\n";
        return 0;
    }
    virtual void generate_sequences(std::string &s) {
        outstream << "ERROR: Desing method not specified\n";
    }

    Controller::Handler model_;

protected:
    std::string name;
    std::string path;
    std::vector<coordinates> vertices;
    std::vector<std::vector<unsigned long long>> faces;
    std::vector<std::vector<unsigned long long>> non_face_edges;
    std::vector<std::set<unsigned long long>> adj_vertices;
    unsigned int number_vertices;
    unsigned int number_faces;
    unsigned int number_edges; //non-face
    std::string rpoly_code;
    embedding_t embedding;
    std::stringstream outstream;
    std::stringstream filestream;
    bool write_intermediates;


};

#endif // DESIGN_H
