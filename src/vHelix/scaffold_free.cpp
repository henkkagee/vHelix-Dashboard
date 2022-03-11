#include "scaffold_free.h"
#include "design.h"

using namespace dir;
using namespace boost;
using json = nlohmann::json;

// Python.h includes "slots" as well, resulting in a conflict. This fixes the issue.
#pragma push_macro("slots")
#undef slots
#define MS_NO_COREDLL
#ifdef _DEBUG
    #undef _DEBUG
    #include "Python.h"
    #define _DEBUG
#else
    #include "Python.h"
#endif
#pragma pop_macro("slots")

#define BUFFER_SIZE 1024
// 0.84 scaling is ad hoc solution to get good looking models
#define POSITION_SCALING 0.84f

bool dir::write_dimacs(std::string output_filename, DiGraph & G)
{
	std::string line;
	std::ofstream ofs(output_filename.c_str(), std::ifstream::out);
	if( !ofs.is_open())
	{
		std::cerr<<"Unable to open file "<<output_filename;
		return false;
	}
	ofs<<"p edge "<<num_vertices(G)<<" "<<num_edges(G)<<"\n";
	EdgeIterator e_it, e_end;
	tie(e_it, e_end) = edges(G);
	Vertex u, v;
	for( ; e_it != e_end; e_it++)
	{
		u = source(*e_it, G);
		v = target(*e_it, G);
		ofs<<"e "<< u + 1<<" "<<v+1<<"\n";
	}
	ofs.close();
	return true;
}

std::string dir::print_walk(std::vector<Vertex> Trail, std::string label)
{
    std::stringstream sstr;
	std::cout<<label;
	for( auto it = Trail.begin(); it != Trail.end(); it++)
	{
        sstr<<*it<<" ";
	}
    sstr<<std::endl;
    std::string ret(sstr.str());
    std::cout << ret;
    return ret;
}

//Create_strands::Create_strands(std::string &rpoly) : rpoly_(rpoly) {}
Create_strands::Create_strands() {}

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

