/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
 *
 * Base, Helix and Strand data classes.
*/

#include "model.h"

// utility functions

void rotate_vector_by_quaternion(const QVector3D& v, const QQuaternion& q, QVector3D& vprime)
{
    QVector3D u(q.x(), q.y(), q.z());
    float s = q.scalar();
    vprime = 2.0f * QVector3D::dotProduct(u, v) * u
          + (s*s - QVector3D::dotProduct(u, u)) * v
          + 2.0f * s * QVector3D::crossProduct(u, v);
}


// Base members

void Model::Base::setForward(Base *base)
{
    forward_ = base;
}
void Model::Base::setBackward(Base *base)
{
    backward_ = base;
}
void Model::Base::setOpposite(Base *base)
{
    opposite_ = base;
}
void Model::Base::setBase(Nucleobase nb)
{
    nucleobase_ = nb;
}
void Model::Base::setParent(Helix* helix)
{
    parent_ = helix;
}
void Model::Base::setType(Type set_type)
{
    type_ = set_type;
}
void Model::Base::setPos(QVector3D pos)
{
    position_ = pos;
}

Model::Base* Model::Base::getOpposite()
{
    return opposite_;
}
Model::Base* Model::Base::getForward()
{
    return forward_;
}
Model::Base* Model::Base::getBackward()
{
    return backward_;
}
Model::Helix* Model::Base::getParent()
{
    return parent_;
}
Type &Model::Base::getType()
{
    return type_;
}
Nucleobase &Model::Base::getBase()
{
    return nucleobase_;
}
QVector3D &Model::Base::getPos()
{
    return position_;
}

// standard constructor for structure generation
Model::Base::Base(const QVector3D &arg_position, Type arg_type, int offset, unsigned long BaseId) :
    position_(arg_position), type_(arg_type), in_strand_(false), offset_(offset), baseId_(BaseId)
{
    backward_ = nullptr;
    forward_ = nullptr;
    opposite_ = nullptr;
    strand_forward_ = nullptr;
    strand_backward_ = nullptr;
    checked_ = false;
}

// constructor for loading existing structures from .oxview-format
Model::Base::Base(unsigned int StrandId, std::string& nb, long Base3neigh, long Base5neigh, unsigned long BaseId, long Oppneigh)
    : strandId_(StrandId), base3neigh_(Base3neigh), base5neigh_(Base5neigh), baseId_(BaseId), oppneigh_(Oppneigh)
{
    if (nb == "A" || nb == "a") {
        nucleobase_ = Nucleobase::ADENINE;
    }
    else if (nb == "T" || nb == "t") {
        nucleobase_ = Nucleobase::THYMINE;
    }
    else if (nb == "G" || nb == "g") {
        nucleobase_ = Nucleobase::GUANINE;
    }
    else if (nb == "C" || nb == "c") {
        nucleobase_ = Nucleobase::CYTOSINE;
    }
    else {
        nucleobase_ = Nucleobase::NONE;
    }
    backward_ = nullptr;
    forward_ = nullptr;
    opposite_ = nullptr;
    strand_forward_ = nullptr;
    strand_backward_ = nullptr;
    checked_ = false;
}



// Helix members

Model::Helix::Helix(const QVector3D &position, const QQuaternion &orientation, const char *name, unsigned long bIdStart, unsigned int bases)
 : position_(position), orientation_(orientation), name_(name), bases_(bases), sequence_assigned_(false)
{
    // create the bases and position them
    QVector3D basePositions[2];
    Model::Base *base_objects[2], *last_base_objects[2];
    QVector3D pos1, pos2;
    Fbases_.reserve(bases_);
    Bbases_.reserve(bases_);
    for (int i = 0; i < bases_; i++) {
        /*
         * Get the positions for the bases from DNA.h
         */

        if (!(DNA::CalculateBasePairPositions(double(i), basePositions[0], basePositions[1], 0.0, bases))) {
            std::cerr << "CalculateBasePairPositions error\n";
        }

        /*
         * Now create the two opposite base pairs
         */

        rotate_vector_by_quaternion(basePositions[0], orientation, pos1);
        rotate_vector_by_quaternion(basePositions[1], orientation, pos2);
        pos1 += position;
        pos2 += position;

        Type typeF, typeB;
        if (i == 0) {
            typeF = Type::FIVE_PRIME_END;
            typeB = Type::THREE_PRIME_END;
        }
        else if (i == bases_ - 1) {
            typeF = Type::THREE_PRIME_END;
            typeB = Type::FIVE_PRIME_END;
        }
        else {
            typeF = Type::BASE;
            typeB = Type::BASE;
        }
        Fbases_.push_back(Model::Base(pos1, typeF, i, bIdStart+i));
        Bbases_.push_back(Model::Base(pos2, typeB, i, bIdStart+bases_+i));
        /*
         * Now connect them
         */
        base_objects[0] = &Fbases_[i];
        base_objects[1] = &Bbases_[i];

        Fbases_[i].setOpposite(&Bbases_[i]);
        Bbases_[i].setOpposite(&Fbases_[i]);
        // default
        Fbases_[i].setBase(Nucleobase::NONE);
        Bbases_[i].setBase(Nucleobase::NONE);

        /*
         * Now connect the previous bases to the newly created ones
         */
        if (i != 0) {
            last_base_objects[0]->setForward(base_objects[0]);
            last_base_objects[1]->setBackward(base_objects[1]);
            // strand traversal
            last_base_objects[0]->strand_forward_ = base_objects[0];
            last_base_objects[1]->strand_backward_ = base_objects[1];

        }
        if (i == 0 ) {
            forward5Prime_ = &Fbases_[i];
            backward3Prime_ = &Bbases_[i];
            Fbases_[i].setBackward((Model::Base*)nullptr);
            Bbases_[i].setForward((Model::Base*)nullptr);
            // strand traversal
            Fbases_[i].strand_backward_ = nullptr;
            Bbases_[i].strand_forward_ = nullptr;
        }
        else if (i == bases_ -1) {
            forward3Prime_ = &Fbases_[i];
            backward5Prime_ = &Bbases_[i];
            Fbases_[i].setForward((Model::Base*)nullptr);
            Bbases_[i].setForward(last_base_objects[1]);
            Fbases_[i].setBackward(last_base_objects[0]);
            Bbases_[i].setBackward((Model::Base*)nullptr);
            // strand traversal
            Fbases_[i].strand_forward_ = nullptr;
            Bbases_[i].strand_forward_ = last_base_objects[1];
            Fbases_[i].strand_backward_ = last_base_objects[0];
            Bbases_[i].strand_backward_ = nullptr;
        }
        else {
            Fbases_[i].setBackward(last_base_objects[0]);
            Bbases_[i].setForward(last_base_objects[1]);
            // strand traversal
            Fbases_[i].strand_backward_ = last_base_objects[0];
            Bbases_[i].strand_forward_ = last_base_objects[1];
        }
        for(int j = 0; j < 2; j++) {
            last_base_objects[j] = base_objects[j];
        }
    }
}

Model::Base* Model::Helix::getForwardFivePrime()
{
    return forward5Prime_;
}
Model::Base* Model::Helix::getBackwardFivePrime()
{
    return backward5Prime_;
}
Model::Base* Model::Helix::getBackwardThreePrime()
{
    return backward3Prime_;
}
Model::Base*Model::Helix::getForwardThreePrime()
{
    return forward3Prime_;
}

