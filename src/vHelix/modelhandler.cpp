/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 * Class for creating structures and storing them
*/

#include "modelhandler.h"
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

Controller::Handler::Handler()
{

}


namespace Controller {

    unsigned int MIN_NICK_VERTEX_DISTANCE = 7;

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

    Handler::Connection::Type Handler::Connection::TypeFromString(const char *type) {
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


    /* Creates a DNA structure from an .rpoly file generated from the BSCOR-pipeline assuming a scaffold- and staple strand configuration*/
    /* You probably don't want to touch this function unless you know what you're doing */
    std::vector<Model::Helix> &Handler::readRpoly(const char *filename, int nicking_min_length, int nicking_max_length) {

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
            return helices;
        }
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

                return helices;

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
        unsigned long sId = 0;
        Model::Strand strand = Model::Strand(0, sId);
        bool forward;

        // forward and backward strands throughout structure
        for (int i = 0; i < 2; i++) {
            if (i == 0) {
                current = h -> getForwardFivePrime();
                forward = true;
            }
            else {
                current = h -> getBackwardFivePrime();
                forward = false;
            }
            while (true) {
                if (!current || (strand.length_ > nicking_min_length + (nicking_max_length - nicking_min_length)/2 && !forward) || current->in_strand_) {
                    if (forward) {
                        // make scaffold strand cyclic
                        current->setBackward(current->strand_backward_);
                        current->strand_backward_->setForward(current);
                    }
                    // make sure that the last strand isn't too short if we looped back to already assigned bases
                    if (strand.length_ < nicking_min_length) {
                        strands.push_back(strand);
                        std::vector<Model::Strand>::iterator strandit = --strands.end();
                        std::vector<Model::Strand>::iterator prevStrand;
                        // loop through last strands and adjust in case they are too short
                        while (strandit->length_ < nicking_min_length) {
                            // get previous strand
                            // auto prevStrand = find_if(strands.begin(), strands.end(), [&prevSId](const Model::Strand& str) {return str.id_ == prevSId;});
                            prevStrand = strandit - 1;
                            // transfer bases from previous strand to current strand until it is long enough
                            while (strandit->length_ < nicking_min_length) {
                                strandit->bases_.insert(strandit->bases_.begin(), prevStrand->bases_[prevStrand->bases_.size()-1]);
                                prevStrand->bases_.pop_back();
                                strandit->length_ = strandit->length_ + 1;
                                prevStrand->length_ = prevStrand->length_ -1;
                            }
                            strandit = prevStrand;
                        }
                    }
                    if (current->in_strand_) {
                        strands.push_back(strand);
                        sId++;
                        strand = Model::Strand(0, sId);
                        break;
                    }
                    strands.push_back(strand);
                    sId++;
                    strand = Model::Strand(0, sId);
                }
                strand.bases_.push_back(current);
                strand.length_ += 1;
                strand.forward_ = forward;
                strand.scaffold_ = forward;
                current->in_strand_ = true;
                current->strandId_ = strand.id_;

                // next
                current = current->strand_forward_;
            }
        }

        // loop through all helices and look for staple loops that weren't added to strands
        sId++;
        strand = Model::Strand(0, sId);
        bool found;
        for (unsigned int i = 0; i < helices.size(); i++) {
            for (int j = 0; j < 2; j++) {
                if (j == 0) {
                    current = helices[i].getForwardFivePrime();
                    forward = true;
                }
                else {
                    current = helices[i].getBackwardFivePrime();
                    forward = false;
                }
                found = false;
                int firstloopid = current->baseId_;
                bool first = true;
                while (true) {
                    // found unassigned loop
                    if (!current->in_strand_) {
                        found = true;
                        if (!current || (strand.length_ > nicking_min_length + (nicking_max_length - nicking_min_length)/2 && !forward) || current->in_strand_) {
                            // make sure that the last strand isn't too short if we looped back to already assigned bases
                            if (current->in_strand_) {
                                int helix_len = current->getParent()->Bbases_.size();
                                int minlen = helix_len > 2.5 * MIN_NICK_VERTEX_DISTANCE ? MIN_NICK_VERTEX_DISTANCE : helix_len / 2;
                                unsigned int nextstrid = current->strandId_;
                                Model::Strand* nextstrand;
                                for (unsigned int ss = 0; ss < strands.size(); ++ss) {
                                    if (strands[ss].id_ == nextstrid) {
                                        nextstrand = &strands[ss];
                                    }
                                }
                                for (int l = 0; l < minlen; l++) {
                                    strand.bases_.push_back(current);
                                    strand.length_++;
                                    current->strandId_ = strand.id_;
                                    nextstrand->bases_.erase(nextstrand->bases_.begin());
                                    nextstrand->length_--;
                                    current = current->strand_forward_;
                                }
                            }
                            strands.push_back(strand);
                            sId++;
                            strand = Model::Strand(0, sId);
                        }
                        strand.bases_.push_back(current);
                        strand.length_ += 1;
                        strand.forward_ = forward;
                        strand.scaffold_ = forward;
                        current->in_strand_ = true;
                        current->strandId_ = strand.id_;

                        // next
                        current = current->strand_forward_;
                        first = false;
                    }
                    else if (current->in_strand_ && found) {
                        // skip entire helix edge as it is already assigned
                        strands.push_back(strand);
                        sId++;
                        strand = Model::Strand(0, sId);
                        break;
                    }
                    else {
                        break;
                    }
                }
            }
        }


        Model::Strand* s;

        int mnvd;
        for (unsigned int i = 0; i < strands.size(); ++i) {
            s = &strands[i];

            // skip scaffold strand
            if (s->forward_) {
                continue;
            }

            // due to direction of the staple strands, the first base in the vector is the one with the last base id
            current = s->bases_[0];
            int helix_len = current->getParent()->bases_;
            mnvd = helix_len > 2.5 * MIN_NICK_VERTEX_DISTANCE ? MIN_NICK_VERTEX_DISTANCE : helix_len / 2;
            if (current->offset_ > (helix_len - mnvd - 1) ) {
                // expand the next strand in the forward direction
                if (current->strand_backward_ && current->strand_forward_) {
                    unsigned int nextstrandid = current->strand_backward_->strandId_;
                    Model::Strand* nextstrand = nullptr;
                    for (unsigned int ss = 0; ss < strands.size(); ++ss) {
                        if (strands[ss].id_ == nextstrandid) {
                            nextstrand = &strands[ss];
                        }
                    }
                    int offset = current->offset_;
                    while (offset > (helix_len - mnvd)) {
                        nextstrand->bases_.push_back(current);
                        current->strandId_ = nextstrandid;
                        nextstrand->length_++;
                        s->bases_.erase(s->bases_.begin());
                        s->length_--;
                        current = current->strand_forward_;
                        offset = current->offset_;
                    }
                }
            }
            current = s->bases_[s->bases_.size()-1];
            if (current->offset_ < (mnvd+1)) {
                if (current->strand_backward_ && current->strand_forward_) {
                    // expand the next strand in the backward direction
                    unsigned int prevstrandid = current->strand_forward_->strandId_;
                    Model::Strand* prevstrand;
                    for (unsigned int ss = 0; ss < strands.size(); ++ss) {
                        if (strands[ss].id_ == prevstrandid) {
                            prevstrand = &strands[ss];
                        }
                    }
                    int offset = current->offset_;
                    while (offset < mnvd) {
                        prevstrand->bases_.insert(prevstrand->bases_.begin(), current);
                        current->strandId_ = prevstrandid;
                        prevstrand->length_++;
                        s->bases_.pop_back();
                        s->length_--;
                        current = current->strand_backward_;
                        offset = current->offset_;
                    }
                }
            }
        }


        for (unsigned int i = 0; i < strands.size(); ++i) {
            if (strands[i].bases_.empty()) {
                strands.erase(strands.begin() + i);
            }
        }

        // remove strands with identical ids that connect strangely
        // can't find a solution to this rare bug but these can easily be connected/disconneced manually in oxview
        std::vector<unsigned long> sids;
        bool removed = false;
        for (unsigned long i = 0; i < strands.size(); ++i) {
            for (unsigned long j = 0; j < sids.size(); ++j) {
                if (strands[i].id_ == sids[j]) {
                    for (unsigned int b = 0; b < strands[i].bases_.size(); ++b) {
                        strands[i].bases_[b]->in_strand_ = false;
                    }
                    strands.erase(strands.begin()+i);
                    removed = true;
                }
            }
            if (removed) {
                removed = false;

                continue;
            }
            sids.push_back(strands[i].id_);
        }

        unsigned long largest = 0;
        for (unsigned long i = 0; i < strands.size(); ++i) {
            if (strands[i].id_ > largest) {
                largest = strands[i].id_;
            }
        }
        largest++;
        Model::Strand newstrand = Model::Strand(0, largest);
        bool found_unassigned = false;
        for (unsigned int h = 0; h < helices.size(); ++h) {
            for (unsigned int b = 0; b < helices[h].Bbases_.size(); ++b) {
                if (!helices[h].Bbases_[b].in_strand_) {
                    found_unassigned = true;
                    helices[h].Bbases_[b].in_strand_ = true;
                    helices[h].Bbases_[b].strandId_ = newstrand.id_;
                    newstrand.length_++;
                    newstrand.bases_.push_back(&helices[h].Bbases_[b]);
                }
            }
            if (found_unassigned) {
                found_unassigned = false;
                strands.push_back(newstrand);
                largest++;
                newstrand = Model::Strand(0, largest);
            }
        }

        // make sure no strands are too short, and even them out if that's the case
        for (unsigned int i = 0; i < strands.size(); ++i) {
            s = &strands[i];
            if (s->bases_.size() < MIN_NICK_VERTEX_DISTANCE + 1) {
                std::cout << "Strand id: " << s->id_ << " is too short with length " << s->bases_.size() << std::endl << std::flush;
                unsigned int forwstrandid = s->bases_[s->bases_.size()-1]->strand_backward_->strandId_;
                unsigned int prevstrandid = s->bases_[0]->strand_forward_->strandId_;
                std::cout << "forwstrandid: " << forwstrandid << " prevstrandid: " << prevstrandid << std::endl << std::flush;
                Model::Strand* prevstrand = nullptr;
                Model::Strand* forwstrand = nullptr;
                for (unsigned int ss = 0; ss < strands.size(); ++ss) {
                    if (strands[ss].id_ == prevstrandid) {
                        prevstrand = &strands[ss];
                    }
                    if (strands[ss].id_ == forwstrandid) {
                        forwstrand = &strands[ss];
                    }
                }
                if (!prevstrand || !forwstrand || prevstrand == forwstrand) {
                    std::cout << "Skipped strand id: " << s->id_  << std::endl << std::flush;
                    continue;
                }

                Model::Strand* receiver = prevstrand->bases_.size() > forwstrand->bases_.size() ? forwstrand : prevstrand;
                bool forw = prevstrand->bases_.size() > forwstrand->bases_.size() ? true : false;

                while (s->bases_.size() > 0) {
                    if (forw) {
                        receiver->bases_.insert(receiver->bases_.begin(), s->bases_[s->bases_.size()-1]);
                        s->bases_.pop_back();
                        receiver->bases_[0]->strandId_ = receiver->id_;
                        receiver->length_++;
                    }
                    else {
                        receiver->bases_.push_back(s->bases_[0]);
                        s->bases_.erase(s->bases_.begin());
                        receiver->bases_[receiver->bases_.size()-1]->strandId_ = receiver->id_;
                        receiver->length_++;
                    }
                }
                strands.erase(strands.begin()+i);
                i--;

                /*while (s->bases_.size() <= MIN_NICK_VERTEX_DISTANCE*2) {
                    s->bases_.push_back(forwstrand->bases_[forwstrand->bases_.size()-1]);
                    s->bases_[s->bases_.size()-1]->strandId_ = s->id_;
                    forwstrand->bases_.pop_back();
                    s->bases_.insert(s->bases_.begin(), prevstrand->bases_[0]);
                    s->bases_[0]->strandId_ = s->id_;
                    prevstrand->bases_.erase(prevstrand->bases_.begin());
                    s->length_ += 2;
                    forwstrand->length_--;
                    prevstrand->length_--;
                }*/
            }
        }

        // finally, connect all bases inside strands and disconnect connections between strands
        for (auto s : strands) {
            for (unsigned long b = 0; b < s.bases_.size(); ++b) {
                if (b == 0) {
                    s.bases_[b]->setBackward(nullptr);
                    s.bases_[b]->setForward(s.bases_[b+1]);
                }
                else if (b == s.bases_.size()-1) {
                    s.bases_[b]->setBackward(s.bases_[b-1]);
                    s.bases_[b]->setForward(nullptr);
                }
                else {
                    s.bases_[b]->setBackward(s.bases_[b-1]);
                    s.bases_[b]->setForward(s.bases_[b+1]);
                }
            }
        }
        return helices;
    }