Create_strands::Connection::Type Create_strands::Connection::TypeFromString(const char *type) {
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
std::vector<Model::Helix> &Create_strands::readRpoly(const char *filename, int nicking_min_length, int nicking_max_length) {
    unsigned int MIN_NICK_VERTEX_DISTANCE = 7;

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
    unsigned int num_bases;
    char nameBuffer[BUFFER_SIZE], helixNameBuffer[BUFFER_SIZE], materialNameBuffer[BUFFER_SIZE], targetNameBuffer[BUFFER_SIZE], targetHelixNameBuffer[BUFFER_SIZE];
    char label;
    bool autostaple(true);
    std::vector< std::pair<std::string, std::string> > paintStrands;
    std::vector<Model::Base*> paintStrandBases, disconnectBases;

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
        else if (sscanf(line.c_str(), "hb %s %u %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &num_bases, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 9) {
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
            for (int i = 0; i < helices.back().bases_; i++) {
                helices.back().Fbases_[i].setParent(&helices.back());
                helices.back().Bbases_[i].setParent(&helices.back());
            }
        }
        else if (sscanf(line.c_str(), "hb %s %u %lf %lf %lf %lf %lf %lf %lf", nameBuffer, &num_bases, &pos[0], &pos[1], &pos[2], &ori[0], &ori[1], &ori[2], &ori[3]) == 9) {
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
            //std::cout << "Orientation: x = " << orientation.x() << ", y = " << orientation.y() << ", z = " << orientation.z() << ", scalar = " << orientation.scalar() << "\n";
            
            Model::Helix(position, orientation, nameBuffer, bIdStart, num_bases);
            helices.push_back(Model::Helix(position, orientation, nameBuffer, bIdStart, num_bases));
            for (int i = 0; i < helices.back().bases_; i++) {
                helices.back().Fbases_[i].setParent(&helices.back());
                helices.back().Bbases_[i].setParent(&helices.back());
            }
            bIdStart += helices.back().bases_*2;
            std::cout << "Bases: " << helices.back().bases_ << " -- next bIdStart: " << bIdStart << std::endl << std::flush;
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
    std::cout << "information imported from rpoly file\n";

    // Use Meliks code here (cadwire)
    for (std::vector<Model::Helix>::iterator it(helices.begin()); it != helices.end(); ++it) {
        std::set<int> disconnect_forw;
        std::set<int> disconnect_back;
        if (abs(it->bases_ - 21) <= 2) { // ~ 2 turns
            disconnect_forw.insert(it->bases_ - 7);
            disconnect_back.insert(7);
        }
        else if (abs(it->bases_ - 32) <= 2) { // ~ 3 turns
            disconnect_forw.insert(it->bases_ - 11);
            disconnect_back.insert(11);
        }
        else if (abs(it->bases_ - 42) <= 2) { // ~ 4 turns
            disconnect_forw.insert(it->bases_ - 14);
            disconnect_back.insert(14);
        }
        else if (it->bases_ > 44 && it->bases_ < 82)  { // ~ 5, 6 or 7 turns
            disconnect_forw.insert(21);
            disconnect_forw.insert(it->bases_ - 11);
            disconnect_back.insert(11);
            disconnect_back.insert(it->bases_ - 21);
        }
        else if (abs(it->bases_ - 84) <= 2)  {
            disconnect_forw.insert(21);
            disconnect_forw.insert(47);
            disconnect_forw.insert(it->bases_ - 11);
            disconnect_back.insert(11);
            disconnect_back.insert(37);
            disconnect_back.insert(it->bases_ - 21);
        }
        else if (abs(it->bases_ - 95) <= 2)  {
            disconnect_forw.insert(21);
            disconnect_forw.insert(52);
            disconnect_forw.insert(it->bases_ - 11);
            disconnect_back.insert(11);
            disconnect_back.insert(42);
            disconnect_back.insert(it->bases_ - 21);
        }
        else if (it->bases_ > 97) {
            disconnect_forw.insert(21);
            disconnect_forw.insert(21 + (int) ((it->bases_ - 32) / 2));
            disconnect_forw.insert(it->bases_ - 11);
            disconnect_back.insert(11);
            disconnect_back.insert(11 + (int) ((it->bases_ - 32) / 2));
            disconnect_back.insert(it->bases_ - 21);
        }
        Model::Base *bbase = it->getBackwardThreePrime();
        Model::Base *fbase = it->getForwardFivePrime();
        for (int i = 0; i < it->bases_ - 1; ++i) {
            bbase = bbase->getBackward();
            fbase = fbase->getForward();
            if (bbase == nullptr || fbase == nullptr) {
                std::cout << "baseError\n" << std::flush;
                break;
            }
            if (disconnect_back.find(i) != disconnect_back.end()) {
                disconnectBases.push_back(bbase);
            }
            if (disconnect_forw.find(i) != disconnect_forw.end()) {
                disconnectBases.push_back(fbase);
            }

        }
    }

    std::cerr << "Nicking locations determined\n";

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
            std::cout << "Connected " << fromHelix->name_ << " " << it->fromName << " to " << toHelix->name_ << " " << it->toName << "\n";
        }
    }


    std::cerr << "Connections determined\n";


    for (auto k : disconnectBases) {
        Model::Base *b = k->getBackward();
        k->checked_ = true;
        k->setBackward(nullptr);
        b->setForward(nullptr);
    }

    std::cerr << "Bases disconnected\n";
    std::cout << "Size of disconnectbases: " << disconnectBases.size() << "\n";
    unsigned long int sId = 0;

    for (auto &b : disconnectBases) {
        Model::Base *current = b;
        Model::Strand strand = Model::Strand(0, sId);
        while (current != nullptr) {
            strand.bases_.push_back(current);
            strand.length_ += 1;
            //strand.forward_ = forward;
            //strand.scaffold_ = forward;
            current->in_strand_ = true;
            current->strandId_ = strand.id_;
            current = current->forward_;
        }
        strands.push_back(strand);
        sId++;
    }

    std::cerr << "Strands created. Number of strands: " << strands.size() << "\n";

    for (auto &s : strands) {
        std::cout << "Length of strand " << s.id_ << ": " << s.length_ << "\n";
    }

    /*for (auto &s : strands) {
        Model::Helix *parent = s.bases_[0]->getParent();
        Model::Base *fiveprime = s.bases_[0];
        Model::Base *threeprime = s.bases_[s.length_-1];
        if (parent != nullptr) {
            std::cout << "Base " << base.baseId_ << " in strand" << parent->name_ << "\n";
        }
        else {
            std::cout << "Base " << base.baseId_ << " is in no strand \n";
        }
    }*/

    /*for (unsigned int i = 0; i < helices.size(); i++) {
        for (int forw = 0; forw < 2; forw++) {
            if (forw == 0) { // backward strand
                Model::Base *current = nullptr;
                for (auto &b : helices[i].Bbases_) { // Start from nicking point
                    if (std::find(disconnectBases.begin(),disconnectBases.end(),b) == disconnectBases.end()) {
                        std::cout << b.baseId_;
                        current = b.getForward();
                        break;
                    }
                }
                Model::Strand strand = Model::Strand(0, sId);
                while (current != nullptr) {
                    strand.bases_.push_back(current);
                    strand.length_ += 1;
                    //strand.forward_ = forward;
                    //strand.scaffold_ = forward;
                    current->in_strand_ = true;
                    current->strandId_ = strand.id_;
                    current = current->getForward();
                }

            }
            else { // forward strand
                Model::Base *current = nullptr;
                for (auto &b : helices[i].Fbases_) { // Start from nicking point
                    if (std::find(disconnectBases.begin(),disconnectBases.end(),b) == disconnectBases.end()) {
                        std::cout << b.baseId_;
                        current = b.getForward();
                        break;
                    }
                }
                Model::Strand strand = Model::Strand(0, sId);
                while (current != nullptr) { // continue to next nicking point
                    strand.bases_.push_back(current);
                    strand.length_ += 1;
                    //strand.forward_ = forward;
                    //strand.scaffold_ = forward;
                    current->in_strand_ = true;
                    current->strandId_ = strand.id_;
                    current = current->getForward();
                }
                strands.push_back(strand);
                sId++;

            }
        }
    }
    // create the strands


    /*Model::Helix *h = &helices.front();
    Model::Base *current;
    unsigned long sId = 0;
    Model::Strand strand = Model::Strand(0, sId);
    bool forward;

    // forward and backward strands throughout structure. Assign bases to strands.
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
            if (!current || (strand.length_ > nicking_min_length + (nicking_max_length - nicking_min_length)/2) || current->in_strand_) {
                std::cout << "Helix: " << current->getParent()->name_ << ", Base: " << current->baseId_ << "\n";
                // make sure that the last strand isn't too short if we looped back to already assigned bases
                if (strand.length_ < nicking_min_length) {
                    std::cout << "Strand too short\n";
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
            //strand.scaffold_ = forward;
            current->in_strand_ = true;
            current->strandId_ = strand.id_;

            // next
            current = current->strand_forward_;
        }
    }
    //strands.pop_back();
    std::cerr << "Strands created\n";

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
                    if (!current || (strand.length_ > nicking_min_length + (nicking_max_length - nicking_min_length)/2) || current->in_strand_) {
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
/*
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
    }*/


    for (unsigned int i = 0; i < strands.size(); ++i) {
        if (strands[i].bases_.empty()) {
            strands.erase(strands.begin() + i);
        }
    }
    std::cerr << "Bases connected\n";
    // remove strands with identical ids that connect strangely
    // can't find a solution to this rare bug but these can easily be connected/disconneced manually in oxview
    /*std::vector<unsigned long> sids;
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
    }*/

    // make sure no strands are too short, and even them out if that's the case
    /*for (unsigned int i = 0; i < strands.size(); ++i) {
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

            begin comment while (s->bases_.size() <= MIN_NICK_VERTEX_DISTANCE*2) {
                s->bases_.push_back(forwstrand->bases_[forwstrand->bases_.size()-1]);
                s->bases_[s->bases_.size()-1]->strandId_ = s->id_;
                forwstrand->bases_.pop_back();
                s->bases_.insert(s->bases_.begin(), prevstrand->bases_[0]);
                s->bases_[0]->strandId_ = s->id_;
                prevstrand->bases_.erase(prevstrand->bases_.begin());
                s->length_ += 2;
                forwstrand->length_--;
                prevstrand->length_--;
            } end comment
        }
    }
    */
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

void Create_strands::writeOxView(const char *fileName)
{

    if (!strands.empty() && !helices.empty()) {
        // could use nlohmann::json here
            std::ofstream out(fileName, std::ios::out);
            if( !out.is_open())
            {
                std::cout << "ERROR! Unable to create file "<<fileName<<std::endl;
                return;
            }
            auto time = std::chrono::system_clock::now();
            std::time_t now = std::chrono::system_clock::to_time_t(time);
            std::stringstream sstr;
            sstr << "{\"date\":\"" << std::ctime(&now);     // ctime is deprecated, but ctime_s is not standard c++
            std::string timestr = sstr.str();
            timestr.erase(timestr.size()-1);
            sstr.str(std::string());
            sstr << timestr << "\",";
            out << sstr.str().c_str();
            // find box size from max base coordinates
            float max = 0;
            long long int basecount = 0;
            for (const auto &s : strands) {
                for (const auto &b : s.bases_) {
                    basecount++;
                    for (unsigned int dim = 0; dim < 3; dim++) {
                        if (b->getPos()[dim] > max) {
                            max = b->getPos()[dim];
                        }
                    }
                }
            }
            max *= 3;       // 3x multiplier to box dimensions to make it large enough for the structure
            out << "\"box\":[" << max << "," << max << "," << max << "],";

            // write systems (strands and bases). Currently, only one system supported as there is no such notion in vhelix
            out << "\"systems\":[{\"id\":0,";
            out << "\"strands\":[";

            bool firstStrand = true;
            for (const auto &s : strands) {
                bool firstBase = true;
                if (!firstStrand) {
                    out << ",";
                }
                firstStrand = false;
                out << "{\"id\":" << s.id_ << ",";
                out << "\"monomers\":[";
                unsigned long basenum = 0;
                for (const auto &b : s.bases_) {

                    if (!firstBase) {
                        out << ",";
                    }

                    out << "{\"id\":" << b->baseId_ << ",";
                    out << "\"type\":";
                    if (b->getBase() == Nucleobase::ADENINE) {
                        out << "\"A\",";
                    }
                    else  if (b->getBase() == Nucleobase::CYTOSINE) {
                        out << "\"C\",";
                    }
                    else if (b->getBase() == Nucleobase::THYMINE) {
                        out << "\"T\",";
                    }
                    else if (b->getBase() == Nucleobase::GUANINE) {
                        out << "\"C\",";
                    }
                    else if (b->getBase() == Nucleobase::NONE) {
                        out << "\"?\",";
                    }

                    // TODO: RNA support?
                    out << "\"class\":\"DNA\",";

                    QVector3D a1;
                    QVector3D a3;
                    if (b->getOpposite() == nullptr) {
                        a1 = QVector3D(sin(b->getPos().x()),sin(b->getPos().y()), sin(b->getPos().z())).normalized();
                        a3 = QVector3D::crossProduct(a1, QVector3D(0, 1, 0));
                    }

                    if (b->getOpposite() != nullptr) {
                        a1.setX(b->getPos().x() - b->getOpposite()->getPos().x());
                        a1.setY(b->getPos().y() - b->getOpposite()->getPos().y());
                        a1.setZ(b->getPos().z() - b->getOpposite()->getPos().z());
                        a1 = a1.normalized();
                        a3 = b->a3_.normalized();
                    }

                    out << "\"p\":[" << (b->getPos().x() * 1.104) - a1.x() * 0.35 << ","
                        << (b->getPos().y() * 1.104) - a1.y() * 0.35<< ","
                        << (b->getPos().z() * 1.104) - a1.z() * 0.35<< "],";

                    if (b->getForward() != nullptr) {
                        if (basenum != s.bases_.size()-1) {
                            out << "\"n5\":" << b->getForward()->baseId_ << ",";
                        }
                    }
                    if (b->getBackward() != nullptr) {
                        if (!firstBase) {
                            out << "\"n3\":" << b->getBackward()->baseId_ << ",";
                        }
                    }
                    out << "\"cluster\":1,";
                    out << "\"a1\":[" << a1.x()*-1.0 << "," << a1.y()*-1.0 << "," << a1.z()*-1.0 << "],";
                    out << "\"a3\":[" << a3.x()*-1.0 << "," << a3.y()*-1.0 << "," << a3.z()*-1.0 << "]";

                    if (b->getOpposite() != nullptr) {
                        out << ",\"bp\":" << b->getOpposite()->baseId_;
                    }
                    out << "}";
                    firstBase = false;
                    basenum++;
                }
                out << "],";        // close monomers-section for each strand
                out << "\"end3\":" << s.bases_.at(0)->baseId_ << ",\"end5\":" << s.bases_.at(s.bases_.size()-1)->baseId_ << ",\"class\":\"NucleicAcidStrand\"}";
            }

            out << "]";       // close strands-section
            out << "}]";      // close systems-section
            out << "}";       // close entire .json
            out.close();
            std::cout << "\nWrote current helices, strands, connections and bases to " << fileName << std::endl;
        }
    else {
        std::cout << "There is nothing to write!\n";
    }
}

Scaffold_free::Scaffold_free(const std::string meshpath, const std::string meshname) : Design(meshpath, meshname)
{}

int Scaffold_free::createGraph() {
    //using namespace undir;
    outstream << "creating Graph...\n";
    std::vector<bool> check_double(number_vertices*number_vertices,false);
    graph = DiGraph(number_vertices);
    int nodes_per_face;
    int start_node;
    int end_node;
    int aux;
    int max;
    int min;
    size_t edge_ind = 0;
    for (unsigned int i = 0; i < number_faces; i++) {
        nodes_per_face = faces[i].size();
        start_node = faces[i][0] + 1;
        aux = start_node;
        end_node = faces[i][1] + 1;
        max = std::max(start_node,end_node);
        min = std::min(start_node,end_node);
        if (check_double[min + (max-1)*(max-2)/2] == false) {
            add_edge(start_node-1,end_node-1,edge_ind,graph);
            edge_ind++;
            check_double[min + (max-1)*(max-2)/2] = true;
        }
        for (int j = 2; j < nodes_per_face; j++) {
            start_node = end_node;
            end_node = faces[i][j] + 1;
            max = std::max(start_node, end_node);
            min = std::min(start_node, end_node);
            if (check_double[min + (max-1)*(max-2)/2] == false)
            {
                add_edge(start_node-1,end_node-1,edge_ind,graph);
                edge_ind++;
                check_double[min + (max-1)*(max-2)/2] = true;
            }
        }
        start_node = end_node;
        end_node = aux;
        max = std::max(start_node, end_node);
        min = std::min(start_node, end_node);
        if (check_double[min + (max-1)*(max-2)/2] == false)
        {
            add_edge(start_node-1,end_node-1,edge_ind,graph);
            edge_ind++;
            check_double[min + (max-1)*(max-2)/2] = true;
        }
    }
    for (unsigned int i = 0; i < number_edges; i++) {
        start_node = non_face_edges[i][0];
        end_node = non_face_edges[i][1];
        add_edge(start_node,end_node,edge_ind,graph);
        edge_ind++;
    }
    outstream << "Successfully created the Graph\n";

    if (write_intermediates) {
        std::string dimacs(name);
        write_dimacs(dimacs.append(".dimacs"),graph);
    }
    return 1;
}

int Scaffold_free::find_trail()
{

    std::vector<std::vector<Vertex>> isolated_trails;
    unsigned int nof_trails = 0;
    std::vector<Vertex> Trail(0);
    std::vector<Vertex>::iterator insert_loc;
    VertexIterator vit = boost::vertices(graph).first;
    VertexIterator vend = --(boost::vertices(graph).second);
    while(out_degree(*vit, graph) == 0)
    {
        if( vit == vend)
        {
            outstream<<"WARNING! The graph consists of isolated vertices, the trail is empty"<<std::endl;
            return 1;
        }
        vit++;
    }
    Vertex v = *vit;
    //Trail.push_back(v);
    insert_loc = Trail.end();
    Vertex prev;
    Vertex next;
    AdjacencyIterator adIt, adEnd;
    //AdjacencyIterator neiSt, neiEnd;

    // unmarked edges of a vertex, every edge should be traversed once in both directions
    // an element in unused_edges represents an edge directed *AWAY FROM* a vertex
    std::vector< std::list<std::pair<Vertex,bool> > >unused_edges(num_vertices(graph));
    unsigned int i=0;
    for(auto myIt = unused_edges.begin(); myIt != unused_edges.end(); myIt++, i++)
    {
        //tie(neiSt, neiEnd) = adjacent_vertices(i, G);
        for(auto neiIt = embedding[i].begin(); neiIt != embedding[i].end(); neiIt++)
        {
            myIt->push_back(std::make_pair((unsigned int)*neiIt,true));
        }
    }
    std::set<Vertex> in_trail_has_unused; //vertices on the current tour that have unused edges
    in_trail_has_unused.insert(v);

    bool first = true;
    while (!in_trail_has_unused.empty())
    {
        nof_trails++;
        v = *in_trail_has_unused.begin();
        //insert_loc = std::find( Trail.begin(), Trail.end(), v);
        insert_loc = Trail.end();
        //insert_loc = Trail.insert(insert_loc,99999999);
        insert_loc = Trail.insert(insert_loc,v);
        //std::cout << "Insert location of new trail: " << std::distance(insert_loc, Trail.begin()) << std::endl;
        insert_loc++;
        prev = v;
        next = v;
        first = true;
        while(1)
        {
            Vertex adIt;
            if (first == true) {
                adIt = std::find_if(unused_edges[next].begin(),unused_edges[next].end(),[](std::pair<Vertex,bool>& edge) {
                            return edge.second == true;
                        })->first;
                first = false;
                //std::cout << next << " to " << adIt << " (start)" << std::endl;
            }
            else {
                auto tmp = std::find_if(unused_edges[next].begin(),unused_edges[next].end(), [prev](std::pair<Vertex,bool>& edge) {
                    return (prev == edge.first);
                });
                std::pair<Vertex,bool> maybe_unused_edge;
                if (std::distance(tmp, unused_edges[next].end()) == 1) {
                    maybe_unused_edge = unused_edges[next].front();

                    //adIt = unused_edges[next].front().first; //last vertex in vertex code -> first vertex in vertex code

                    //std::cout << "(found, end) ";
                }
                else {
                    tmp++;
                    maybe_unused_edge = *tmp;
                    //adIt = tmp->first;
                    //std::cout << "(found) ";
                }
                if (maybe_unused_edge.second == false) {
                    //std::cout << "Next edge used, starting new trail\n";
                    break;
                }
                else {
                    adIt = maybe_unused_edge.first;
                    //std::cout << next << " to " << adIt <<  std::endl;
                }
            }


            prev = next;
            next = adIt;
            std::find(unused_edges[prev].begin(), unused_edges[prev].end(), std::make_pair(next,true))->second = false;
            in_trail_has_unused.insert(next);
            insert_loc = Trail.insert(insert_loc,next);
            insert_loc++;
        }//!(unused_edges[next].second == false));
        //std::cout << "DEBUG: "; print_walk(Trail, "Trail ");


        for(std::set<Vertex>::iterator setIt = in_trail_has_unused.begin(); setIt != in_trail_has_unused.end();)
        {
            if (std::find_if(unused_edges[*setIt].begin(),unused_edges[*setIt].end(),[](std::pair<Vertex,bool>& edge) {
                          return (edge.second == true);
                      }) == unused_edges[*setIt].end()) // condition fulfilled when vertex has no (outgoing) unused edges
            {
                in_trail_has_unused.erase(setIt++);
            }
            else
            {
                ++setIt;
            }
        }

        isolated_trails.push_back(Trail);
        Trail.clear();
    }
    std::cout << "Trail as node list\n";
    for (unsigned int i = 0; i < nof_trails; i++) {
        print_walk(isolated_trails[i], "");
    }
    nodetrail = isolated_trails;
    if (write_intermediates) {
        write_trails();
    }
    //print_walk(Trail, "Trail as edge list: \n");
    return 0;
}

bool Scaffold_free::write_trails() {
        std::string nodetrail_file(name);
        nodetrail_file.append(".ntrail");
        std::ofstream nfs(nodetrail_file.c_str(), std::ios::out);
        if( !nfs.is_open()){
            outstream << "ERROR! Unable to create file "<<nodetrail_file<<std::endl;
            return false;
        }
        else {
            outstream << "INFO: Appending the trail as node list to file "<<nodetrail_file<<std::endl;
            
            for( std::vector<std::vector<Vertex>>::iterator it = nodetrail.begin(); it != nodetrail.end(); it++){
                for (std::vector<Vertex>::iterator it1 = it->begin(); it1 != it->end();it1++) {
                    nfs<<*it1<<" ";
                }
                nfs << "\n";
            }
        }
        nfs.close();
        return true;
}

int Scaffold_free::main() {
    if (read3Dobject() < 1) return 0;
    std::cerr << "read ply\n";
    std::cerr << vertices.size() << nodetrail.size() << number_vertices<< std::endl;
    if (createEmbedding() < 1) return 0;
    std::cerr << "created embedding\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;
    if (createGraph() < 1) return 0;
    std::cerr << "Created graph\n";
    std::cerr << vertices.size() << nodetrail.size()<< std::endl;;
    if (!find_trail()) return 0;
    outstream << "Found trails\n";
    std::cerr << "Found trails\n";
    //std::cerr << vertices.size() << " " << nodetrail.size()<< std::endl;
    return 1;
}

int Scaffold_free::relax(const QVector<QVariant> args,bool &hasresult) {
    for (unsigned int i = 0; i < number_vertices; i++) {
        std::cerr << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z<< std::endl;
    }

    using namespace PhysXRelax;
    double *dblArgs = new double[9];
    bool *boolArgs = new bool[3];
    /*unsigned int k = 0; unsigned int j = 0;
    for (unsigned int i = 0; i < 15; i++) {
        switch (i) {
            case 0: // [0]scaling
            case 2: // [1]density
            case 3: // [2]spring stiffness
            case 4: // [3]fixed spring stiffness
            case 5: // [4]spring damping
            case 7: // [5]static friction
            case 8: // [6]dynamic friction
            case 9: // [7]restitution
            case 10:// [8]rigid body sleep threshold
                dblArgs[k] = args[i].toDouble();
                k++;
                break;
            case 1:  // discretize lengths
            case 6:  // attach fixed
            case 11: // visual debugger
                if (args[i].toString() == "Disabled") {
                    boolArgs[j] = false;
                }
                else {
                    boolArgs[j] = true;
                }
                j++;
                break;
        }
    }
    const int iterations = args[12].toInt();*/

    std::ifstream json("settings.json");
    nlohmann::json settings;
    if (json.is_open()) {
        std::cout << "Found settings file\n";
        json >> settings;
    }
    else {
        std::cerr << "Could not read physics settings from file\n";
        return -1;
    }
    std::cout << settings;
    settings = settings["physX"];
    dblArgs[0] = args[0].toDouble(); // scaling
    dblArgs[1] = settings["density"];
    dblArgs[2] = settings["spring_stiffness"];
    dblArgs[3] = settings["fixed_spring_stiffness"];
    dblArgs[4] = settings["spring_damping"];
    dblArgs[5] = settings["static_friction"];
    dblArgs[6] = settings["dynamic_friction"];
    dblArgs[7] = settings["restitution"];
    dblArgs[8] = settings["rigid_body_sleep_threshold"];
    boolArgs[0] = settings["discretize_lengths"];
    boolArgs[1] = settings["attach_fixed"];
    boolArgs[2] = settings["visual_debugger"];
    const int iterations = settings["iterations"];
    double timeout = settings["timeout"];
    
    PhysXRelax::physics::settings_type physics_settings;
    PhysXRelax::scene::settings_type scene_settings;
    PhysXRelax::Helix::settings_type helix_settings;
    PhysXRelax::parse_settings(dblArgs,boolArgs,physics_settings,scene_settings,helix_settings);
    //PhysXRelaxation = new PhysXRelaxation(name,physics_settings,scene_settings,helix_settings, iterations);
    PhysXRelaxation relaxation(name,physics_settings,scene_settings,helix_settings, iterations, timeout);
    std::cerr << "Created relaxation object\n";
    std::vector<std::vector<unsigned int>> input_paths;
    for (auto it = nodetrail.begin(); it != nodetrail.end(); it++) {
        std::vector<unsigned int> input_path;
        std::copy((*it).begin(),(*it).end(),std::back_inserter(input_path));
        input_paths.push_back(input_path);
    }
    
    //std::copy(nodetrail.begin(),nodetrail.end(),std::back_inserter(input_path));

    //outstream << relaxation->getoutstream().c_str();
    std::cerr << "Running scaffold_free_main(). Argument sizes: " << vertices.size() << ", " << input_paths.size()<< std::endl;
    //relaxation.scaffold_free_main(vertices,input_paths);
    relaxation.scaffold_free_main2(vertices,input_paths,rpoly_code);
    if (write_intermediates) {
        std::string output_file(name.c_str());
        output_file.append(".rpoly");
        std::ofstream outfile(output_file);
        rpoly_code.write(outfile);
        outfile.close();
    }
    std::cout << "Almost finsihed relaxation\n";
    outstream << relaxation.getoutstream().c_str();
    std::cout << "Finished relaxation\n";
    //create_strands();
    return 1;
}

void Scaffold_free::generate_sequences(std::string &s) {
    std::cout << "Creating sequences\n";
    int opt = std::stoi(s);
    std::cout << "Seq gen option: " << opt << "\n";
    if (opt == 1) {
        create_random_sequences();
    }
    else if (opt == 2) {
        outstream << "Using simulated annealing\n";
        
        create_sequences();
    }

}

int Scaffold_free::create_strands() {
    outstream << "Creating strands\n";
    std::string rpoly(name);
    rpoly.append(".rpoly");
    model_.readScaffoldFreeRpoly(rpoly.c_str());
    return 0;
}

int Scaffold_free::create_random_sequences() {

    // pseudo-random base generator
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist4(0,3);
    std::vector<Nucleobase> basemap{ADENINE, CYTOSINE, THYMINE, GUANINE}; // Maps number created by random generator to bases
    std::map<Nucleobase, Nucleobase> oppmap; // Key: Base; Value: Its complementary base
    oppmap.insert(std::pair<Nucleobase,Nucleobase>(ADENINE,THYMINE));
    oppmap.insert(std::pair<Nucleobase,Nucleobase>(THYMINE,ADENINE));
    oppmap.insert(std::pair<Nucleobase,Nucleobase>(CYTOSINE,GUANINE));
    oppmap.insert(std::pair<Nucleobase,Nucleobase>(GUANINE,CYTOSINE));
    for (auto &strand : model_.strands) {
        for (auto &base : strand.bases_) {
            if (base->getBase() == Nucleobase::NONE) { // If base not set, set it and its complemetary
                Nucleobase nb = basemap[dist4(rng)];
                base->setBase(nb);
                base->getOpposite()->setBase(oppmap[nb]);
                //std::cout << "set base " << base->baseId_ << " to " << nb <<  " and " << base->getOpposite()->baseId_ << " to " << oppmap[nb] << "\n";
            }
        }
    }
    return 0;
}

//Useful definition defined in mostly in model.h
/*
Strand {
	vector<Base*> bases_
	int length_
	unsigned long id_

	# scaffolded models
	bool scaffold_
	bool forward_
}

enum Nucleobase {
    ADENINE = 0,
    CYTOSINE = 1,
    GUANINE = 2,
    THYMINE = 3,
    NONE = 4
};

enum Type {
    BASE = 0,
    FIVE_PRIME_END = 1,
    THREE_PRIME_END = 2,
    END = 3
};

Base {
	Base* getForward()
	Base *getBackward()
	Base *getOpposite()
	void setBase(Nucleobase)
	Type &getType();
	unsigned int strandId_
}
*/
int Scaffold_free::create_sequences() {
    std::cout << "In create_sequences\n";
    // vector of Strand structs to work with: model_.strands
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist4(0,3);
    std::vector<Nucleobase> basemap{ADENINE, CYTOSINE, THYMINE, GUANINE}; // Maps number created by random generator to bases
    std::map<const char , Nucleobase> base_char_map; // Key: Char; Value: Its corresponding base
    base_char_map.insert(std::pair<const char ,Nucleobase>('T',THYMINE));
    base_char_map.insert(std::pair<const char ,Nucleobase>('A',ADENINE));
    base_char_map.insert(std::pair<const char ,Nucleobase>('G',GUANINE));
    base_char_map.insert(std::pair<const char ,Nucleobase>('C',CYTOSINE));

    //std::map<int,int> pairing_dict;
    PyObject *pairing_dict = PyDict_New();

    //std::cout << "Length of bases container: " << model_.bases.size() << "\n";
    for (auto &strand : model_.strands) {
        for (auto &base : strand.bases_) {
            PyObject *key = PyLong_FromLong(base->baseId_);
            PyObject *val = NULL;
            if (base->getOpposite() == nullptr) {
                val = PyLong_FromLong(-1);
            }
            else {
                val = PyLong_FromLong(base->getOpposite()->baseId_);
            }
            PyDict_SetItem(pairing_dict,key,val);
        }
    }

    PyObject *pName = PyUnicode_FromString("multi_strand_generator");
    PyObject *pModule = PyImport_Import(pName);
    if (pModule == NULL) {
        std::cout << "--- Something went wrong when importing multi_strand_generator.py ---\n";
        PyErr_PrintEx(1);
        Py_DECREF(pName);
        return 0;
    }
    PyObject *pFunc = PyObject_GetAttrString(pModule,"generate_primary");
    if (pFunc == NULL || !PyCallable_Check(pFunc)) {
        std::cout << "Something went wrong while importing the function\n";
        PyErr_PrintEx(1);
        Py_DECREF(pName);
        return 0;
    }
    PyObject *pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs,0,pairing_dict);
    PyObject *pRet = PyObject_CallObject(pFunc,pArgs);
    if (pRet == NULL) {
        std::cout << "Something went wrong while running the function\n";
        PyErr_PrintEx(1);
        Py_DECREF(pName);
        return 0;
    }
    const char *ps = PyUnicode_AsUTF8(pRet);
    std::cout << ps << "\n";

    for (auto &strand : model_.strands) {
        for (auto &base : strand.bases_) {
            std::cout << base->baseId_ << " ";
            base->setBase(base_char_map[ps[base->baseId_]]);
        }
    }
    return 1;

}

