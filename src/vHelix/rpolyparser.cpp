/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#include "rpolyparser.h"
#include "model.h"

#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <functional>
#include <iostream>

#include <QVector4D>
#include <QVector3D>
#include <QMatrix3x3>

// FIXME: Qt5 progress bar
//#include <maya/MProgressWindow.h>
#define BUFFER_SIZE 1024
// 0.84 scaling is ad hoc solution to get good looking models
#define POSITION_SCALING 0.84f
Controller::Parser::Parser()
{

}

namespace Controller {
    struct non_nicked_strand_t {
        Model::Strand strand;
        // Bases together with their calculated offset along the strand.
        std::vector< std::pair<Model::Base, int> > bases;

        inline non_nicked_strand_t(Model::Base & base) : strand(), bases(1, std::make_pair(base, 0)) {}
        inline void add_base(Model::Base & base) {
            bases.push_back(std::make_pair(base, 0));
        }
    };

    struct base_offset_comparator_t : public std::binary_function<std::pair<Model::Base, int>, std::pair<Model::Base, int>, bool> {
        const int offset;

        inline base_offset_comparator_t(int offset) : offset(offset) {}

        inline bool operator() (const std::pair<Model::Base, int> & p1, const std::pair<Model::Base, int> & p2) const {
            return std::abs(p1.second - offset) < std::abs(p2.second - offset);
        }
    };

    Parser::Connection::Type Parser::Connection::TypeFromString(const char *type) {
        if (strcmp("f5'", type) == 0)
            return kForwardFivePrime;
        else if (strcmp("f3'", type) == 0)
            return kForwardThreePrime;
        else if (strcmp("b5'", type) == 0)
            return kBackwardFivePrime;
        else if (strcmp("b3'", type) == 0)
            return kBackwardThreePrime;
        else
            return kNamed;
    }


