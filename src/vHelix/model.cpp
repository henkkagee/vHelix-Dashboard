/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#include "model.h"

void Model::Base::setForward(Base *base)
{
    forward = base;
}
void Model::Base::setBackward(Base *base)
{
    backward = base;
}
void Model::Base::setOpposite(Base *base)
{
    opposite = base;
}
void Model::Base::setBase(Nucleobase nb)
{
    nucleobase = nb;
}

Model::Base* Model::Base::getOpposite()
{
    return opposite;
}
Model::Base* Model::Base::getForward()
{
    return forward;
}
Model::Base* Model::Base::getBackward()
{
    return backward;
}
Model::Helix* Model::Base::getParent()
{
    return parent;
}

void rotate_vector_by_quaternion(const QVector3D& v, const QQuaternion& q, QVector3D& vprime)
{
    QVector3D u(q.x(), q.y(), q.z());
    float s = q.scalar();
    vprime = 2.0f * QVector3D::dotProduct(u, v) * u
          + (s*s - QVector3D::dotProduct(u, u)) * v
          + 2.0f * s * QVector3D::crossProduct(u, v);
}



Model::Helix::Helix(const QVector3D &position, const QQuaternion &orientation, const char *name, unsigned int bases)
 : position_(position), orientation_(orientation), name_(name), bases_(bases), sequence_assigned_(false)
{
    // create the bases and position them
    QVector3D basePositions[2];
    Model::Base *base_objects[2], *last_base_objects[2];
    QVector3D pos1, pos2;
    Fbases.reserve(bases_);
    Bbases.reserve(bases_);

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
        Fbases.push_back(Model::Base(pos1, typeF, i));
        Bbases.push_back(Model::Base(pos2, typeB, i));
        /*
         * Now connect them
         */
        base_objects[0] = &Fbases[i];
        base_objects[1] = &Bbases[i];

        Fbases[i].setOpposite(&Bbases[i]);
        Bbases[i].setOpposite(&Fbases[i]);
        // default
        Fbases[i].setBase(Nucleobase::NONE);
        Bbases[i].setBase(Nucleobase::NONE);

        /*
         * Now connect the previous bases to the newly created ones
         */
        if (i != 0) {
            last_base_objects[0]->setForward(base_objects[0]);
            last_base_objects[1]->setBackward(base_objects[1]);
            // strand traversal
            last_base_objects[0]->strand_forward = base_objects[0];
            last_base_objects[1]->strand_backward = base_objects[1];

        }
        if (i == 0 ) {
            forward5Prime = &Fbases[i];
            backward3Prime = &Bbases[i];
            Fbases[i].setBackward((Model::Base*)nullptr);
            Bbases[i].setForward((Model::Base*)nullptr);
            // strand traversal
            Fbases[i].strand_backward = nullptr;
            Bbases[i].strand_forward = nullptr;
        }
        else if (i == bases_ -1) {
            forward3Prime = &Fbases[i];
            backward5Prime = &Bbases[i];
            Fbases[i].setForward((Model::Base*)nullptr);
            Bbases[i].setForward(last_base_objects[1]);
            Fbases[i].setBackward(last_base_objects[0]);
            Bbases[i].setBackward((Model::Base*)nullptr);
            // strand traversal
            Fbases[i].strand_forward = nullptr;
            Bbases[i].strand_forward = last_base_objects[1];
            Fbases[i].strand_backward = last_base_objects[0];
            Bbases[i].strand_backward = nullptr;
        }
        else {
            Fbases[i].setBackward(last_base_objects[0]);
            Bbases[i].setForward(last_base_objects[1]);
            // strand traversal
            Fbases[i].strand_backward = last_base_objects[0];
            Bbases[i].strand_forward = last_base_objects[1];
        }
        for(int j = 0; j < 2; j++) {
            last_base_objects[j] = base_objects[j];
        }
    }
}

Model::Base* Model::Helix::getForwardFivePrime()
{
    return forward5Prime;
}
Model::Base* Model::Helix::getBackwardFivePrime()
{
    return backward5Prime;
}
Model::Base* Model::Helix::getBackwardThreePrime()
{
    return backward3Prime;
}
Model::Base*Model::Helix::getForwardThreePrime()
{
    return forward3Prime;
}

Model::Base::Base(const QVector3D &arg_position, Type arg_type, int offset) :
    position(arg_position), type(arg_type), in_strand(false), offset_(offset)
{
    backward = nullptr;
    forward = nullptr;
    opposite = nullptr;
    strand_forward = nullptr;
    strand_backward = nullptr;
    checked = 43;
}

Model::Base::Base(long long int strandId, unsigned char &nb, long long int base3neigh, long long int base5neigh, unsigned long long int baseId)
    : strandId_(strandId), base3neigh_(base3neigh), base5neigh_(base5neigh), baseId_(baseId)
{
    if (nb == 'A' || nb == 'a') {
        nucleobase = Nucleobase::ADENINE;
    }
    else if (nb == 'T' || nb == 't') {
        nucleobase = Nucleobase::THYMINE;
    }
    else if (nb == 'G' || nb == 'g') {
        nucleobase = Nucleobase::GUANINE;
    }
    else if (nb == 'C' || nb == 'c') {
        nucleobase = Nucleobase::CYTOSINE;
    }
    else {
        nucleobase = Nucleobase::NONE;
    }
}
