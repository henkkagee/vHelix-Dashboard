#ifndef DNARELAXATION_H
#define DNARELAXATION_H

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <math.h>
#include <vector>
#include "definitions.h"
#include <QDebug>

namespace Debug {
        void printf(const char *file, const char *function, size_t line, const char *expr, ...);
        void printf_noreturn(const char *file, const char *function, size_t line, const char *expr, ...);
}


class Relaxation
{
public:
    Relaxation(std::string & name) : name(name) {}
    virtual int scaffold_main(std::vector<coordinates> &input_vertices,std::vector<unsigned int> &input_path) {
        outstream << "ERROR: relaxation mode not specified";
        std::cerr << "ERROR: relaxation mode not specified"<< std::endl;
        return 0;
    }
    virtual int scaffold_free_main(std::vector<coordinates> &input_vertices,std::vector<std::vector<unsigned int>> &input_path) {
        outstream << "ERROR: relaxation mode not specified";
        std::cerr << "ERROR: relaxation mode not specified"<< std::endl;
        return 0;
    }
    std::string getoutstream() {
        return outstream.str();
    }
protected:
    std::stringstream outstream;
    std::string name;
};

#endif