    /* reads an rPoly file and creates helices, bases and connections. Unfortunately there is something fundamentaly wrong
     with how the transforms (both positions and orientations) are handled so this remains unused */
    std::vector<Model::Helix> Parser::readRpoly(const char *filename, int nicking_min_length, int nicking_max_length) {

        //clear vectors if they have been in use from before
        helices.clear();
        connections.clear();
        strands.clear();
        bases.clear();
        explicitBases.clear();
        explicitBaseLabels.clear();

        std::string status;
        std::ifstream file(filename);

        if (file.fail()) {
            std::cout << "Failed to open file \"%s\"" << filename;
            std::vector<Model::Helix> ret;
            return ret;
        }
        std::cout << "Parsing...\n" << std::flush;
        std::vector<double> pos(3);
        std::vector<double> ori(4); // x, y, z, w
        unsigned int bases;
        char nameBuffer[BUFFER_SIZE], helixNameBuffer[BUFFER_SIZE], materialNameBuffer[BUFFER_SIZE], targetNameBuffer[BUFFER_SIZE], targetHelixNameBuffer[BUFFER_SIZE];
        char label;
        bool autostaple(true);
        std::vector< std::pair<std::string, std::string> > paintStrands;
        std::vector<Model::Base*> paintStrandBases, disconnectBackwardBases;

        std::vector<Model::Base*> nonNickedBases;
        // check how many helices were dealing with so we can reserve space in helices[] beforehand
        // to avoid vector reallocation
        long int num_helices = 0;
        while (file.good()) {
            std::string line;
            std::getline(file, line);
            if (sscanf(line.c_str(), "h %s %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 8) {
                num_helices++;
            }
            else if (sscanf(line.c_str(), "hb %s %u %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &bases, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 9) {
                num_helices++;
            }
        }
        file.clear();
        file.seekg(0);
        helices.reserve(num_helices + 1);
        std::cout << "ORIENTATIONS" << std::endl;
        while (file.good()) {
            std::string line;
            std::getline(file, line);
            QVector3D position;
            if (sscanf(line.c_str(), "h %s %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 8) {
                // 0.84 scaling is ad hoc solution to get good looking models
                position.setX(pos[0]/POSITION_SCALING);  position.setY(pos[1]/POSITION_SCALING); position.setZ(pos[2]/POSITION_SCALING);
                QQuaternion orientation(ori[3], QVector3D(ori[0], ori[1], ori[2]));
                helices.push_back(Model::Helix(position, orientation, nameBuffer));
                for (int num_bases = 0; num_bases < helices.back().bases_; num_bases++) {
                    helices.back().Fbases[num_bases].parent = &helices.back();
                    helices.back().Bbases[num_bases].parent = &helices.back();
                }
            }
            else if (sscanf(line.c_str(), "hb %s %u %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &bases, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 9) {
                // 0.84 scaling is ad hoc solution to get good looking models
                position.setX(pos[0]/POSITION_SCALING);  position.setY(pos[1]/POSITION_SCALING); position.setZ(pos[2]/POSITION_SCALING);
                //QQuaternion orientation(ori[3], QVector3D(ori[0], ori[1], ori[2]));
                //QQuaternion dir = QQuaternion::fromDirection(QVector3D(0.65f, -0.76f, 0.0f), QVector3D(0.f, 1.f, 0.f));
                //orientation = dir * orientation;
                // QQuaternion orientation = QQuaternion::fromAxisAndAngle(QVector3D(ori[0], ori[1], ori[2]), ori[3]);
                QQuaternion orientation;
                float *values = new float[9];
                values[0] = 1-2*pow(ori[1],2)-2*pow(ori[2],2);
                values[1] = 2*ori[0]*ori[1]-2*ori[2]*ori[3];
                values[2] = 2*ori[0]*ori[2]+2*ori[1]*ori[3];
                values[3] = 2*ori[0]*ori[1]+2*ori[2]*ori[3];
                values[4] = 1-2*pow(ori[0],2)-2*pow(ori[2],2);
                values[5] = 2*ori[1]*ori[2]-2*ori[0]*ori[3];
                values[6] = 2*ori[0]*ori[2]-2*ori[1]*ori[3];
                values[7] = 2*ori[1]*ori[2]+2*ori[0]*ori[3];
                values[8] = 1-2*pow(ori[0], 2)-2*pow(ori[1],2);
                std::cout << "Values: " << std::endl;
                for (int i = 0; i < 9; i++) {
                    std::cout << values[i] << " ";
                }
                std::cout << std::endl;
                orientation = QQuaternion::fromRotationMatrix(QMatrix3x3(values));
                std::cout << "Orientation: " << orientation.x() << " " << orientation.y() << " " << orientation.z() << std::endl;
                helices.push_back(Model::Helix(position, orientation, nameBuffer, bases));
                for (int num_bases = 0; num_bases < helices.back().bases_; num_bases++) {
                    helices.back().Fbases[num_bases].parent = &helices.back();
                    helices.back().Bbases[num_bases].parent = &helices.back();
                }
                delete[] values;
            }
            /*else if (sscanf(line.c_str(), "b %s %s %lf %lf %lf %s %c", nameBuffer, helixNameBuffer, &position[0], &position[1], &position[2], materialNameBuffer, &label) == 7) {
                std::vector<Model::Helix>::iterator helix_it(std::find(helices.begin(), helices.end(), helixNameBuffer));
                explicitBases.push_back(Model::Base(nameBuffer, position, materialNameBuffer, label));
            }*/
            else if (sscanf(line.c_str(), "c %s %s %s %s", helixNameBuffer, nameBuffer, targetHelixNameBuffer, targetNameBuffer) == 4) {
                connections.push_back(Connection(helixNameBuffer, nameBuffer, targetHelixNameBuffer, targetNameBuffer, Connection::TypeFromString(nameBuffer), Connection::TypeFromString(targetNameBuffer)));
            }
            else if (sscanf(line.c_str(), "l %s %c", nameBuffer, &label) == 2) {
                explicitBaseLabels.insert(std::make_pair(nameBuffer, label));
            }
            else if (sscanf(line.c_str(), "ps %s %s", helixNameBuffer, nameBuffer) == 2) {
                paintStrands.push_back(std::make_pair(std::string(helixNameBuffer), std::string(nameBuffer)));
            }
            else if (line == "autostaple" || line == "autonick") {
                autostaple = true;
            }
        }

        // Now create the helices, bases and make the connections.
        #if defined(WIN32) || defined(WIN64)
                    typedef std::unordered_map<std::string, Model::Helix> string_helix_map_t;
        #else
                    typedef std::tr1::unordered_map<std::string, Model::Helix> string_helix_map_t;
        #endif /* N Windows */
        string_helix_map_t helixStructures;

        for (std::vector<Model::Helix>::iterator it(helices.begin()); it != helices.end(); ++it) {
            std::cout << it->name_ << std::endl << std::flush;
            //helixStructures.insert(std::make_pair(it->name_, *it));
            if (autostaple && it->bases_ > 1) {
                Model::Base *base = it->getBackwardThreePrime();

                // Disconnect base at backward 5' + floor(<num bases> / 2)
                for (int i = 0; i < (it->bases_ - 1) / 2 + 1; ++i) {
                    base = base->getBackward();
                    if (base == nullptr) {
                        std::cout << "baseError\n" << std::flush;
                        break;
                    }
                }
                if (it->bases_ > nicking_min_length) {
                    std::cout << "Base at " << base->getParent()->name_ << " in pos " << base->offset_ << " should be disconnected. Parent is: " << base->getParent() << "\n" << std::flush;
                    disconnectBackwardBases.push_back(base);
                    paintStrandBases.push_back(base);
                }
                else {
                    // Add this edge to non-nicked strands but only add the strand once.
                    //HPRINT("Not nicking %s because only %u bases", base.getDagPath(status).fullPathName().asChar(), it->bases);
                    nonNickedBases.push_back(base);
                }
            }
        }
        for (std::vector<Connection>::iterator it(connections.begin()); it != connections.end(); it++) {
            Model::Helix *fromHelix;
            Model::Helix *toHelix;
            Model::Base *fromBase;
            Model::Base *toBase;
            bool fromHelixFound = false; bool toHelixFound = false;
            for (auto &h : helices) {
               if (h.name_ == it->fromHelixName) {
                   fromHelix = &h;
                   fromHelixFound = true;
               }
               if (h.name_ == it->toHelixName) {
                   toHelix = &h;
                   toHelixFound = true;
               }
            }
            if (fromHelixFound && toHelixFound) {
                if (it->fromName == "f3'") {
                    fromBase = fromHelix->getForwardThreePrime();
                }
                else if (it->fromName == "f5'") {
                    fromBase = fromHelix->getForwardFivePrime();
                }
                else if (it->fromName == "b3'") {
                    fromBase = fromHelix->getBackwardThreePrime();
                }
                else if (it->fromName == "b5'") {
                    fromBase = fromHelix->getBackwardFivePrime();
                }

                if (it->toName == "f3'") {
                    toBase = toHelix->getForwardThreePrime();
                }
                else if (it->toName == "f5'") {
                    toBase = toHelix->getForwardFivePrime();
                }
                else if (it->toName == "b3'") {
                    toBase = toHelix->getBackwardThreePrime();
                }
                else if (it->toName == "b5'") {
                    toBase = toHelix->getBackwardFivePrime();
                }

                // connect the bases from the two strands
                fromBase->setForward(toBase);
                fromBase->strand_forward = toBase;
                toBase->setBackward(fromBase);
                toBase->strand_backward = fromBase;
            }
        }
        for (auto k : disconnectBackwardBases) {
            Model::Base *b = k->getBackward();
            k->checked = 96;
            k->setBackward(nullptr);
            b->setForward(nullptr);
        }

        // create the strands
        Model::Helix *h = &helices.front();
        Model::Base *current;
        current = h->getForwardFivePrime();
        Model::Strand strand;
        bool forward;
        std::string *name1;
        // forward and backward strands throughout structure
        for (int i = 0; i < 2; i++) {
            bool first = true;
            if (i == 0) {
                current = h->getForwardFivePrime();
                forward = true;
            }
            else {
                current = h->getBackwardThreePrime();
                forward = false;
            }
            while (true) {
                if ((current->in_strand)) {
                    strand.name_ = *name1 + "->" + current->strand_backward->parent->name_;
                    // check for identical strand names
                    {
                        int tries = 1;
                        while(true) {
                            bool unique = true;
                            for (const auto &st : strands) {
                                if (st.name_ == strand.name_) {
                                    strand.name_ += "_" + std::to_string(tries+1);
                                    tries++;
                                    unique = false;
                                }
                            }
                            if (!unique) {
                                continue;
                            }
                            break;
                        }
                    }
                    if (forward) {
                        strand.forward_ = true;
                    }
                    else {
                        strand.forward_ = false;
                    }
                    strands.push_back(strand);
                    for (auto& b : strand.bases_) {
                        b->strandname_ = strand.name_;
                    }
                    strand = Model::Strand();
                    break;
                }
                current->in_strand = true;
                if (first) {
                    name1 = &(current->getParent()->name_);
                }
                if (!first) {
                    if (current->forward == nullptr) {
                        strand.name_ = *name1 + "->" + current->parent->name_;
                        // check for identical strand names
                        {
                            int tries = 1;
                            while(true) {
                                bool unique = true;
                                for (const auto &st : strands) {
                                    if (st.name_ == strand.name_) {
                                        strand.name_ += "_" + std::to_string(tries+1);
                                        tries++;
                                        unique = false;
                                    }
                                }
                                if (!unique) {
                                    continue;
                                }
                                break;
                            }
                        }
                        strand.bases_.push_back(current);
                        strand.length_ += 1;
                        if (forward) {
                            strand.forward_ = true;
                        }
                        else {
                            strand.forward_ = false;
                        }
                        strands.push_back(strand);
                        for (auto& b : strand.bases_) {
                            b->strandname_ = strand.name_;
                        }
                        strand = Model::Strand();
                        current = current->strand_forward;
                        first = true;
                        continue;
                    }
                }
                strand.bases_.push_back(current);
                strand.length_ += 1;
                first = false;
                current = current->strand_forward;
                if (!current) {
                    break;
                }

            }
        }
        long int total_strand_lengths = 0;
        long int total_bases = 0;
        for (const auto &h : helices) {
            for (const auto &b : h.Bbases) {
                total_bases++;
            }
            for (const auto &b : h.Fbases) {
                total_bases++;
            }
        }
        for (auto str : strands) {
            total_strand_lengths += str.length_;
        }
        if (total_strand_lengths < total_bases) {
            for (auto &h : helices) {
                for (auto &b : h.Bbases) {
                    if (!b.in_strand) {
                        Model::Strand newstrand = Model::Strand();
                        std::string firstname = b.getParent()->name_;
                        newstrand.length_ = 0;
                        newstrand.bases_.push_back(&b);
                        b.in_strand = true;
                        newstrand.length_++;
                        Model::Base* next;
                        Model::Base* previous;
                        bool cut_forward = false; bool cut_backward = false;
                        long long int its = 0;
                        next = b.strand_forward;
                        previous = b.strand_backward;
                        while (true) {
                            if (!next->in_strand) {
                                if (b.getForward() == nullptr) {
                                    cut_forward = true;
                                }
                                if (!cut_forward) {
                                    newstrand.bases_.push_back(next);
                                    next->in_strand = true;
                                    newstrand.length_++;
                                }
                            }
                            if (!previous->in_strand) {
                                if (b.getBackward() == nullptr) {
                                    cut_backward = true;
                                }
                                if (!cut_backward) {
                                    newstrand.bases_.insert(newstrand.bases_.begin(), previous);
                                    previous->in_strand = true;
                                    newstrand.length_++;
                                }
                            }
                            if (newstrand.length_ < nicking_min_length) {
                                next = next->strand_forward;
                                previous = previous->strand_backward;

                                if ((next->in_strand && previous->in_strand) || (cut_forward && cut_backward)) {
                                    newstrand.name_ = firstname + "->" + next->strand_backward->getParent()->name_;
                                    // check for identical strand names
                                    {
                                        int tries = 1;
                                        while(true) {
                                            bool unique = true;
                                            for (const auto &st : strands) {
                                                if (st.name_ == newstrand.name_) {
                                                    newstrand.name_ += "_" + std::to_string(tries+1);
                                                    tries++;
                                                    unique = false;
                                                }
                                            }
                                            if (!unique) {
                                                continue;
                                            }
                                            break;
                                        }
                                    }
                                    strands.push_back(newstrand);
                                    for (auto& b : newstrand.bases_) {
                                        b->strandname_ = newstrand.name_;
                                    }
                                    break;
                                }
                            }
                            else {
                                newstrand.name_ = firstname + "->" + next->strand_backward->getParent()->name_;
                                // check for identical strand names
                                {
                                    int tries = 1;
                                    while(true) {
                                        bool unique = true;
                                        for (const auto &st : strands) {
                                            if (st.name_ == newstrand.name_) {
                                                newstrand.name_ += "_" + std::to_string(tries+1);
                                                tries++;
                                                unique = false;
                                            }
                                        }
                                        if (!unique) {
                                            continue;
                                        }
                                        break;
                                    }
                                }
                                strands.push_back(newstrand);
                                for (auto& b : newstrand.bases_) {
                                    b->strandname_ = newstrand.name_;
                                }
                                break;
                            }
                            its++;
                        }
                    }
                }
            }
        }
        std::cout << "Number of strands: " << strands.size() << std::endl << std::flush;
        for (std::vector<Model::Strand>::iterator s(strands.begin()); s != strands.end(); s++) {

            if (s->length_ < nicking_min_length) {
                Model::Base **safe_bases = new Model::Base*[s->length_];
                for (int i = 0; i < s->length_; i++) {
                    safe_bases[i] = s->bases_[i];
                }
                Model::Base *b = s->bases_[0];
                b = b->strand_backward;
                Model::Strand *strand_to_expand;
                for (unsigned long long i = 0; i < strands.size(); i++) {
                    if (b->strandname_ == strands[i].name_) {
                        strand_to_expand = &strands[i];
                        break;
                    }
                }
                strand_to_expand->length_ += s->length_;
                for (int i = 0; i < s->length_; i++) {
                    strand_to_expand->bases_.push_back(s->bases_[i]);
                    strand_to_expand->bases_[0]->strandname_ = strand_to_expand->name_;
                }
                delete[] safe_bases;
                s = strands.erase(s);
                s--;
            }
        }
        long long int totalbases = 0;
        for (auto &s : strands) {
            totalbases += s.length_;
            std::cout << s.length_ << std::endl << std::flush;
            if (s.forward_) {
                s.scaffold_ = true;
            }
        }
        std::cout << "Total bases: " << totalbases << "\n" << std::flush;
        return helices;
    }


    // Temporary implementation using OxDNA files to visualize the structure
    std::vector<Model::Strand> Parser::readOxDNA(const char *conffile, const char *topfile) {
        std::string status;
        std::ifstream file(topfile);
        if (file.fail()) {
            std::cout << "Failed to open file \"%s\"" << topfile << std::endl << std::flush;
            std::vector<Model::Strand> ret;
            return ret;
        }
        std::cout << "Parsing...\n" << std::flush;
        char firstline = 1;
        unsigned long long int nbases = 0;
        unsigned long long int nstrands = 0;
        unsigned long long int strandNo, base3neigh, base5neigh;
        unsigned char nucleotide;
        // reading the topology file first with connection and strand information
        unsigned long long int linecount = 0;
        while (file.good()) {
            std::string line;
            std::getline(file, line);
            if (firstline == 1) {
                if (sscanf(line.c_str(), "%lld %lld", &nbases, &nstrands) == 2) {
                    bases.reserve(nbases);
                    strands.reserve(nstrands);
                }
                else {
                    std::vector<Model::Strand> ret;
                    return ret;
                }
                firstline++;
            }

            else if (sscanf(line.c_str(), "%lld %c %lld %lld", &strandNo, &nucleotide, &base3neigh, &base5neigh) == 4) {
                bases.push_back(Model::Base(strandNo, nucleotide, base3neigh, base5neigh, linecount));
                if (strands.size() < strandNo) {
                    strands.push_back(Model::Strand(0, strandNo));
                }
                strands[strandNo-1].bases_.push_back(&(bases.back()));
                strands[strandNo-1].length_ += 1;
            }
            linecount++;
        }

        // now read the configuration file with base coordinates and rotations
        std::ifstream conf(conffile);
        if (conf.fail()) {
            std::cout << "Failed to open configuration file \"%s\"" << conffile;
            std::vector<Model::Strand> ret;
            return ret;
        }
        linecount = 1;
        std::vector<double> pos(3);
        std::vector<double> bbv(3); // backbone-base unit vector
        std::vector<double> nv(3); // normal unit vector, same for all bases belonging to the same strand
        std::vector<double> v(3); // velocity of nucleotide, not used
        std::vector<double> av(3); // angular velococity of nucleotide, not used
        std::vector<double> box_size(3);
        while (conf.good()) {
            std::string line;
            std::getline(conf, line);
            if (linecount <= 3) {
                switch(line[0]) {
                    case 't':   // Timestep information. Not used.
                        break;
                    case 'b':   // box side lengths Lx, Ly, Lz.
                        sscanf(line.c_str(), "b = %lf %lf %lf", &box_size[0], &box_size[0], &box_size[0]);
                        break;
                    case 'E':
                        break;  // simulation energies Etot, Y and K. Not used.
                }
            }
            else if (sscanf(line.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                            &pos[0], &pos[1], &pos[2], &bbv[0], &bbv[1], &bbv[2], &nv[0], &nv[1], &nv[2],
                            &v[0], &v[1], &v[2], &av[0], &av[1], &av[2]) == 15)
                {
                bases[linecount - 4].position = QVector3D(pos[0], pos[1], pos[2]);

                // FIXME: set base rotation later

            }
            linecount++;
        }

        // identify the scaffold strand if there is one
        std::vector<unsigned long int> strand_lengths;
        unsigned long long strands_size = strands.size();
        for (unsigned long long i = 0; i < strands_size; i++) {
            strand_lengths.push_back(strands[i].bases_.size());
        }
        std::sort(strand_lengths.begin(), strand_lengths.end());
        unsigned long long median = strand_lengths[int(strands_size/2)];
        for (unsigned long long j = 0; j < strands_size; j++) {
            std::cout << strand_lengths[j] << " > " << "(median=" << median << " * " << strands_size - 1 << ")...\n" << std::flush;
            if (strand_lengths[j] > (median * (int(strands_size/2) - 5)) && strand_lengths[j] < (median * (int(strands_size/2) + 5))) {
                std::cout << "\n " << j << " is likely a scaffold strand" << std::flush;
                strands[j].scaffold_ = true;
            }
        }

        return strands;
    }
}
