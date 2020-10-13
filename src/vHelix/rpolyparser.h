/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#ifndef RPOLYPARSER_H
#define RPOLYPARSER_H

#include <dna.h>

#include "model.h"

#if defined(WIN32) || defined(WIN64)
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif /* N Windows */

#include <vector>
#include <string>

#include <QQuaternion>

namespace Controller {

class Parser
{
public:
    Parser();
    void read();
    std::vector<Model::Helix> readRpoly(const char *filename, int nicking_min_length = 0, int nicking_max_length = 0);

    std::vector<Model::Strand> readOxDNA(const char *conffile, const char *topfile);

    struct Base {
        std::string name, helixName, materialName;
        std::vector<double> position;
        std::string label;

        inline Base(const char *name, const std::vector<double> & position, const char *materialName, std::string & label) : name(name), materialName(materialName), position(position), label(label) {}
        inline Base() {}
    };

    // not used currently
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




    std::vector<Model::Helix> helices;
    std::vector<Connection> connections;
    std::vector<Model::Strand> strands;
    std::vector<Model::Base> bases;

    std::vector<Base> explicitBases; // Bases explicitly created with the 'b' command.
    explicit_base_labels_t explicitBaseLabels;

private:

    friend std::string getBaseFromConnectionType(Model::Helix & helix, Connection::Type type, Model::Base & base);

};

}   // namespace Controller

#endif // RPOLYPARSER_H