    // open oxview model
    std::vector<Model::Strand> &Handler::readOxview(const char *path) {

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
            return strands;
        }

        json j = json::parse(file);

        unsigned long basecount = 0;
        unsigned long strandcount = 0;
        for (auto &system : j["systems"]) {
            for (auto &strand : system["strands"]) {
                strandcount++;
                for (auto &base : strand["monomers"]) {
                    basecount++;
                }
            }
        }

        bases.reserve(basecount+1);
        strands.reserve(strandcount+1);

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



    void Controller::Handler::autofillStrandGaps() {
        long baseid = 0;
        for (const auto &b : bases) {
            if (b.baseId_ > baseid) {
                baseid = b.baseId_;
            }
        }
        for (const auto &h : helices) {
            for (const auto &bb : h.Bbases_) {
                if (bb.baseId_ > baseid) {
                    baseid = bb.baseId_;
                }
            }
            for (const auto &bf : h.Fbases_) {
                if (bf.baseId_ > baseid) {
                    baseid = bf.baseId_;
                }
            }
        }
        baseid++;
        // iterate through each strand and check for gaps
        Model::Strand* s;
        std::vector<std::pair<int, int>> bases_ext_idx = std::vector<std::pair<int, int>>();
        int added_bases;
        for (unsigned int i = 0; i < strands.size(); ++i) {
            s = &strands[i];
            added_bases = 0;
            Model::Base *base1 = s->bases_[0];
            Model::Base *base2 = s->bases_[1];
            int idx = 1;
            while (idx < s->length_) {
                double dist = sqrt(pow(abs(base2->getPos().x() - base1->getPos().x()),2) +
                                   pow(abs(base2->getPos().y() - base1->getPos().y()),2) +
                                   pow(abs(base2->getPos().z() - base1->getPos().z()),2));
                if (dist > DNA::STEP*2) {
                    // how many bases should be inserted
                    int count = dist/(DNA::STEP*1.574) - 1;
                    // initialize the bases, connect their pointers directly later
                    int c = 0;
                    while (c < count) {
                        std::string str = "";
                        Model::Base b = Model::Base(s->id_, str, c == 0 ? base1->baseId_ : baseid-1, c == count-1 ? base2->baseId_ : baseid+1, baseid, -1);
                        b.setPos(QVector3D(base1->getPos().x() + (base2->getPos().x() - base1->getPos().x())/(count+1)*(c+1)*0.75,
                                           base1->getPos().y() + (base2->getPos().y() - base1->getPos().y())/(count+1)*(c+1)*0.75,
                                           base1->getPos().z() + (base2->getPos().z() - base1->getPos().z())/(count+1)*(c+1)*0.75));
                        bases_ext.push_back(b);
                        bases_ext_idx.push_back(std::pair<int, int>(idx+added_bases, i));
                        c++;
                        added_bases++;
                        baseid++;
                        // s->bases_.insert(s->bases_.begin()+idx+c, &b);
                    }
                }
                idx++;
                base1 = base2;
                base2 = s->bases_[idx];
            }
        }

        // add the new bases to their correct positions in strands
        for (unsigned int i = 0; i < bases_ext_idx.size(); ++i) {
            strands[std::get<1>(bases_ext_idx[i])].bases_.insert(strands[std::get<1>(bases_ext_idx[i])].bases_.begin() + std::get<0>(bases_ext_idx[i]), &bases_ext[i]);
        }

        // again, make sure the connections are correct
        for (auto s : strands) {
            for (unsigned long b = 0; b < s.bases_.size(); ++b) {
                if (b == 0) {
                    s.bases_[b]->setBackward(nullptr);
                    s.bases_[b]->setForward(s.bases_[b+1]);
                }
                else if (b == s.bases_.size()-1) {
                    s.bases_[b]->setBackward(s.bases_[b-1]);
                    s.bases_[b]->setForward(nullptr);
                }
                else {
                    s.bases_[b]->setBackward(s.bases_[b-1]);
                    s.bases_[b]->setForward(s.bases_[b+1]);
                }
            }
        }
    }
}
