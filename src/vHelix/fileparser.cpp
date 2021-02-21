/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 * Class for parsing rpoly and oxdna file formats.
*/

#include "fileparser.h"
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

using json = nlohmann::json;

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
        unsigned long bIdStart = 0;
        while (file.good()) {
            std::string line;
            std::getline(file, line);
            QVector3D position;
            if (sscanf(line.c_str(), "h %s %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 8) {
                // 0.84 scaling is ad hoc solution to get good looking models
                // no explicit base length per helix strand specified... this should not be used currently
                // TODO

                std::vector<Model::Helix> helices_empty;
                return helices_empty;

                position.setX(pos[0]/POSITION_SCALING);  position.setY(pos[1]/POSITION_SCALING); position.setZ(pos[2]/POSITION_SCALING);
                QQuaternion orientation(ori[3], QVector3D(ori[0], ori[1], ori[2]));
                helices.push_back(Model::Helix(position, orientation, nameBuffer, bIdStart));
                for (int num_bases = 0; num_bases < helices.back().bases_; num_bases++) {
                    helices.back().Fbases_[num_bases].setParent(&helices.back());
                    helices.back().Bbases_[num_bases].setParent(&helices.back());
                }
            }
            else if (sscanf(line.c_str(), "hb %s %u %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &bases, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 9) {
                // 0.84 scaling is ad hoc solution to get good looking models
                position.setX(pos[0]/POSITION_SCALING);  position.setY(pos[1]/POSITION_SCALING); position.setZ(pos[2]/POSITION_SCALING);
                QQuaternion orientation;
                float *values = new float[9];
                // helix rotation x,y,z and w to 3x3 rotation matrix
                // no Qt method for this?
                values[0] = 1-2*pow(ori[1],2)-2*pow(ori[2],2);
                values[1] = 2*ori[0]*ori[1]-2*ori[2]*ori[3];
                values[2] = 2*ori[0]*ori[2]+2*ori[1]*ori[3];
                values[3] = 2*ori[0]*ori[1]+2*ori[2]*ori[3];
                values[4] = 1-2*pow(ori[0],2)-2*pow(ori[2],2);
                values[5] = 2*ori[1]*ori[2]-2*ori[0]*ori[3];
                values[6] = 2*ori[0]*ori[2]-2*ori[1]*ori[3];
                values[7] = 2*ori[1]*ori[2]+2*ori[0]*ori[3];
                values[8] = 1-2*pow(ori[0], 2)-2*pow(ori[1],2);
                orientation = QQuaternion::fromRotationMatrix(QMatrix3x3(values));
                helices.push_back(Model::Helix(position, orientation, nameBuffer, bIdStart, bases));
                for (int num_bases = 0; num_bases < helices.back().bases_; num_bases++) {
                    helices.back().Fbases_[num_bases].setParent(&helices.back());
                    helices.back().Bbases_[num_bases].setParent(&helices.back());
                }
                bIdStart += helices.back().bases_*2;
                //std::cout << "Bases: " << helices.back().bases_ << " -- next bIdStart: " << bIdStart << std::endl << std::flush;
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
            //std::cout << it->name_ << std::endl << std::flush;
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
                    //std::cout << "Base at " << base->getParent()->name_ << " in pos " << base->offset_ << " should be disconnected. Parent is: " << base->getParent() << "\n" << std::flush;
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
                fromBase->strand_forward_ = toBase;
                toBase->setBackward(fromBase);
                toBase->strand_backward_ = fromBase;
            }
        }
        for (auto k : disconnectBackwardBases) {
            Model::Base *b = k->getBackward();
            k->checked_ = true;
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
                if ((current->in_strand_)) {
                    strand.name_ = *name1 + "->" + current->strand_backward_->getParent()->name_;
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
                current->in_strand_ = true;
                if (first) {
                    name1 = &(current->getParent()->name_);
                }
                if (!first) {
                    if (current->getForward() == nullptr) {
                        strand.name_ = *name1 + "->" + current->getParent()->name_;
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
                        current = current->strand_forward_;
                        first = true;
                        continue;
                    }
                }
                strand.bases_.push_back(current);
                strand.length_ += 1;
                first = false;
                current = current->strand_forward_;
                if (!current) {
                    break;
                }

            }
        }
        long int total_strand_lengths = 0;
        long int total_bases = 0;
        for (const auto &h : helices) {
            for (const auto &b : h.Bbases_) {
                total_bases++;
            }
            for (const auto &b : h.Fbases_) {
                total_bases++;
            }
        }
        for (auto str : strands) {
            total_strand_lengths += str.length_;
        }
        if (total_strand_lengths < total_bases) {
            for (auto &h : helices) {
                for (auto &b : h.Bbases_) {
                    if (!b.in_strand_) {
                        Model::Strand newstrand = Model::Strand();
                        std::string firstname = b.getParent()->name_;
                        newstrand.length_ = 0;
                        newstrand.bases_.push_back(&b);
                        b.in_strand_ = true;
                        newstrand.length_++;
                        Model::Base* next;
                        Model::Base* previous;
                        bool cut_forward = false; bool cut_backward = false;
                        long long int its = 0;
                        next = b.strand_forward_;
                        previous = b.strand_backward_;
                        while (true) {
                            if (!next->in_strand_) {
                                if (b.getForward() == nullptr) {
                                    cut_forward = true;
                                }
                                if (!cut_forward) {
                                    newstrand.bases_.push_back(next);
                                    next->in_strand_ = true;
                                    newstrand.length_++;
                                }
                            }
                            if (!previous->in_strand_) {
                                if (b.getBackward() == nullptr) {
                                    cut_backward = true;
                                }
                                if (!cut_backward) {
                                    newstrand.bases_.insert(newstrand.bases_.begin(), previous);
                                    previous->in_strand_ = true;
                                    newstrand.length_++;
                                }
                            }
                            if (newstrand.length_ < nicking_min_length) {
                                next = next->strand_forward_;
                                previous = previous->strand_backward_;

                                if ((next->in_strand_ && previous->in_strand_) || (cut_forward && cut_backward)) {
                                    newstrand.name_ = firstname + "->" + next->strand_backward_->getParent()->name_;
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
                                newstrand.name_ = firstname + "->" + next->strand_backward_->getParent()->name_;
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

        // divide strands so none are longer than nicking_max_length
        /*
        bool pass = false;
        while (!pass) {
            if (nicking_max_length > 0) {
                for (int i = 0; i < strands.size(); i++) {
                    if (strands[i].length_ > nicking_max_length) {
                        if (strands[i].forward_) {
                            continue;
                        }
                        std::cout << "Strand " << i << " too long at " << strands[i].length_ << std::endl << std::flush;
                        if (strands[i].length_ - nicking_max_length > nicking_max_length) {
                            std::cout << "new" << std::endl << std::flush;
                            // create new strand
                            Model::Strand newstrand = Model::Strand();
                            std::string firstname = strands[i].bases_[nicking_max_length]->parent->name_;
                            for (int b = nicking_max_length; b < strands[i].length_; b++) {
                                newstrand.bases_.push_back(strands[i].bases_[b]);
                            }
                            strands[i].bases_.erase(strands[i].bases_.begin() + nicking_max_length, strands[i].bases_.end());
                            strands[i].length_ = strands[i].bases_.size();
                            newstrand.length_ = newstrand.bases_.size();
                            if (strands[i].forward_) {
                                newstrand.forward_ = true;
                            }
                            else {
                                newstrand.forward_ = false;
                            }
                            i = -1;
                            continue;
                        }
                        else {
                            std::cout << "extend" << std::endl << std::flush;
                            bool forw = false; bool backw = false;
                            if (strands[i].bases_[0]->getBackward()) {
                                backw = true;
                            }
                            if (strands[i].bases_[0]->getForward()) {
                                forw = true;
                            }
                            int extraBases = strands[i].length_ - nicking_max_length;
                            if (forw && backw) {
                                std::string findb = strands[i].bases_[0]->getBackward()->strandname_;
                                std::string findf = strands[i].bases_[0]->getForward()->strandname_;
                                Model::Strand* backws; Model::Strand* forws;
                                for (int s = 0; s < strands.size(); s++) {
                                    if (strands[s].name_ == findb) {
                                        backws = &strands[s];
                                    }
                                    if (strands[s].name_ == findf) {
                                        forws = &strands[s];
                                    }
                                }
                                int cnt = 0;
                                for (int bc = nicking_max_length; bc < strands[i].length_; bc++) {
                                    if (cnt < int(extraBases/2)) {
                                        backws->bases_.push_back(strands[i].bases_[bc]);
                                    }
                                    else {
                                        forws->bases_.insert(forws->bases_.begin(), strands[i].bases_[bc]);
                                    }
                                    cnt++;
                                }
                                strands[i].bases_.erase(strands[i].bases_.begin() + nicking_max_length, strands[i].bases_.end());
                                strands[i].length_ = strands[i].bases_.size();
                                backws->length_ = backws->bases_.size();
                                forws->length_ = forws->bases_.size();
                            }
                            i = -1;
                            continue;
                        }
                    }
                    pass = true;
                }
            }
            else {
                pass = true;
            }
        }*/

        std::cout << "Number of strands: " << strands.size() << std::endl << std::flush;
        for (std::vector<Model::Strand>::iterator s(strands.begin()); s != strands.end(); s++) {

            if (s->length_ < nicking_min_length) {
                Model::Base **safe_bases = new Model::Base*[s->length_];
                for (int i = 0; i < s->length_; i++) {
                    safe_bases[i] = s->bases_[i];
                }
                Model::Base *b = s->bases_[0];
                b = b->strand_backward_;
                Model::Strand *strand_to_expand;
                // TODO: empty strands?
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
        unsigned long sId = 0;
        for (auto &s : strands) {
            s.id_ = sId;
            if (s.forward_) {
                s.scaffold_ = true;
            }
            sId++;
        }
        return helices;
    }

    // open oxview model
    std::vector<Model::Strand> Parser::readOxview(const char *path) {

        //clear vectors if they have been in use from before
        helices.clear();
        connections.clear();
        strands.clear();
        bases.clear();
        explicitBases.clear();
        explicitBaseLabels.clear();

        std::string status;
        std::ifstream file(path);
        if (file.fail()) {
            std::cout << "Failed to open file \"%s\"" << path << std::endl << std::flush;
            std::vector<Model::Strand> ret;
            return ret;
        }

        json j = json::parse(file);

        unsigned long basecount = 0;
        for (auto &system : j["systems"]) {
            for (auto &strand : system["strands"]) {
                for (auto &base : strand["monomers"]) {
                    basecount++;
                }
            }
        }
        bases.reserve(basecount+1);

        for (auto &system : j["systems"]) {
            for (auto &strand : system["strands"]) {
                Model::Strand s = Model::Strand();
                s.id_ = strand["id"];
                for (auto &base : strand["monomers"]) {
                    std::stringstream sstr;
                    sstr << base["type"];
                    std::string btype = sstr.str();
                    long n3, n5, bp;
                    if (base.find("n5") != base.end()) {
                        n5 = base["n5"];
                    }
                    else {
                        n5 = -1;
                    }
                    if (base.find("n3") != base.end()) {
                        n3 = base["n3"];
                    }
                    else {
                        n3 = -1;
                    }
                    if (base.find("bp") != base.end()) {
                        bp = base["bp"];
                    }
                    else {
                        bp = -1;
                    }
                    Model::Base b = Model::Base(s.id_, btype, n3, n5, base["id"], bp);
                    QVector3D pos = QVector3D();
                    int index = 0;
                    for (auto &dim : base["p"]) {
                        switch(index) {
                            case 0:
                                pos.setX(dim);
                                break;
                            case 1:
                                pos.setY(dim);
                                break;
                            case 2:
                                pos.setZ(dim);
                                break;
                        }
                        index++;
                    }
                    b.setPos(pos);
                    bases.push_back(b);
                    s.bases_.push_back(&bases.at(bases.size()-1));
                }
                strands.push_back(s);
            }
        }
        // create base connections
        for (auto &b : bases) {
            // base pair
            if (b.oppneigh_ != -1) {
                for (auto &bp : bases) {
                    long bId = bp.baseId_;
                    if (bId == b.oppneigh_) {
                        b.setOpposite(&bp);
                        bp.setOpposite(&b);
                    }
                }
            }
            // 3' neighbour
            if (b.base3neigh_ != -1) {
                for (auto &b3 : bases) {
                    long bId = b3.baseId_;
                    if (bId == b.base3neigh_) {
                        b.setForward(&b3);
                        b3.setBackward(&b);
                    }
                }
            }
            // 5' neighbour
            if (b.base5neigh_ != -1) {
                for (auto &b5 : bases) {
                    long bId = b5.baseId_;
                    if (bId == b.base5neigh_) {
                        b.setBackward(&b5);
                        b5.setForward(&b);
                    }
                }
            }
        }
        int longest = 0;
        unsigned int longestId = 0;
        for (auto &s : strands) {
            s.length_ = s.bases_.size();
            if (s.length_ > longest) {
                longestId = s.id_;
                longest = s.length_;
            }
        }
        for (auto &s : strands) {
            if (s.id_ == longestId) {
                s.scaffold_ = true;
            }
            else {
                s.scaffold_ = false;
            }
        }
        return strands;
    }


}
